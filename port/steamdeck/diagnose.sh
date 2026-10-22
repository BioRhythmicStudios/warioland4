#!/bin/sh
# Private Deck diagnostic. Save and settings are always disposable.
set -u
root=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd -P) || exit 2
mkdir -p "$root/diagnostics" || exit 2
session=$(mktemp -d "$root/diagnostics/run-XXXXXXXX") || exit 2
log="$root/diagnostics/wl4-deck-diagnostic-$(date +%Y%m%d-%H%M%S)-$$.log"
: > "$log" || exit 2
chmod 600 "$log"
ui() {
    [ "${WL4_DIAG_NO_UI:-0}" = 1 ] && return
    if command -v dolphin >/dev/null 2>&1; then
        dolphin --select "$log" >/dev/null 2>&1 &
    elif command -v xdg-open >/dev/null 2>&1; then
        xdg-open "$root/diagnostics" >/dev/null 2>&1 &
    fi
    if command -v kdialog >/dev/null 2>&1; then
        kdialog --msgbox "Game exit status: $1\nDiagnostic log: $log" >/dev/null 2>&1 || :
    elif command -v zenity >/dev/null 2>&1; then
        zenity --info --text="Game exit status: $1\nDiagnostic log: $log" >/dev/null 2>&1 || :
    fi
}
{
    echo "WL4 private Steam Deck diagnostic"
    echo "Started: $(date -Is)"
    echo "Package: $(basename "$root")"
    echo "Game: bin/wl4"
    echo "Save/settings: fresh disposable diagnostic session"
    uname -srm
    if [ -r /etc/os-release ]; then
        grep -E '^(NAME|VERSION|VERSION_ID)=' /etc/os-release
    fi
    getconf GNU_LIBC_VERSION 2>&1 || :
    echo "Relevant environment:"
    for name in XDG_SESSION_TYPE DISPLAY WAYLAND_DISPLAY SDL_VIDEODRIVER SDL_AUDIODRIVER; do
        value=$(printenv "$name" 2>/dev/null || :)
        printf '%s=%s\n' "$name" "$value"
    done
    for name in SteamAppId SteamGameId SDL_GAMECONTROLLERCONFIG; do
        if printenv "$name" >/dev/null 2>&1; then
            printf '%s=present\n' "$name"
        else
            printf '%s=absent\n' "$name"
        fi
    done
    for socket in "${XDG_RUNTIME_DIR:-/nonexistent}/pulse/native" "${XDG_RUNTIME_DIR:-/nonexistent}/pipewire-0"; do
        if [ -S "$socket" ]; then echo "Audio socket present: $socket"; else echo "Audio socket absent: $socket"; fi
    done
    echo "Dependency manifest:"
    cat "$root/DEPENDENCIES.json"
    echo "Game shared-library resolution (host glibc expected):"
} >> "$log" 2>&1
export LD_LIBRARY_PATH="$root/lib"
ldd "$root/bin/wl4" >> "$log" 2>&1 || :
{
    echo "SDL audio probe:"
    "$root/bin/audio-probe" || :
} >> "$log" 2>&1
rom=${1:-}
if [ -n "$rom" ]; then shift; fi
if [ -z "$rom" ] && [ -r "$HOME/Downloads/baserom_us.gba" ]; then
    rom="$HOME/Downloads/baserom_us.gba"
fi
if [ -z "$rom" ] && command -v kdialog >/dev/null 2>&1; then
    rom=$(kdialog --getopenfilename "$HOME/Downloads" '*.gba|GBA ROM files' 2>/dev/null || :)
fi
if [ -z "$rom" ] && command -v zenity >/dev/null 2>&1; then
    rom=$(zenity --file-selection --title='Select your supported US GBA ROM' 2>/dev/null || :)
fi
if [ ! -r "$rom" ]; then
    echo "No readable ROM selected; game not launched." >> "$log"
    ui "not launched"
    exit 2
fi
cp "$root/settings.template" "$session/settings.cfg" || exit 2
export WL4_SETTINGS_FILE="$session/settings.cfg"
export WL4_DIAG_EVENTS=1
export WL4_TIMING_STATS=1
echo "ROM: selected supported file (read-only; path omitted)" >> "$log"
echo "Game starting: $(date -Is)" >> "$log"
ulimit -c unlimited 2>/dev/null || :
if command -v stdbuf >/dev/null 2>&1; then
    stdbuf -oL -eL "$root/bin/wl4" "$rom" --save "$session/wl4.sav" "$@" >> "$log" 2>&1 &
else
    "$root/bin/wl4" "$rom" --save "$session/wl4.sav" "$@" >> "$log" 2>&1 &
fi
pid=$!
wait "$pid"
status=$?
{
    echo "Game PID: $pid"
    echo "Game exit status: $status"
    if [ "$status" -ge 128 ]; then
        echo "Likely signal number: $((status - 128))"
        if command -v coredumpctl >/dev/null 2>&1; then
            echo "System core dump information:"
            coredumpctl --no-pager info "$pid" 2>&1 || :
            if command -v gdb >/dev/null 2>&1 && coredumpctl dump "$pid" --output "$session/core" >/dev/null 2>&1; then
                echo "GDB core backtrace:"
                gdb -q --batch -ex 'thread apply all bt full' "$root/bin/wl4" "$session/core" 2>&1 || :
            else
                echo "GDB core backtrace unavailable; game PC or coredump info may still identify the fault."
            fi
        else
            echo "coredumpctl unavailable; game PC may still identify the fault."
        fi
    fi
    echo "Finished: $(date -Is)"
    echo "Copy this one log file back to the developer: $log"
} >> "$log" 2>&1
ui "$status"
exit "$status"
