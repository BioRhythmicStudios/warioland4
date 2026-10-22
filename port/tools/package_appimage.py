#!/usr/bin/env python3
"""Build the private Linux AppImage from the verified native ELF outputs."""
from __future__ import annotations

import hashlib
import json
import os
from pathlib import Path
import re
import shutil
import struct
import subprocess
import zlib

ROOT = Path(__file__).resolve().parents[2]
OUT = Path(os.environ.get('WL4_APPIMAGE_OUT', str(ROOT / 'build/appimage'))).resolve()
APPDIR = OUT / 'WarioLand4.AppDir'
NAME = 'Wario_Land_4-v0.1.0-beta-x86_64.AppImage'
TOOL_CACHE = ROOT / 'build/appimage-tools'


def output(*args: str, env: dict[str, str] | None = None) -> str:
    return subprocess.check_output(args, text=True, env=env, stderr=subprocess.STDOUT)


def sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def glibc_floor(path: Path) -> str:
    versions = [tuple(map(int, m)) for m in re.findall(r'GLIBC_(\d+)\.(\d+)',
                                                        output('readelf', '--version-info', str(path)))]
    return '.'.join(map(str, max(versions, default=(0, 0))))


def neutral_png(size: int = 256) -> bytes:
    """Original geometry from make_neutral_icon.py; no game artwork."""
    rows = bytearray()
    for y in range(size):
        rows.append(0)
        for x in range(size):
            rows.extend((255, 204, 64) if size // 4 <= x < 3 * size // 4
                        and size // 4 <= y < 3 * size // 4 else (40, 17, 66))

    def chunk(kind: bytes, payload: bytes) -> bytes:
        return struct.pack('>I', len(payload)) + kind + payload + struct.pack('>I', zlib.crc32(kind + payload))

    return (b'\x89PNG\r\n\x1a\n'
            + chunk(b'IHDR', struct.pack('>IIBBBBB', size, size, 8, 2, 0, 0, 0))
            + chunk(b'IDAT', zlib.compress(bytes(rows), 9))
            + chunk(b'IEND', b''))


def main() -> None:
    game = ROOT / 'build/linux/wl4'
    launcher = ROOT / 'build/linux/wl4-launcher'
    sdl = ROOT / 'build/steamdeck-package/sdl-compat-build/libSDL3.so.0'
    runtime = TOOL_CACHE / 'runtime-x86_64'
    runtime_license = TOOL_CACHE / 'runtime-LICENSE.txt'
    tool = TOOL_CACHE / 'squashfs-root/AppRun'
    sources = (game, launcher, sdl, runtime, runtime_license, tool)
    for path in sources:
        if not path.is_file():
            raise RuntimeError(f'Missing verified build input: {path}')
    for path in (game, launcher, sdl):
        if 'ELF 64-bit' not in output('file', '-L', str(path)):
            raise RuntimeError(f'Not an x86-64 ELF: {path}')
        if tuple(map(int, glibc_floor(path).split('.'))) > (2, 38):
            raise RuntimeError(f'GLIBC requirement exceeds 2.38: {path}')
    if APPDIR.exists() or (OUT / NAME).exists():
        raise RuntimeError('AppDir or AppImage already exists; preserve old candidate and choose a new build directory')

    (APPDIR / 'usr/bin').mkdir(parents=True)
    (APPDIR / 'usr/lib').mkdir()
    (APPDIR / 'usr/share/applications').mkdir(parents=True)
    (APPDIR / 'usr/share/metainfo').mkdir(parents=True)
    (APPDIR / 'usr/share/icons/hicolor/256x256/apps').mkdir(parents=True)
    (APPDIR / 'usr/share/doc/wl4/licenses').mkdir(parents=True)
    shutil.copy2(game, APPDIR / 'usr/bin/wl4')
    shutil.copy2(launcher, APPDIR / 'usr/bin/wl4-launcher')
    shutil.copy2(sdl, APPDIR / 'usr/lib/libSDL3.so.0')
    shutil.copy2(ROOT / 'port/steamdeck/play.sh', APPDIR / 'usr/play.sh')
    for filename in ('AppRun', 'wl4-appimage-launcher'):
        destination = APPDIR / (filename if filename == 'AppRun' else 'usr/bin/' + filename)
        shutil.copy2(ROOT / 'port/appimage' / filename, destination)
        destination.chmod(0o755)
    shutil.copy2(ROOT / 'port/appimage/wl4.desktop', APPDIR / 'io.github.biorhythmicstudios.warioland4.desktop')
    shutil.copy2(ROOT / 'port/appimage/wl4.desktop', APPDIR / 'usr/share/applications/io.github.biorhythmicstudios.warioland4.desktop')
    shutil.copy2(ROOT / 'port/appimage/wl4.appdata.xml', APPDIR / 'usr/share/metainfo/io.github.biorhythmicstudios.warioland4.appdata.xml')
    icon = neutral_png()
    (APPDIR / 'wl4.png').write_bytes(icon)
    (APPDIR / 'usr/share/icons/hicolor/256x256/apps/wl4.png').write_bytes(icon)
    (APPDIR / '.DirIcon').symlink_to('wl4.png')
    shutil.copy2(ROOT / 'LICENSE', APPDIR / 'usr/share/doc/wl4/licenses/wl4-LICENSE')
    shutil.copy2(ROOT / 'build/steamdeck-package/SDL3-3.4.2/LICENSE.txt',
                 APPDIR / 'usr/share/doc/wl4/licenses/SDL3-LICENSE')
    shutil.copy2(runtime_license, APPDIR / 'usr/share/doc/wl4/licenses/AppImage-runtime-LICENSE')
    for filename in ('README_APPIMAGE.md', 'RELEASE_NOTES_LINUX_0.1.0-beta.md'):
        shutil.copy2(ROOT / 'port/appimage' / filename, APPDIR / 'usr/share/doc/wl4' / filename)

    resolved = {}
    for label, binary in (('game', APPDIR / 'usr/bin/wl4'),
                          ('launcher', APPDIR / 'usr/bin/wl4-launcher')):
        links = output('ldd', str(binary), env={**os.environ, 'LD_LIBRARY_PATH': str(APPDIR / 'usr/lib')})
        if 'not found' in links or str(APPDIR / 'usr/lib/libSDL3.so.0') not in links:
            raise RuntimeError(f'{label} dependency closure failed:\n{links}')
        resolved[label] = links
    manifest = {
        'source_commit': output('git', '-C', str(ROOT), 'rev-parse', 'HEAD').strip(),
        'architecture': 'x86_64',
        'game_glibc_floor': glibc_floor(game),
        'launcher_glibc_floor': glibc_floor(launcher),
        'sdl3_glibc_floor': glibc_floor(sdl),
        'sdl3_sha256': sha256(sdl),
        'runtime_sha256': sha256(runtime),
        'runtime_release': 'AppImage/type2-runtime 20251108',
        'appimagetool_release': 'AppImage/appimagetool 1.9.1',
        'libraries_bundled': ['libSDL3.so.0'],
        'glibc_and_elf_loader': 'host-provided',
    }
    (APPDIR / 'usr/share/doc/wl4/DEPENDENCIES.json').write_text(json.dumps(manifest, indent=2) + '\n')
    env = {**os.environ, 'ARCH': 'x86_64', 'VERSION': '0.1.0-beta'}
    subprocess.run([str(tool), '--runtime-file', str(runtime), str(APPDIR), str(OUT / NAME)],
                   check=True, env=env)
    archive = OUT / NAME
    archive.chmod(0o755)
    digest = sha256(archive)
    (OUT / (NAME + '.sha256')).write_text(f'{digest}  {NAME}\n')
    print(f'AppImage: {archive}')
    print(f'SHA-256: {digest}')
    print(f'GLIBC minimum: {max(manifest["game_glibc_floor"], manifest["launcher_glibc_floor"], manifest["sdl3_glibc_floor"])}')
    print(f'Bundled: SDL3 only; host provides glibc and ELF loader')


if __name__ == '__main__':
    main()
