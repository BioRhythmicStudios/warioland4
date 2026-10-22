#!/usr/bin/env python3
"""Create a private native Linux bundle using host glibc and a compatible SDL3."""
from __future__ import annotations

import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import shlex
import shutil
import subprocess
import tarfile

ROOT = Path(__file__).resolve().parents[2]
DEFAULT_NAME = "wl4-linux-private-20261022-launcher-preview"
GLIBC_PATTERN = re.compile(r"GLIBC_(\d+)\.(\d+)")


def output(*args: str, env: dict[str, str] | None = None) -> str:
    return subprocess.check_output(args, text=True, stderr=subprocess.STDOUT, env=env)


def symbol_floor(path: Path) -> str:
    versions = [tuple(map(int, match)) for match in GLIBC_PATTERN.findall(output("readelf", "--version-info", str(path)))]
    major, minor = max(versions, default=(0, 0))
    return f"{major}.{minor}"


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--name", default=DEFAULT_NAME)
    args = parser.parse_args()
    if not re.fullmatch(r"[a-z0-9-]+", args.name):
        parser.error("Package name must use lowercase letters, digits and hyphens")
    executable = ROOT / "build/linux/wl4"
    if not executable.is_file() or "ELF 64-bit" not in output("file", str(executable)):
        raise RuntimeError("Verified native Linux ELF build is missing")
    package_parent = ROOT / "build/steamdeck-package"
    stage = package_parent / args.name
    archive = package_parent / (args.name + ".tar.gz")
    if stage.exists() or archive.exists():
        raise RuntimeError("Package output already exists; choose a new --name")
    sdl_source = package_parent / "sdl-compat-build/libSDL3.so.0"
    source_tar = package_parent / "SDL3-3.4.2.tar.gz"
    source_license = package_parent / "SDL3-3.4.2/LICENSE.txt"
    if not sdl_source.is_file() or not source_tar.is_file() or not source_license.is_file():
        raise RuntimeError("The verified SDL3 3.4.2 compatibility build and source are required")
    sdl_floor = tuple(map(int, symbol_floor(sdl_source).split(".")))
    if sdl_floor > (2, 38):
        raise RuntimeError(f"SDL3 glibc floor {sdl_floor} exceeds 2.38")
    if "libSDL3.so.0" not in output("readelf", "-d", str(executable)):
        raise RuntimeError("Game executable does not require SDL3")
    probe_source = ROOT / "port/steamdeck/audio_probe.c"
    probe = package_parent / "audio-probe"
    cflags = shlex.split(output("pkg-config", "--cflags", "sdl3"))
    libs = shlex.split(output("pkg-config", "--libs", "sdl3"))
    subprocess.run(["cc", "-O2", "-g", "-fno-omit-frame-pointer", *cflags,
                    str(probe_source), "-o", str(probe), *libs], check=True)
    if tuple(map(int, symbol_floor(probe).split("."))) > (2, 38):
        raise RuntimeError("Audio probe exceeds GLIBC 2.38")
    stage.mkdir(parents=True)
    (stage / "bin").mkdir()
    (stage / "lib").mkdir()
    (stage / "licenses").mkdir()
    (stage / "userdata").mkdir()
    (stage / "diagnostics").mkdir()
    launcher = ROOT / "build/linux/wl4-launcher"
    if not launcher.is_file() or "ELF 64-bit" not in output("file", str(launcher)):
        raise RuntimeError("Native Linux graphical launcher build is missing")
    shutil.copy2(executable, stage / "bin/wl4")
    shutil.copy2(launcher, stage / "bin/wl4-launcher")
    shutil.copy2(probe, stage / "bin/audio-probe")
    shutil.copy2(sdl_source, stage / "lib/libSDL3.so.0")
    shutil.copy2(ROOT / "LICENSE", stage / "licenses/wl4-LICENSE")
    shutil.copy2(source_license, stage / "licenses/SDL3-LICENSE")
    launcher_floor = tuple(map(int, symbol_floor(launcher).split(".")))
    if launcher_floor > (2, 38):
        raise RuntimeError(f"Launcher glibc floor {launcher_floor} exceeds 2.38")
    manifest = {
        "source_commit": output("git", "-C", str(ROOT), "rev-parse", "HEAD").strip(),
        "game_glibc_floor": symbol_floor(executable),
        "launcher_glibc_floor": symbol_floor(launcher),
        "audio_probe_glibc_floor": symbol_floor(probe),
        "sdl3_glibc_floor": symbol_floor(sdl_source),
        "glibc": "host-provided; no glibc or ELF loader bundled",
        "sdl3_source": "https://github.com/libsdl-org/SDL/releases/download/release-3.4.2/SDL3-3.4.2.tar.gz",
        "sdl3_source_sha256": hashlib.sha256(source_tar.read_bytes()).hexdigest(),
        "linker_libm_source": "Debian 13 libc6 2.41-12+deb13u4; linked only, not bundled",
        "linker_libm_package_sha256": "967aa62605721081c3eb2a17650611a792aa802d76a6511d1840242623d204c9",
        "sdl3_sha256": hashlib.sha256(sdl_source.read_bytes()).hexdigest(),
        "audio_backends": "PipeWire, PulseAudio, and ALSA dynamically loaded from host",
        "pipewire_build_headers": "Ubuntu 26.04 libpipewire/spa 1.6.2 extracted at build time; not bundled",
    }
    resolved = output("ldd", str(executable), env={**os.environ,
                      "LD_LIBRARY_PATH": str(stage / "lib")})
    if "not found" in resolved or str(stage / "lib/libSDL3.so.0") not in resolved:
        raise RuntimeError("Bundled SDL3 did not resolve correctly")
    launcher_resolved = output("ldd", str(launcher), env={**os.environ,
                               "LD_LIBRARY_PATH": str(stage / "lib")})
    if "not found" in launcher_resolved or str(stage / "lib/libSDL3.so.0") not in launcher_resolved:
        raise RuntimeError("Launcher did not resolve bundled SDL3 correctly")
    for name in ("play.sh", "play_gui.sh", "configure.sh", "diagnose.sh"):
        shutil.copy2(ROOT / "port/steamdeck" / name, stage / name)
        (stage / name).chmod(0o755)
    for name in ("Play-Wario-Land-4.desktop", "Diagnose-Wario-Land-4.desktop"):
        shutil.copy2(ROOT / "port/steamdeck" / name, stage / name)
        (stage / name).chmod(0o755)
    shutil.copy2(ROOT / "port/steamdeck/settings.template", stage / "settings.template")
    shutil.copy2(ROOT / "port/steamdeck/README.md", stage / "README.md")
    (stage / "DEPENDENCIES.json").write_text(json.dumps(manifest, indent=2) + "\n")
    with tarfile.open(archive, "w:gz") as bundle:
        bundle.add(stage, arcname=stage.name, recursive=True)
    checksum = hashlib.sha256(archive.read_bytes()).hexdigest()
    (package_parent / (archive.name + ".sha256")).write_text(f"{checksum}  {archive.name}\n")
    print(f"Archive: {archive}")
    print(f"SHA-256: {checksum}")
    print("Bundled libraries: SDL3 only; glibc and loader: host")
    print(f"Game GLIBC floor: {manifest['game_glibc_floor']}; SDL3 GLIBC floor: {manifest['sdl3_glibc_floor']}")


if __name__ == "__main__":
    main()
