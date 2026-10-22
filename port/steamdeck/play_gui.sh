#!/bin/sh
# Open the native Linux graphical launcher with package-local settings/saves.
set -eu
root=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd -P)
mkdir -p "$root/userdata"
export LD_LIBRARY_PATH="$root/lib"
export WL4_SETTINGS_FILE="$root/userdata/settings.cfg"
exec "$root/bin/wl4-launcher" "$root" "$@"
