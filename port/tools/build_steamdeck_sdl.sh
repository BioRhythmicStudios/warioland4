#!/bin/sh
# Build SDL3 for the private Steam Deck test package. No system package is installed.
set -eu
root=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd -P)
build="$root/build/steamdeck-package"
mkdir -p "$build"
if [ ! -f "$build/SDL3-3.4.2.tar.gz" ]; then
    curl -LfsS -o "$build/SDL3-3.4.2.tar.gz" \
        https://github.com/libsdl-org/SDL/releases/download/release-3.4.2/SDL3-3.4.2.tar.gz
fi
if [ ! -f "$build/libc6_2.41-12+deb13u4_amd64.deb" ]; then
    curl -LfsS -o "$build/libc6_2.41-12+deb13u4_amd64.deb" \
        'https://deb.debian.org/debian/pool/main/g/glibc/libc6_2.41-12+deb13u4_amd64.deb'
fi
(cd "$build" && printf '%s  %s\n' \
    ef39a2e3f9a8a78296c40da701967dd1b0d0d6e267e483863ce70f8a03b4050c SDL3-3.4.2.tar.gz \
    967aa62605721081c3eb2a17650611a792aa802d76a6511d1840242623d204c9 libc6_2.41-12+deb13u4_amd64.deb | sha256sum -c)
if [ ! -d "$build/SDL3-3.4.2" ]; then
    tar -xzf "$build/SDL3-3.4.2.tar.gz" -C "$build"
fi
if [ ! -f "$build/debian-glibc/usr/lib/x86_64-linux-gnu/libm.so.6" ]; then
    mkdir -p "$build/debian-glibc"
    dpkg-deb -x "$build/libc6_2.41-12+deb13u4_amd64.deb" "$build/debian-glibc"
fi
mkdir -p "$build/link-compat"
if [ ! -e "$build/link-compat/libm.so" ]; then
    ln -s ../debian-glibc/usr/lib/x86_64-linux-gnu/libm.so.6 "$build/link-compat/libm.so"
fi
cmake -S "$build/SDL3-3.4.2" -B "$build/sdl-compat-build" -G Ninja \
    -DCMAKE_BUILD_TYPE=Release -DSDL_SHARED=ON -DSDL_STATIC=OFF \
    -DSDL_TEST_LIBRARY=OFF -DSDL_TESTS=OFF \
    -DSDL_X11_SHARED=ON -DSDL_PULSEAUDIO_SHARED=ON \
    -DCMAKE_SHARED_LINKER_FLAGS="-L$build/link-compat" \
    -DCMAKE_EXE_LINKER_FLAGS="-L$build/link-compat"
cmake --build "$build/sdl-compat-build" --parallel 8
python3 "$root/port/tools/package_linux.py" "$@"
