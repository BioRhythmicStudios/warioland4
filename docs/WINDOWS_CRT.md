# Optional CRT filters: Windows Beta 2 development

Implemented on `beta2-crt`, based on `831151f`. The released annotated
`v0.1.0-beta` tag remains at `909d998`; Beta 1 ZIPs are preserved. Feature
binaries and test evidence are under `build/beta2-crt-20261020/`.

## Display behaviour

Settings -> Display cycles Original (Off), CRT Lite and CRT Enhanced.
Original remains the default for new and existing installations. Save and
restart the game: the launcher settings editor and running game are separate
processes, and the game currently loads its settings once at startup.

- Original uses the existing nearest-neighbour streaming framebuffer draw.
  Its per-frame path skips the CRT function, allocations, uploads and draws.
- Lite overlays horizontal scanlines, adjustable from 0 to 50% attenuation.
  The default is 20%. Lower source-row subpixels are darkened; at 1x the
  scanlines are suppressed instead of aliasing or darkening entire GBA rows.
- Enhanced adds optional 0-30% bright-pass glow (default 10%) and an optional
  RGB phosphor mask. The mask attenuates the two non-selected channels by
  approximately 7.5%, is anchored to physical image columns, and needs 3x.
  Glow uses a five-tap spatial kernel and linear sampling; no temporal feedback.

There is no curvature, noise, animation, temporal flicker, heavy blur, or change
to the PPU's colour math. Save warnings are drawn after the filter.
Integer scaling and black bars remain controlled by SDL's existing logical
presentation. Resizing or F11 causes the cached pattern to follow the new
integer scale. The game simulation, frame scheduler, input, audio and save
algorithms are unchanged. Headless output continues to contain the original
unfiltered framebuffer.

## Implementation and compatibility

`port/platform/sdl/crt.c` uses the existing SDL_Renderer backend, with standard
multiplicative/additive texture composition. The scanline/mask texture is built
only when scale or parameters change; glow processes only 240x160 pixels and
uploads one additional 150 KiB texture each displayed frame. Explicit RGBA
textures keep the filter's pixel format independent of implicit conversions.

The current SDL_Renderer is not assumed to accept arbitrary shaders. SDL's
custom shader facility requires the GPU renderer and compatible shader formats:
https://wiki.libsdl.org/SDL3/SDL_SetGPURenderState
Keeping the existing backend avoids a GPU-backend migration, shader compiler,
shader binaries and additional runtime DLLs for these effects. The filter
module itself uses portable SDL/C APIs; it does not implement the future Linux
platform layer.

Allocation, texture-size, blend-mode and draw failures disable CRT for the
session, log the reason once, and redraw Original in the failing frame. Renderer
reset events clear CRT resources so they can be recreated. Overlay allocation
is bounded to integer scale 16 and the renderer's advertised texture limit.
No physical driver failure or actual device-loss recovery is claimed tested.

Version-3 settings add preset, scanline strength, glow strength and mask. The
bounded parser accepts v1/v2, preserves their bindings/display values, retains
v1's former VSync Off behaviour and v2's stored VSync value, and defaults CRT to
Off. Loading does not rewrite any config. Explicit Save upgrades to v3. Invalid
ranges and malformed files are rejected using existing fallback behaviour.
Beta 1 cannot read v3 files; keep a configuration copy if switching versions.
SRAM is separate and unchanged.

## Verification

- Fresh Release and ASan feature builds pass; configured Release flags are
  explicitly `-O3 -DNDEBUG`. Builds are separate from existing Windows builds.
- Normal/ASan isolated configuration, physical-key mapping and SDL virtual-pad
  tests pass. New coverage includes v1/v2 migration without file rewriting,
  all CRT presets, strength endpoints, invalid values and round-trip persistence.
