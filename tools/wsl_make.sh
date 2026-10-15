#!/bin/sh
# make for objdiff on Windows: objdiff passes object paths with backslashes
# (build\us\objdiff\foo.o), so convert them before calling make.
# Run as `wsl --exec sh tools/wsl_make.sh <targets>` so no shell strips them first.
for arg in "$@"; do
    set -- "$@" "$(printf '%s' "$arg" | tr '\\' '/')"
    shift
done
exec make "$@"
