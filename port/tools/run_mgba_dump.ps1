<#
.SYNOPSIS
Makes a frame dump in mGBA without anyone at the keyboard: starts mGBA, loads
port/tools/mgba_dump.lua and waits for the dump to finish.

.DESCRIPTION
mGBA 0.10 has no command-line option to run a script, so this drives its window:
1. Copies the ROM into a new run directory (by default under %TEMP%), so that mGBA
   finds no old save next to it and writes its screenshots there.
2. Starts mGBA with WL4_FRAMES, WL4_DUMP, WL4_SHOTS and WL4_INPUT set for the script.
3. Opens Tools > Scripting... and File > Load script... through UI Automation.
4. Fills in and confirms the file dialog with Win32 messages: UI Automation calls into mGBA
   time out while its modal dialog is open, so the menu item is invoked from another runspace.
5. Waits until the script has written and closed the dump, then closes mGBA.

The script itself (mgba_dump.lua) erases SRAM, or with -Save loads a save file into it,
resets the game and dumps memory at the start of each VBlank. With -Shots it also saves a screenshot each frame, which mGBA names
wl4-0.png, wl4-1.png, ... in the run directory: wl4-N.png goes with the N-th frame dumped,
from 0. With -InputScript it presses the script's keys on the same main-loop steps as wl4
--input does, and with -FromStep it dumps from that step on, as wl4 --from-step does.

Tested with mGBA 0.10.5 (Qt, win32) on Windows 11. The mGBA window may be in the
background; nothing needs the keyboard or mouse.

