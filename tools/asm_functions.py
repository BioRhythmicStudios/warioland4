"""Find the hand-written assembly in the C sources.

Each asm statement is classified as:
- register: `register T x asm("rN")`, a register binding on a C variable (C, not asm)
- rename: `T f(...) asm("name")`, a symbol name for a declaration (C, not asm)
- barrier: an asm statement with no instructions, e.g. `asm volatile("" : "+r"(x))`
- inline: instructions inside a C function
- toplevel: instructions outside any function, which is how functions written
  entirely in asm are defined

A function is asm-only when it is defined by top-level asm, or when its C body is
nothing but asm statements (and declarations).

  asm_functions.py --redefine-syms src/foo.c   objcopy renames hiding functions with asm (asm-only
                                               or inline) from objdiff, so they count as unmatched
  asm_functions.py --report [-v us]            summary for the whole project
"""

from argparse import ArgumentParser
from pathlib import Path
import re
import struct
import subprocess

token_regex = re.compile(
    r"""
    (?P<ws>\s+)
  | (?P<comment>//[^\n]*|/\*.*?\*/)
  | (?P<pp>^[ \t]*\#(?:\\\r?\n|[^\n])*)
  | (?P<string>"(?:\\.|[^"\\])*")
  | (?P<char>'(?:\\.|[^'\\])*')
  | (?P<ident>[A-Za-z_]\w*)
  | (?P<number>\.?\d[\w.]*)
  | (?P<punct>.)
    """,
    re.VERBOSE | re.DOTALL | re.MULTILINE,
)

register_regex = re.compile(r"^(r\d+|sl|fp|ip|sp|lr|pc)$")
label_regex = re.compile(r"^\s*([A-Za-z_.$][\w.$]*)\s*:")
directive_regex = re.compile(r"^\s*\.(\w+)\s*(.*)$")
STATEMENT_KEYWORDS = {"if", "else", "while", "for", "do", "switch", "return", "goto", "case", "default", "break", "continue"}
TYPE_KEYWORDS = {
    "register", "static", "const", "volatile", "unsigned", "signed", "struct", "union", "enum",
    "int", "char", "short", "long", "void", "u8", "u16", "u32", "s8", "s16", "s32", "bool8", "bool32",
}
DATA_DIRECTIVES = {"word", "4byte", "2byte", "byte", "hword", "short", "long", "space", "fill", "incbin", "ascii", "asciz"}


linemarker_regex = re.compile(r'^[ \t]*\#\s*(\d+)\s+"([^"]*)"')


def preprocess(path: Path, version: str):
    """The source as the compiler sees it, so asm from macros is counted too."""
    define = "VERSION_US" if version == "us" else "VERSION_JP"
    root = Path(__file__).resolve().parent.parent
    return subprocess.run(
        ["cc", "-E", "-I", "include", "-nostdinc", f"-D{define}", str(path.resolve().relative_to(root))],
        cwd=root, check=True, capture_output=True, text=True,
    ).stdout


def tokenize(text):
    """Tokens as (kind, text, line); line is 0 for tokens from other files (headers)."""
    tokens = []
    line = 1
    in_main = True
    main_file = None
    for match in token_regex.finditer(text):
        kind = match.lastgroup
        if kind == "pp":
            marker = linemarker_regex.match(match.group())
            if marker:
                main_file = main_file or marker.group(2)
                in_main = marker.group(2) == main_file
                line = int(marker.group(1))
                continue
        elif kind not in ("ws", "comment"):
            tokens.append((kind, match.group(), line if in_main else 0))
        line += match.group().count("\n")
    return tokens


def unquote(literal):
    body = literal[1:-1]
    return body.encode("latin-1", "backslashreplace").decode("unicode_escape")


def template_lines(template):
    for line in re.split(r"[\n;]", template):
        line = line.split("@", 1)[0].strip()
        while True:
            match = label_regex.match(line)
            if match is None:
                break
            yield ("label", match.group(1))
            line = line[match.end():].strip()
        if not line:
            continue
        directive = directive_regex.match(line)
        if directive:
            yield ("directive", directive.group(1), directive.group(2))
        else:
            yield ("instruction", line.split()[0])


def instruction_bytes(template):
    size = 0
    for item in template_lines(template):
        if item[0] == "instruction":
            size += 4 if item[1] in ("bl", "blx") else 2
        elif item[0] == "directive" and item[1] in ("word", "4byte", "long"):
            size += 4 * (item[2].count(",") + 1)
        elif item[0] == "directive" and item[1] in ("2byte", "hword", "short"):
            size += 2 * (item[2].count(",") + 1)
    return size


