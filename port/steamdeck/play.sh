#!/bin/sh
set -eu
root=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd -P)
game="$root/bin/wl4"
export LD_LIBRARY_PATH="$root/lib"
if [ "${1:-}" = "--check" ]; then
    exec ldd "$game"
fi
if [ "$#" -lt 1 ]; then
    printf 'Usage: %s /absolute/path/to/baserom_us.gba [game options]\n' "$0" >&2
    exit 2
fi
rom=$1
shift
case "$rom" in
    /*) ;;
    *) rom="$PWD/$rom" ;;
esac
if [ ! -f "$rom" ] || [ ! -r "$rom" ]; then
    printf 'ROM is missing or unreadable: %s\n' "$rom" >&2
    exit 2
fi
data_dir=${WL4_USERDATA_DIR:-"$root/userdata"}
mkdir -p -- "$data_dir"
export WL4_SETTINGS_FILE=${WL4_SETTINGS_FILE:-"$data_dir/settings.cfg"}
exec "$game" "$rom" \
    --save "$data_dir/wl4.sav" "$@"