- Normal/ASan coroutine, BIOS, I/O and disposable-save tests pass.
- Deterministic software and local Direct3D 11 readback tests cover 1x/2x/3x/6x
  and 853x571 output. Off equals a direct bypass pixel-for-pixel and allocates no
  CRT resources. Tests check scanline placement, white-level readability, glow
  next to bright pixels, neutral settings, unchanged input framebuffer and
  repeated static frames. An invalid-renderer fixture tests failure fallback.
- Readback uses the unchanged logical presentation and the image viewport.
  Earlier diagnostic harnesses that temporarily toggled presentation state
  produced invalid Direct3D readbacks; their failed logs remain as evidence.
  Final passing comparisons are `gpu-render-v4.log` and `gpu-shots-v4/`.
- Visible local game tests cover all three presets, VSync On/Off and startup
  windowed/fullscreen: 12 Release cases plus Lite/Enhanced ASan cases. Each
  case toggles F11 both ways; windowed cases resize to a non-integer size.
  Scales 1/2/3/6 are represented. All owned windows are explicitly shown and
  closed. Settings UI tests cover preset, strength, glow/mask, reopening,
  Display defaults and Cancel in both builds. Screenshots are in `gui-run1/`.
- These short local game runs report about 59.72-59.74 simulation Hz, zero
  redundant presents, zero clock resets, one initial audio refill and no audio
  drops. This is timing/queue evidence, not an audible-quality or scanout claim.
- The unchanged 8,500-frame Hall input route has byte-identical captured RAM,
  unfiltered pictures, audio and SRAM versus the existing Windows game binary.
  Ten late frames are captured from main-loop step 8480. Exact equality is
  limited to that captured window and final SRAM, not every uncaptured frame.
- GBA checks use fresh v7-49 and final v7-50, compared with retained build/us.
  ROM MD5 is `940b597bef3be3f4041021b47a2f4096`, with ROM-SAME and all 202 objects
  OBJ-SAME. The documented make-check base-ROM checksum failure remains.

## Performance and packaging

The local Release Direct3D 11 submission/present benchmark (120 frames per
case, VSync Off) reports approximately 0.12-0.17 ms for Original/Lite and
0.33-0.35 ms for Enhanced across tested sizes. These are wall-clock CPU
submission/present samples, not isolated GPU timings; small negative Lite
deltas are measurement noise, not a speedup. Software-renderer timings are
reported separately in the evidence and are higher, especially at 6x.
Creation/resize allocations and GPU readback are outside timed loops.

DLL import inspection finds only SDL3 plus Windows/UCRT system libraries for
both shipping executables. The vendor SDL3.dll remains unchanged. No new
runtime dependency, external shader, image, font or licence payload is needed.
Future Beta 2 packaging can use the existing allowlisted layout, refreshed
executables, SDL3.dll and notices; omit the developer test executable and all
test ROM/save/config/capture files. Release preparation and verification for v0.2.0-beta are recorded in
[WINDOWS_BETA2_RELEASE.md](WINDOWS_BETA2_RELEASE.md).

On 2026-10-20 the user confirmed personal testing on the Windows development PC
and the separate Intel integrated-graphics laptop with a 60 Hz OLED display:
CRT Lite looks good, and CRT Enhanced including phosphor effects and glow looks
excellent. The user confirmed correct Intel-laptop filter operation and approved
publishing v0.2.0-beta. This is manual hardware evidence; no unreported audio,
driver, VRR or other-hardware details are inferred. No Linux build is claimed.

## Reproduce locally

Build the configured feature normal/ASan trees, then run `wl4-input-test
--isolated` with a new explicit `WL4_SETTINGS_FILE`, and the existing runtime
tests with a disposable `--test-save` directory. Render tests need an existing
output directory:

```
wl4-crt-test OUTPUT_DIRECTORY [FRAME.xbgr|-] [software|direct3d11]
```

The optional frame input is exactly 240x160 little-endian XBGR pixels; omission
uses the built-in geometric test pattern. The test executable opens no ROM or
personal settings/save. It writes comparison PNGs and reports per-mode timing.
The local `build/beta2-crt-20261020/gui.ps1` records the disposable GUI procedure.
