# minicoro

Stackful coroutines in C, by Eduardo Bart: https://github.com/edubart/minicoro

The port runs the game on a coroutine. The game keeps its own call stack and yields to the
host loop when it waits for VBlank.

## Provenance

- `minicoro.h` v0.2.0, from upstream commit `02dad0f8b7cbb12fe6e216ae7a76db15ca55cd7b`
  (2024-12-07). The file last changed in `ff5321d93fe2a3fb067a8dd97a37dd623337b9c0`
  (2023-11-15).
- SHA-256 of the unmodified file: `c4205e8db0a95456dfde9f73f071609c6d2cad2ebfd1d74ed0a9254f121caa2f`
  (2033 lines).

## Licence

From the end of `minicoro.h`: your choice of **Public Domain (Unlicense)** or **MIT No
Attribution**, Copyright (c) 2021-2023 Eduardo Bart. Neither requires keeping a notice.

The assembly context switch is marked as taken from LuaCoco by Mike Pall, under the **MIT
licence**, Copyright (C) 2004-2016 Mike Pall. That licence asks for its notice to be kept
with copies. The notice stays in `minicoro.h`, and binary releases must include it as well.
