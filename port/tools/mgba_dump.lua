-- Frame dumps from mGBA, in the format of wl4 --dump, for port/tools/diff_frames.py.
--
-- In mGBA 0.10 or later (Qt): load the ROM, then Tools > Scripting... > File > Load script...
-- and pick this file. The script resets the game, writes GBA memory at the start of each
-- VBlank for FRAMES frames, and says in the scripting console when it is done.
--
-- mGBA starts the game when it loads the ROM, so by now the game may have written its save.
-- The script erases SRAM before the reset, to boot as the port does: with no save.
-- The output goes to %WL4_DUMP% if set, else to %TEMP%\wl4_mgba.bin.
-- With WL4_SHOTS=1 the script also saves a screenshot of each frame before dumping it. mGBA
-- saves them next to the ROM, numbered from 0: give it a fresh copy of the ROM in an empty
-- directory, and screenshot N is frame N+1. port/tools/run_mgba_dump.ps1 does all this
-- without anyone at the keyboard.
-- With WL4_INPUT set to an input script (the format of wl4 --input, described in
-- port/include/port/input_script.h), the script presses its keys on the same steps of the
-- game's main loop as wl4 does. It counts the steps as wl4 does (PortFrameSteps()): at the
-- start of each VBlank, a step has finished if gMainTimer has changed and the main loop waits
-- for VBlank, with bit 0 of gInterruptCheck clear.

local FRAMES = tonumber(os.getenv("WL4_FRAMES") or "") or 1200
local path = os.getenv("WL4_DUMP") or ((os.getenv("TEMP") or ".") .. "\\wl4_mgba.bin")
local shots = os.getenv("WL4_SHOTS") == "1"
local inputPath = os.getenv("WL4_INPUT")

local MAIN_TIMER = 0x03000C41 -- gMainTimer
local INTERRUPT_CHECK = 0x03000C42 -- gInterruptCheck
local BUTTONS = {
    A = 0x001, B = 0x002, SELECT = 0x004, START = 0x008, RIGHT = 0x010,
    LEFT = 0x020, UP = 0x040, DOWN = 0x080, R = 0x100, L = 0x200,
}

-- The input script as a list of { step, keys }, keys as a bit mask of pressed buttons.
local function loadInput(name)
    local entries = {}
    local number = 0
    for line in assert(io.open(name, "r")):lines() do
        number = number + 1
        line = line:gsub("#.*", "")
        if line:find("%S") then
            local step, names = line:match("^%s*(%d+)%s+(%S+)%s*$")
            step = tonumber(step)
            local keys = 0
            if names and names ~= "-" then
                for button in names:gmatch("[^+]+") do
                    local bit = BUTTONS[button:upper()]
                    if not bit then
                        step = nil
                        break
                    end
                    keys = keys | bit
                end
            end
            if not step or step == 0 or (#entries > 0 and step <= entries[#entries][1]) then
                error(string.format("%s:%d: expected \"STEP KEYS\" with increasing steps and known buttons", name, number))
            end
            entries[#entries + 1] = { step, keys }
        end
    end
    return entries
end

local input = inputPath and loadInput(inputPath)
local steps = 0
local lastStepTimer = 0

-- The keys the input script holds on a step.
local function keysFor(step)
    local keys = 0
    for _, entry in ipairs(input) do
        if entry[1] > step then
            break
        end
        keys = entry[2]
    end
    return keys
end

-- mGBA memory domains, in the order of port/tools/frame_dump.py's REGIONS.
local REGIONS = {
    { "wram", 0x40000 },
    { "iwram", 0x8000 },
    { "io", 0x400 },
    { "palette", 0x400 },
    { "vram", 0x18000 },
    { "oam", 0x400 },
    { "sram", 0x8000 },
}
local MAGIC = 0x46344C57 -- "WL4F"

local file = assert(io.open(path, "wb"))
local count = 0
local callback

-- The "frame" callback runs when mGBA ends a frame: at the start of VBlank, before the CPU
-- has taken the VBlank interrupt. wl4 --dump writes at the same point.
local function dump()
    count = count + 1
    if shots then
        emu:screenshot()
    end
    file:write(string.pack("<I4I4", MAGIC, count))
    for _, region in ipairs(REGIONS) do
        file:write(emu.memory[region[1]]:readRange(0, region[2]))
    end
    if input then
        local timer = emu:read8(MAIN_TIMER)
        if (emu:read16(INTERRUPT_CHECK) & 1) == 0 and timer ~= lastStepTimer then
            steps = steps + 1
            lastStepTimer = timer
        end
        -- The keys for the step the main loop runs next, which reads them when it starts.
        emu:setKeys(keysFor(steps + 1))
    end
    if count >= FRAMES then
        file:close()
        callbacks:remove(callback)
        console:log(string.format("wl4: %d frames written to %s", count, path))
    end
end

for i = 0, 0x7FFF do
    emu:write8(0x0E000000 + i, 0xFF)
end
emu:reset()
if input then
    emu:setKeys(keysFor(1))
end
callback = callbacks:add("frame", dump)
console:log(string.format("wl4: dumping %d frames to %s%s", FRAMES, path,
    input and (", with the keys from " .. inputPath) or ""))
