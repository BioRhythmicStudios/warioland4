# Windows v0.2.0-beta: The CRT Update

The user authorized preparation and publication on 2026-10-20 after manually
testing CRT Lite and Enhanced, including phosphor mask/glow, on the Windows
development PC and a separate Intel integrated-graphics laptop with a 60 Hz
OLED display. Both looked good and operated correctly. Do not infer additional
audio, driver, refresh-mode or device coverage from that report.

## Release scope

Presentation-only CRT effects and versioned Display settings are documented in
[WINDOWS_CRT.md](WINDOWS_CRT.md). There are no simulation, camera, physics,
collision, audio, PPU or shared GBA source changes. New settings default to
Original (Off); older v1/v2 configurations load without rewriting. Save and
restart the game after changing filters. Beta 1 cannot read v3 configurations.

Base history is the cleaned `831151f`, following `909d998`. Preserve the
annotated `v0.1.0-beta` tag and published Beta 1 ZIP. The intended source branch
is `port` on `myfork`, https://github.com/BioRhythmicStudios/warioland4. No push
to origin, force-push, history rewrite, old-history publication or tag overwrite
is allowed. Fetch and check divergence and tag/release absence before publishing.

## Verification and packaging procedure

1. Review and stage only CRT implementation/tests/docs by explicit path. Keep
   unrelated provenance research, local cleanup notes, ROMs, saves and symlinks
   outside the commit. Verify the reachable release history contains none of
   the three purged artwork paths.
2. Run normal/ASan settings, runtime and CRT render tests; repeat visible
   VSync/fullscreen/F11/resize checks with explicit disposable save/config paths.
   Fresh GBA v7-51 must match build/us ROM and all 202 objects, with MD5
   `940b597bef3be3f4041021b47a2f4096`. The documented make-check checksum failure
   is separate from these byte comparisons.
3. Commit the verified source. Build Windows x64 Release from that exact commit
   in a clean sparse checkout excluding shared agbcc/data paths, with explicit
   `-O3 -DNDEBUG`, ASan Off and vendor SDL3 3.4.18. Record the full source commit,
   source-tree hash, build flags and payload hashes in BUILD_INFO/SHA256SUMS.
4. Package only wl4-launcher.exe, wl4.exe, SDL3.dll, README/installation,
   known issues, credits/hardware summary, BUILD_INFO, hashes and applicable
   notices. Retain the published Beta 1 notice payloads and project MIT licence.
   Do not include developer test executables, external shader tools, ROMs,
   extracted assets, personal data, config/save fixtures or removed artwork.
5. Audit ZIP CRC, exact allowlist, imports/dependency closure, source resources,
   manifests and binary equality with the clean build. Extract outside the
   repository and test using Windows-only PATH. Test launcher startup/ROM
   selection/game launch, Settings persistence, CRT presets, VSync and F11.
   Preserve personal settings/save state and close only owned test processes.
6. Capture the optional release gameplay screenshot from the final packaged
   Beta 2 executable using a disposable save, CRT Enhanced and a documented
   scripted route. Upload only this new image, ZIP and checksum as release assets.
7. Recheck remote ancestry and tag/release absence. Integrate the feature into
   port by fast-forward when possible. Push one explicitly named branch at a
   time only to myfork; create/push the annotated v0.2.0-beta tag at the verified
   source commit. Publish a GitHub prerelease with the supplied notes and ZIP hash.

Builds, test logs, package inventory, source correspondence, final ZIP hash and
remote release response are retained in `build/release-v0.2.0-beta-20261020/`.
No external dependency or imported artwork is added by the CRT update. This
experimental beta still lacks complete game-compatibility verification; path
encoding and multi-instance save limitations from Beta 1 remain.