.EXAMPLE
powershell -ExecutionPolicy Bypass -File port/tools/run_mgba_dump.ps1 `
    -Mgba C:\path\to\mGBA.exe -Frames 1800 -Out $env:TEMP\mgba.bin -Shots
python port/tools/diff_frames.py port.bin $env:TEMP\mgba.bin
#>
param(
    # mGBA.exe; defaults to %WL4_MGBA%.
    [string]$Mgba = $env:WL4_MGBA,
    [int]$Frames = 1200,
    # The dump to write.
    [Parameter(Mandatory = $true)][string]$Out,
    # Also save a screenshot of every frame in the run directory.
    [switch]$Shots,
    # An input script for the keys, as for wl4 --input (port/include/port/input_script.h).
    [string]$InputScript,
    # Start the dump and screenshots at the frame on which this main-loop step finishes, as
    # wl4 --from-step does. -Frames still counts from the start.
    [int]$FromStep = 0,
    # A save file to boot with (32 KiB, as wl4 --save writes); without it SRAM is erased.
    [string]$Save,
    # The US ROM. Defaults to baserom_us.gba in this worktree, else in the main worktree
    # (a worktree's copy may be a WSL symlink, which Windows programs cannot read).
    [string]$Rom,
    # Where the run directory is made; it is named after the time and printed.
    [string]$RunRoot = $env:TEMP,
    # The Lua script to load; defaults to mgba_dump.lua beside this one.
    [string]$Script,
    [int]$TimeoutSeconds = 900
)

$ErrorActionPreference = 'Stop'
Add-Type -AssemblyName UIAutomationClient, UIAutomationTypes

Add-Type -TypeDefinition @'
using System;
using System.Runtime.InteropServices;
using System.Text;

public static class Wl4Win32
{
    delegate bool EnumWindowsProc(IntPtr hwnd, IntPtr lParam);

    [DllImport("user32.dll")] static extern bool EnumWindows(EnumWindowsProc proc, IntPtr lParam);
    [DllImport("user32.dll")] static extern uint GetWindowThreadProcessId(IntPtr hwnd, out uint pid);
    [DllImport("user32.dll")] static extern bool IsWindowVisible(IntPtr hwnd);
    [DllImport("user32.dll", CharSet = CharSet.Unicode)] static extern int GetClassName(IntPtr hwnd, StringBuilder name, int size);
    [DllImport("user32.dll")] static extern IntPtr GetDlgItem(IntPtr dialog, int id);
    [DllImport("user32.dll", CharSet = CharSet.Unicode)] static extern IntPtr FindWindowEx(IntPtr parent, IntPtr after, string cls, string title);
    [DllImport("user32.dll", CharSet = CharSet.Unicode)] static extern IntPtr SendMessage(IntPtr hwnd, uint msg, IntPtr wParam, string lParam);
    [DllImport("user32.dll")] static extern bool PostMessage(IntPtr hwnd, uint msg, IntPtr wParam, IntPtr lParam);

    const uint WM_SETTEXT = 0x000C;
    const uint BM_CLICK = 0x00F5;
    const int IDOK = 1;
    const int FILE_NAME_COMBO = 0x47C;  // the file name box of the common file dialog

    // A visible standard dialog (class #32770) of the process, or zero.
    public static IntPtr FindDialog(int pid)
    {
        IntPtr found = IntPtr.Zero;
        EnumWindows(delegate (IntPtr hwnd, IntPtr lParam) {
            uint owner;
            StringBuilder cls = new StringBuilder(64);
            GetWindowThreadProcessId(hwnd, out owner);
            if (owner == (uint)pid && IsWindowVisible(hwnd) && GetClassName(hwnd, cls, cls.Capacity) > 0 && cls.ToString() == "#32770") {
                found = hwnd;
                return false;
            }
            return true;
        }, IntPtr.Zero);
        return found;
    }

    // Types the path into a file dialog's name box and presses Open. Returns false if the
    // dialog does not have the expected controls.
    public static bool OpenFile(IntPtr dialog, string path)
    {
        IntPtr combo = GetDlgItem(dialog, FILE_NAME_COMBO);
        IntPtr edit = IntPtr.Zero;
        if (combo != IntPtr.Zero) {
            IntPtr inner = FindWindowEx(combo, IntPtr.Zero, "ComboBox", null);
            edit = FindWindowEx(inner != IntPtr.Zero ? inner : combo, IntPtr.Zero, "Edit", null);
        } else {
            edit = GetDlgItem(dialog, 0x480);  // the old-style dialog's name box
        }
        IntPtr ok = GetDlgItem(dialog, IDOK);
        if (edit == IntPtr.Zero || ok == IntPtr.Zero)
            return false;
        SendMessage(edit, WM_SETTEXT, IntPtr.Zero, path);
        return PostMessage(ok, BM_CLICK, IntPtr.Zero, IntPtr.Zero);
    }
}
'@

$UIA = [System.Windows.Automation.AutomationElement]
$Tree = [System.Windows.Automation.TreeScope]

function Find-Element([System.Windows.Automation.AutomationElement]$Parent, [int]$ProcessId,
                      [System.Windows.Automation.ControlType]$Type, [string]$Name, [int]$Seconds = 20) {
    $conditions = [System.Windows.Automation.Condition[]]@(
        (New-Object System.Windows.Automation.PropertyCondition($UIA::ProcessIdProperty, $ProcessId)),
        (New-Object System.Windows.Automation.PropertyCondition($UIA::ControlTypeProperty, $Type)),
        (New-Object System.Windows.Automation.PropertyCondition($UIA::NameProperty, $Name)))
    $condition = New-Object System.Windows.Automation.AndCondition($conditions)
    $deadline = (Get-Date).AddSeconds($Seconds)
    do {
        $element = $Parent.FindFirst($Tree::Descendants, $condition)
        if ($element) { return $element }
        Start-Sleep -Milliseconds 250
    } while ((Get-Date) -lt $deadline)
    throw "mGBA: no $($Type.ProgrammaticName) named '$Name' appeared"
}

# Opens a menu-bar menu, or runs a menu item.
function Select-MenuItem([System.Windows.Automation.AutomationElement]$Item) {
    $pattern = $null
    if ($Item.TryGetCurrentPattern([System.Windows.Automation.ExpandCollapsePattern]::Pattern, [ref]$pattern)) {
        $pattern.Expand()
    } elseif ($Item.TryGetCurrentPattern([System.Windows.Automation.InvokePattern]::Pattern, [ref]$pattern)) {
        $pattern.Invoke()
    } else {
        throw "mGBA: menu item '$($Item.Current.Name)' cannot be selected"
    }
}

# --- Inputs ----------------------------------------------------------------------------------

if (-not $Mgba -or -not (Test-Path -LiteralPath $Mgba -PathType Leaf)) {
    throw 'Pass -Mgba C:\path\to\mGBA.exe or set WL4_MGBA (mGBA 0.10 or later, the Qt build).'
}
if (-not $Script) { $Script = Join-Path $PSScriptRoot 'mgba_dump.lua' }  # not usable in param() on 5.1
$Script = (Resolve-Path -LiteralPath $Script).Path
$expectedMd5 = ((Get-Content (Join-Path $PSScriptRoot '..\..\warioland4_us.md5')) -split '\s+')[0].ToUpper()
if (-not $Rom) {
    $candidates = @(Join-Path $PSScriptRoot '..\..\baserom_us.gba')
    $main = (git -C $PSScriptRoot worktree list --porcelain | Select-Object -First 1) -replace '^worktree ', ''
    if ($main) { $candidates += Join-Path $main 'baserom_us.gba' }
    $Rom = $candidates | Where-Object { (Test-Path -LiteralPath $_) -and (Get-Item -LiteralPath $_).Length -gt 0 } | Select-Object -First 1
    if (-not $Rom) { throw "No readable baserom_us.gba in $($candidates -join ' or '); pass -Rom." }
}
if ((Get-FileHash -LiteralPath $Rom -Algorithm MD5).Hash -ne $expectedMd5) {
    throw "$Rom is not the US ROM (MD5 $expectedMd5)."
}

$runDir = Join-Path $RunRoot ('wl4-mgba-' + (Get-Date -Format 'yyyyMMdd-HHmmss'))
New-Item -ItemType Directory -Path $runDir | Out-Null
Copy-Item -LiteralPath $Rom -Destination (Join-Path $runDir 'wl4.gba')
$Out = [System.IO.Path]::GetFullPath($Out)
$frameSize = 8 + 0x40000 + 0x8000 + 0x400 + 0x400 + 0x18000 + 0x400 + 0x8000  # header + regions
Write-Host "Run directory: $runDir"

# --- Run -------------------------------------------------------------------------------------

$env:WL4_FRAMES = "$Frames"
$env:WL4_DUMP = $Out
$env:WL4_SHOTS = if ($Shots) { '1' } else { '' }
$env:WL4_INPUT = if ($InputScript) { (Resolve-Path -LiteralPath $InputScript).Path } else { '' }
$env:WL4_FROM_STEP = if ($FromStep) { "$FromStep" } else { '' }
$env:WL4_SAVE = if ($Save) { (Resolve-Path -LiteralPath $Save).Path } else { '' }
$done = "$Out.done"
Remove-Item -LiteralPath $done -ErrorAction SilentlyContinue
$started = Get-Date
$mgbaProcess = Start-Process -FilePath $Mgba -ArgumentList ('"' + (Join-Path $runDir 'wl4.gba') + '"') -PassThru
$runspace = $null
try {
    $deadline = (Get-Date).AddSeconds(30)
    while ($mgbaProcess.MainWindowHandle -eq 0) {
        if ((Get-Date) -gt $deadline) { throw 'mGBA: no main window' }
        Start-Sleep -Milliseconds 250
        $mgbaProcess.Refresh()
    }
    $window = $UIA::FromHandle($mgbaProcess.MainWindowHandle)
    $menuItem = [System.Windows.Automation.ControlType]::MenuItem

    Select-MenuItem (Find-Element $window $mgbaProcess.Id $menuItem 'Tools')
    Select-MenuItem (Find-Element $UIA::RootElement $mgbaProcess.Id $menuItem 'Scripting...')
    $scripting = Find-Element $UIA::RootElement $mgbaProcess.Id ([System.Windows.Automation.ControlType]::Window) 'Scripting'
    Select-MenuItem (Find-Element $scripting $mgbaProcess.Id $menuItem 'File')
    $load = Find-Element $UIA::RootElement $mgbaProcess.Id $menuItem 'Load script...'

    # Invoking the item opens a modal dialog, and the call does not return until it closes.
    $runspace = [powershell]::Create().AddScript({
        param($item)
        try { $item.GetCurrentPattern([System.Windows.Automation.InvokePattern]::Pattern).Invoke() } catch { }
    }).AddArgument($load)
    $null = $runspace.BeginInvoke()

    $deadline = (Get-Date).AddSeconds(20)
    while (($dialog = [Wl4Win32]::FindDialog($mgbaProcess.Id)) -eq [IntPtr]::Zero) {
        if ((Get-Date) -gt $deadline) { throw 'mGBA: the Load script dialog did not open' }
        Start-Sleep -Milliseconds 250
    }
    Start-Sleep -Milliseconds 500  # let the dialog finish building its controls
    if (-not [Wl4Win32]::OpenFile($dialog, $Script)) { throw 'mGBA: the file dialog has no name box or Open button' }
    Write-Host "Loaded $Script; dumping $Frames frames to $Out"

    # The script closes the dump after the last frame, then writes the .done file.
    $deadline = $started.AddSeconds($TimeoutSeconds)
    while (-not (Test-Path -LiteralPath $done)) {
        if ($mgbaProcess.HasExited) { throw 'mGBA exited before the dump was finished' }
        if ((Get-Date) -gt $deadline) { throw "The dump was not finished within $TimeoutSeconds s" }
        Start-Sleep -Seconds 1
    }
    Remove-Item -LiteralPath $done
    $written = (Get-Item -LiteralPath $Out).Length / $frameSize
    Write-Host ("Done: {0} frames run, {1} written, in {2:N0} s" -f $Frames, $written, ((Get-Date) - $started).TotalSeconds)
    if ($Shots) {
        $count = @(Get-ChildItem -LiteralPath $runDir -Filter 'wl4-*.png').Count
        Write-Host "$count screenshots in $runDir (wl4-N.png goes with the N-th frame dumped, from 0)"
    }
} finally {
    if (-not $mgbaProcess.HasExited) {
        $null = $mgbaProcess.CloseMainWindow()
        if (-not $mgbaProcess.WaitForExit(10000)) { Stop-Process -Id $mgbaProcess.Id -Force }
    }
    if ($runspace) { $runspace.Dispose() }
}
