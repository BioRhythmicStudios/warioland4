#!/bin/sh
set -eu
root=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd -P)
usage() {
    printf 'Usage: %s vsync on|off\n       %s crt original|lite|enhanced\n       %s rumble on|off\n' "$0" "$0" "$0" >&2
    exit 2
}
[ "$#" -eq 2 ] || usage
case "$1:$2" in
    vsync:off) field=5; value=0 ;;
    vsync:on) field=5; value=1 ;;
    crt:original) field=6; value=0 ;;
    crt:lite) field=6; value=1 ;;
    crt:enhanced) field=6; value=2 ;;
    rumble:off) field=10; value=0 ;;
    rumble:on) field=10; value=1 ;;
    *) usage ;;
esac
mkdir -p -- "$root/userdata"
config="$root/userdata/settings.cfg"
if [ ! -e "$config" ]; then
    cp -- "$root/settings.template" "$config"
fi
temporary=$(mktemp "$config.tmp.XXXXXX")
if ! awk -v field="$field" -v value="$value" '
    NR == 1 { if ($0 != "WL4_SETTINGS 4") exit 1; print; next }
    NR == 2 { if (NF != 11) exit 1; $field=value; print; next }
    { print }
    END { if (NR != 4) exit 1 }
' "$config" > "$temporary"; then
    printf 'Unsupported settings file; original retained. See %s\n' "$config" >&2
    exit 1
fi
mv -- "$temporary" "$config"
printf 'Saved %s=%s in %s\n' "$1" "$2" "$config"