def analyze(path: Path, version: str = "us"):
    """Returns (asm statements, functions) for one C file."""
    tokens = tokenize(preprocess(path, version))
    statements = []
    functions = {}  # name -> {"line", "c_statements", "asm"}
    depth = 0
    current = None
    i = 0
    while i < len(tokens):
        kind, value, line = tokens[i]
        if value == "{":
            if depth == 0:
                # Function definition: name is the identifier before the parameter list
                j = i - 1
                if tokens[j][1] == ")":
                    level = 0
                    while j >= 0:
                        if tokens[j][1] == ")":
                            level += 1
                        elif tokens[j][1] == "(":
                            level -= 1
                            if level == 0:
                                break
                        j -= 1
                    if j > 0 and tokens[j - 1][0] == "ident":
                        current = tokens[j - 1][1]
                        functions[current] = {"line": tokens[j - 1][2], "c_statements": 0, "asm": []}
            depth += 1
        elif value == "}":
            depth -= 1
            if depth == 0:
                current = None
        elif value == ";" and current is not None:
            # Count C statements: anything ending in ';' that isn't an asm statement or a declaration
            j = i - 1
            start = j
            level = 0
            while start >= 0:
                v = tokens[start][1]
                if v in (")", "]"):
                    level += 1
                elif v in ("(", "["):
                    level -= 1
                elif level == 0 and v in (";", "{", "}"):
                    break
                start -= 1
            first = tokens[start + 1]
            if first[0] == "ident" and first[1] not in ("asm", "__asm__") and first[1] not in TYPE_KEYWORDS:
                # A declaration starts with a type name followed by an identifier or '*'
                second = tokens[start + 2] if start + 2 < len(tokens) else (None, None, None)
                declaration = second[0] == "ident" or (second[1] == "*" and first[1] not in STATEMENT_KEYWORDS)
                if first[1] in STATEMENT_KEYWORDS or not declaration:
                    functions[current]["c_statements"] += 1
        elif kind == "ident" and value in STATEMENT_KEYWORDS and current is not None and value not in ("else", "do"):
            # Control flow is C even when its body is asm
            if value in ("if", "while", "for", "switch", "goto", "return"):
                functions[current]["c_statements"] += 1
        elif kind == "ident" and value in ("asm", "__asm__"):
            prev = tokens[i - 1][1] if i else ";"
            j = i + 1
            while tokens[j][1] in ("volatile", "__volatile__"):
                j += 1
            if tokens[j][1] != "(":
                i += 1
                continue
            # Collect the template (leading string literals) and skip to the closing paren
            k = j + 1
            template = ""
            while tokens[k][0] == "string":
                template += unquote(tokens[k][1])
                k += 1
            level = 1
            m = j + 1
            while level:
                if tokens[m][1] == "(":
                    level += 1
                elif tokens[m][1] == ")":
                    level -= 1
                m += 1
            has_operands = tokens[k][1] == ":"
            if (tokens[i - 1][0] == "ident" and prev not in STATEMENT_KEYWORDS) or (
                not has_operands and register_regex.match(template)
            ):
                category = "register"
            elif prev in (")", "]") and current is None and depth == 0 and not has_operands:
                category = "rename"
            elif not any(item[0] == "instruction" for item in template_lines(template)) and not any(
                item[0] == "directive" and item[1] in DATA_DIRECTIVES for item in template_lines(template)
            ):
                category = "barrier" if current is not None or not template.strip() else "directives"
            elif current is None:
                category = "toplevel"
            else:
                category = "inline"
            statement = {"line": line, "category": category, "function": current, "template": template}
            statements.append(statement)
            if current is not None and category in ("inline", "barrier"):
                functions[current]["asm"].append(statement)
            i = m
            continue
        i += 1
    return statements, functions


def toplevel_functions(statements):
    """Function labels defined by top-level asm, with their estimated sizes."""
    defined = {}
    for statement in statements:
        if statement["category"] != "toplevel":
            continue
        items = list(template_lines(statement["template"]))
        marked = {item[2].split(",")[0].strip() for item in items if item[0] == "directive" and item[1] == "type" and "function" in item[2]}
        has_markers = bool(marked) or any(item[0] == "directive" and item[1] == "thumb_func" for item in items)
        next_is_function = False
        for item in items:
            if item[0] == "directive" and item[1] == "thumb_func":
                next_is_function = True
            elif item[0] == "label" and not item[1].startswith((".", "$")):
                if item[1] in marked or next_is_function or (not has_markers and not defined):
                    defined[item[1]] = statement["line"]
                next_is_function = False
    return defined


