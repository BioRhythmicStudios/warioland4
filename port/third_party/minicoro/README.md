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

## Local patches

Each change is marked `[wl4 patch]` in `minicoro.h`; `git log -p` on the file shows them
against the vendored original.

1. **Caller-provided stacks.** `mco_desc_init_with_stack(func, stack_base, stack_size)`
   and a `stack_base` field in `mco_desc` run the coroutine on memory the caller owns.
   The port uses this for the game stack, which sits at a fixed low address with guard pages
   (see `port/include/port/gba_memory.h`). Assembly and ucontext backends only.
2. **AddressSanitizer fiber annotations.** Upstream calls `__sanitizer_start_switch_fiber`
   and `__sanitizer_finish_switch_fiber` in the wrong places. It never finishes the first
   switch into a coroutine. It finishes a resume only when the coroutine next yields, and
   it never annotates the switch back to the resuming context. Every switch in
   `_mco_jumpin`/`_mco_jumpout` is now wrapped: `start` before it, on the stack being left,
   and `finish` right after it. The coroutine's first entry finishes in `_mco_main`, and a
   coroutine that has finished releases its fake stack. The Windows-fiber and asyncify
   backends no longer have annotations (the port does not use them).
3. **Code bytes kept out of ASan.** The Windows x64 switch is a byte array placed in
   `.text`. Clang's AddressSanitizer padded it and moved it to a non-executable section,
   so the array is marked `no_sanitize("address")`.
4. **Stack overflow check.** `mco_yield` measures the stack with `__builtin_frame_address(0)`,
   not the address of a local, which ASan may put on a fake stack.
