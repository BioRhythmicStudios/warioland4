"""Generate objdiff.json and objdiff target objects.

This project is fully decompiled, so there are no original per-file objects to diff
against. Target objects are made from the current build: every byte each C object
contributes to the ROM is checked against the base ROM (relocation sites excepted, as
they hold addends rather than final values), and only then is the object copied to
expected/<version>/obj/ as the target. Run `make` first so the build passes its md5
check.

The base objects are copies of the build's objects with every function that contains
hand-written asm renamed (tools/asm_functions.py), so objdiff counts those as unmatched.
"""

from argparse import ArgumentParser
from pathlib import Path
import json
import re
import shutil
import struct
import subprocess
import sys

ROM_BASE = 0x8000000
ROM_SIZE = 0x800000

SHT_PROGBITS = 1
SHT_REL = 9

# Bytes in place at each relocation type's offset
RELOC_SIZES = {
    2: 4,  # R_ARM_ABS32
    10: 4,  # R_ARM_THM_CALL
    29: 4,  # R_ARM_JUMP24
    40: 0,  # R_ARM_V4BX: the instruction itself is final
    103: 2,  # R_ARM_THM_JUMP8
}

map_section_regex = re.compile(r"^ (\.\w+)\s+0x([0-9a-f]+)\s+0x([0-9a-f]+)\s+(\S+\.o)$")


def read_sections(path: Path):
    data = path.read_bytes()
    if data[:4] != b"\x7fELF" or data[4] != 1 or data[5] != 1:
        raise ValueError(f"{path}: not a 32-bit little-endian ELF")
    shoff, = struct.unpack_from("<I", data, 0x20)
    shentsize, shnum, shstrndx = struct.unpack_from("<HHH", data, 0x2E)
    headers = [struct.unpack_from("<IIIIIIIIII", data, shoff + i * shentsize) for i in range(shnum)]
    strtab_off = headers[shstrndx][4]

    def name(offset):
        end = data.index(b"\0", strtab_off + offset)
        return data[strtab_off + offset:end].decode()

    sections = {}
    for header in headers:
        sh_name, sh_type, _, _, sh_offset, sh_size, _, sh_info, _, _ = header
        sections[name(sh_name)] = (sh_type, data[sh_offset:sh_offset + sh_size], sh_info)
    return sections


def reloc_mask(sections, section_name, size):
    mask = bytearray(size)
    rel = sections.get(f".rel{section_name}")
    if rel is None:
        return mask
    _, contents, _ = rel
    for offset, info in struct.iter_unpack("<II", contents):
        kind = info & 0xFF
        if kind not in RELOC_SIZES:
            raise ValueError(f"unhandled relocation type {kind} in {section_name}")
        for i in range(RELOC_SIZES[kind]):
            mask[offset + i] = 1
    return mask


def parse_map(path: Path):
    placements = {}
    in_layout = False
    for line in path.read_text().splitlines():
        if line.startswith("Linker script and memory map"):
            in_layout = True
            continue
        if not in_layout:
            continue
        match = map_section_regex.match(line)
        if match is None:
            continue
        section, addr, size, obj = match.groups()
        placements[(obj, section)] = (int(addr, 16), int(size, 16))
    return placements


def main():
    parser = ArgumentParser(description=__doc__)
    parser.add_argument("-v", "--version", default="us", choices=["us", "jp"])
    parser.add_argument("--make", default="wsl", help="custom_make for objdiff.json (default: wsl)")
    args = parser.parse_args()

    root = Path(__file__).resolve().parent.parent
    build = Path("build") / args.version
    expected = Path("expected") / args.version
    md5file = root / f"warioland4_{args.version}.md5"

    if subprocess.run(["md5sum", "--quiet", "-c", md5file.name], cwd=root).returncode != 0:
        sys.exit(f"{build}/warioland4_{args.version}.gba does not match; run make first")

    rom = (root / f"baserom_{args.version}.gba").read_bytes()
    placements = parse_map(root / build / f"warioland4_{args.version}.map")

    units = []
    checked = 0
    for source in sorted((root / "src").rglob("*.c")):
        rel = source.relative_to(root / "src").with_suffix("")
        base = build / "obj" / rel.with_suffix(".o")
        target = expected / "obj" / rel.with_suffix(".o")
        sections = read_sections(root / base)

        for section_name, (sh_type, contents, _) in sections.items():
            if sh_type != SHT_PROGBITS or not contents or section_name.startswith(".ARM"):
                continue
            placement = placements.get((base.as_posix(), section_name))
            if placement is None:
                # Not linked into the ROM (e.g. unused .rodata), nothing to check against
                continue
            addr, size = placement
            if size != len(contents) or not ROM_BASE <= addr < ROM_BASE + ROM_SIZE:
                sys.exit(f"{base}({section_name}): unexpected placement 0x{addr:x}+0x{size:x}")
            mask = reloc_mask(sections, section_name, size)
            original = rom[addr - ROM_BASE:addr - ROM_BASE + size]
            for i, (ours, theirs) in enumerate(zip(contents, original)):
                if not mask[i] and ours != theirs:
                    sys.exit(f"{base}({section_name})+0x{i:x}: differs from base ROM at 0x{addr + i:x}")
            checked += size

        (root / target).parent.mkdir(parents=True, exist_ok=True)
        shutil.copyfile(root / base, root / target)
        units.append({
            "name": rel.as_posix(),
            "target_path": target.as_posix(),
            # Functions written in asm are renamed in this copy so they count as unmatched (see Makefile)
            "base_path": (build / "objdiff" / rel.with_suffix(".o")).as_posix(),
            "metadata": {"source_path": source.relative_to(root).as_posix()},
        })

    bases = [unit["base_path"] for unit in units]
    if subprocess.run(["make", "-s", f"VERSION={args.version}", *bases], cwd=root).returncode != 0:
        sys.exit("building objdiff base objects failed")

    config = {
        "$schema": "https://raw.githubusercontent.com/encounter/objdiff/main/config.schema.json",
        "custom_make": args.make,
        # objdiff passes Windows paths; the wrapper turns their backslashes into slashes for make
        "custom_args": ["--exec", "sh", "tools/wsl_make.sh"] if args.make == "wsl" else [],
        "build_target": False,
        "build_base": True,
        "watch_patterns": ["*.c", "*.h", "*.s", "*.inc", "*.ld"],
        "units": units,
    }
    (root / "objdiff.json").write_text(json.dumps(config, indent=2) + "\n")
    print(f"objdiff.json: {len(units)} units, 0x{checked:x} bytes checked against the base ROM")


if __name__ == "__main__":
    main()
