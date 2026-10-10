# Wario Land 4

This is a work-in-progress decompilation of *Wario Land 4*.

This repository can build the following versions:
- US/EU `md5: 5fe47355a33e3fabec2a1607af88a404` (default)
- JP `md5: 99c8ad779a16be513a9fdff502b6f5c2`

<!-- DECOMP_PROGRESS:START -->
## Decompilation Progress

## It's 100% decompiled. All function have C equivalent so project can be built and recomp to be made. But note that some remaining ASM code sites are presents but i can't seem to figure out how to convert them to pure C as agbcc seems not cooperative enough especially for register bindings pressure. 

![Decompilation progress](docs/progress-treemap.svg)

[Open the interactive progress treemap](https://TsilaAllaoui.github.io/warioland4/progress.html).
<!-- DECOMP_PROGRESS:END -->

## Dependencies

- `binutils-arm-none-eabi`
- `clang-format`
- Python 3

## Building

The C compiler (agbcc) is provided as a submodule in this repository. Make sure you set it and the other tools up before
building:

```sh
git submodule update --init
make tools
```

This will also set up a Python virtual environment and dependencies for the asset extractor.

To build the US version: place the ROM in the repository root as `baserom_us.gba`, then run `make extract` to extract
the assets. Then you can run `make` to build the ROM.

To build the JP version: add `VERSION=jp` when running `make extract` and `make`, e.g.
`make extract VERSION=jp && make VERSION=jp`.

If there were no problems, a message such as this will be printed to the screen:

```
build/us/warioland4_us.gba: OK
```

For more details, run `make help`.