def asm_only_functions(statements, functions):
    names = set(toplevel_functions(statements))
    for name, info in functions.items():
        has_code = any(s["category"] == "inline" for s in info["asm"])
        if has_code and info["c_statements"] == 0:
            names.add(name)
    return names


def inline_asm_functions(functions):
    return {name for name, info in functions.items() if any(s["category"] == "inline" for s in info["asm"])}


def read_function_sizes(path: Path):
    """Global/local FUNC symbol sizes from an ELF object, falling back to the gap to the next symbol."""
    data = path.read_bytes()
    shoff, = struct.unpack_from("<I", data, 0x20)
    shentsize, shnum, shstrndx = struct.unpack_from("<HHH", data, 0x2E)
    headers = [struct.unpack_from("<IIIIIIIIII", data, shoff + i * shentsize) for i in range(shnum)]
    symtab = next(h for h in headers if h[1] == 2)
    strtab = headers[symtab[6]]
    sizes = {}
    by_section = {}
    for offset in range(symtab[4], symtab[4] + symtab[5], 16):
        st_name, st_value, st_size, st_info, _, st_shndx = struct.unpack_from("<IIIBBH", data, offset)
        end = data.index(b"\0", strtab[4] + st_name)
        name = data[strtab[4] + st_name:end].decode()
        if not name or name.startswith("$") or st_shndx == 0 or st_shndx >= 0xFF00:
            continue
        by_section.setdefault(st_shndx, []).append((st_value & ~1, name, st_size))
    for shndx, symbols in by_section.items():
        symbols.sort()
        section_size = headers[shndx][5]
        for index, (value, name, size) in enumerate(symbols):
            if size == 0:
                following = [v for v, _, _ in symbols[index + 1:] if v > value]
                size = (following[0] if following else section_size) - value
            sizes[name] = size
    return sizes


def main():
    parser = ArgumentParser(description=__doc__)
    parser.add_argument("--redefine-syms", metavar="SOURCE", help="print objcopy --redefine-syms lines for SOURCE")
    parser.add_argument("--report", action="store_true", help="summarize the whole project")
    parser.add_argument("-v", "--version", default="us")
    args = parser.parse_args()

    if args.redefine_syms:
        statements, functions = analyze(Path(args.redefine_syms), args.version)
        for name in sorted(asm_only_functions(statements, functions) | inline_asm_functions(functions)):
            print(f"{name} {name}.has_asm")
        return

    if not args.report:
        parser.error("pass --redefine-syms SOURCE or --report")

    root = Path(__file__).resolve().parent.parent
    totals = {"c": 0, "inline": 0, "asm_only": 0}
    counts = {"c": 0, "inline": 0, "asm_only": 0}
    inline_bytes = 0
    categories = {}
    rows = []
    for source in sorted((root / "src").rglob("*.c")):
        rel = source.relative_to(root / "src").with_suffix(".o")
        statements, functions = analyze(source, args.version)
        for statement in statements:
            categories[statement["category"]] = categories.get(statement["category"], 0) + 1
        sizes = read_function_sizes(root / "build" / args.version / "obj" / rel)
        asm_only = asm_only_functions(statements, functions)
        file_asm_only = file_inline = 0
        for name in set(functions) | asm_only:
            size = sizes.get(name, 0)
            if name in asm_only:
                kind = "asm_only"
                file_asm_only += 1
            elif any(s["category"] == "inline" for s in functions[name]["asm"]):
                kind = "inline"
                file_inline += 1
                inline_bytes += sum(instruction_bytes(s["template"]) for s in functions[name]["asm"])
            else:
                kind = "c"
            totals[kind] += size
            counts[kind] += 1
        if file_asm_only or file_inline:
            rows.append((source.relative_to(root).as_posix(), file_asm_only, file_inline))

    total = sum(totals.values())
    print("asm statements in src/ by kind:")
    for category, count in sorted(categories.items(), key=lambda x: -x[1]):
        print(f"  {category:10} {count}")
    print()
    print(f"functions: {sum(counts.values())}, code 0x{total:x} bytes")
    labels = {"c": "pure C", "inline": "C with inline asm", "asm_only": "asm only"}
    for kind in ("c", "inline", "asm_only"):
        print(f"  {labels[kind]:18} {counts[kind]:5} functions  0x{totals[kind]:6x} bytes  {100 * totals[kind] / total:5.1f}%")
    print(f"  inline asm itself: ~0x{inline_bytes:x} bytes ({100 * inline_bytes / total:.2f}% of code)")
    print()
    print("files with asm (asm-only functions, functions with inline asm):")
    for path, asm_only, inline in sorted(rows, key=lambda r: (-r[1], -r[2])):
        print(f"  {path:55} {asm_only:4} {inline:4}")


if __name__ == "__main__":
    main()
