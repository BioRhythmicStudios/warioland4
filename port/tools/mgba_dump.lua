-- Frame dumps from mGBA, in the format of wl4 --dump, for port/tools/diff_frames.py.
--
-- In mGBA 0.10 or later (Qt): load the ROM, then Tools > Scripting... > File > Load script...
-- and pick this file. The script resets the game, writes GBA memory at the start of each
-- VBlank for FRAMES frames, and says in the scripting console when it is done.
--
-- mGBA starts the game when it loads the ROM, so by now the game may have written its save.
-- The script erases SRAM before the reset, to boot as the port does: with no save.
-- The output goes to %WL4_DUMP% if set, else to %TEMP%\wl4_mgba.bin.

local FRAMES = tonumber(os.getenv("WL4_FRAMES") or "") or 1200
local path = os.getenv("WL4_DUMP") or ((os.getenv("TEMP") or ".") .. "\\wl4_mgba.bin")

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
    file:write(string.pack("<I4I4", MAGIC, count))
    for _, region in ipairs(REGIONS) do
        file:write(emu.memory[region[1]]:readRange(0, region[2]))
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
callback = callbacks:add("frame", dump)
console:log(string.format("wl4: dumping %d frames to %s", FRAMES, path))
