#!/bin/sh
# Graphical private launcher. Saves stay inside this extracted package.
set -u
root=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd -P) || exit 2
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
    if command -v kdialog >/dev/null 2>&1; then
        kdialog --error 'Select your supported US Wario Land 4 ROM to play.' || :
    elif command -v zenity >/dev/null 2>&1; then
        zenity --error --text='Select your supported US Wario Land 4 ROM to play.' || :
    fi
    exit 2
fi
exec "$root/play.sh" "$rom" "$@"
