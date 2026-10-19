# Launcher asset redistribution audit (2026-10-19)

Status: public upload is on hold. This record distinguishes an author's stated
reuse condition from our adding a credit, and from rights to underlying game art.
Neither an accessible download nor an attribution is treated as a blanket MIT
licence. No public release, tag or branch push is authorized past an unresolved
release-critical rights check.

## Vampire Wario sprite: MisteryGX

- **File:** `port/assets/vampirewariosprite.png`. A supplied transparent frame
  embedded in launcher resource 101, displayed at integer nearest-neighbour scale.
- **Credited creator:** MisteryGX, the MFGG page author. The sheet also uses the
  name RamenNoodleSprites. Nintendo is credited for Wario/game material.
- **Original page:** https://www.mfgg.net/?act=resdb&param=02&c=1&id=36502
- **Original sheet:** https://mfgg.net/index.php?act=resdb&param=03&c=1&id=36502
- **Verified statement:** the downloaded sheet was inspected locally and says,
  "If used, give credit to me and Nintendo." The page classifies the sheet as
  edited sprites, not wholly original work.
- **Site terms:** https://www.mfgg.net/index.php?act=page&name=rules describes
  nonprofit resource use. The current rules also state a date-limited scope;
  do not assume their newer provisions retroactively settle every older upload.
- **What is established:** the creator's attributed-use condition is documented.
  This is stronger evidence than merely deciding to credit them. We preserve
  MisteryGX/RamenNoodleSprites and Nintendo in the distribution notices; the UI
  already credits MisteryGX and Nintendo.
- **Still missing:** an established grant for any Nintendo-owned base pixels in
  this edited sheet, and a basis for treating the asset as cleared under the
  requested no-extracted-game-assets distribution rule. The sheet does not put
  the sprite under MIT, nor establish that its submitter can license Nintendo's
  underlying material. A broader public source/binary redistribution clearance
  must not be inferred beyond the stated attributed-use condition. The sheet
  supports intended reuse with credit, but does not expressly specify public
  downloadable-build or source-file redistribution. Full public-build clearance
  is therefore not established under the requested audit standard.

The user's initial reference to a friend's permission was clarified to mean the
download page and sheet condition above; no separate private written grant was
provided. We do not describe the author's credit condition as absent.

## Launcher icon: YoshiKong

- **Files:** `port/assets/launchericon.png` (user-supplied icon image) and
  `port/assets/launchericon.ico` (generated transparent, nearest-neighbour sizes).
  The ICO is embedded as the EXE/window/desktop icon; it is not the Vampire Wario
  sprite and must not be credited to MisteryGX.
- **Credited contributor:** YoshiKong, listed by the supplied source as uploader.
  The user explicitly requested this credit; it is now in the launcher UI and
  packaging notices. An uploader is not automatically the underlying artist.
- **Original source:** https://www.spriters-resource.com/game_boy_advance/wl4/asset/52837/
  The indexed source identifies this as **Level Icons**, Wario Land 4, Game Boy
  Advance, uploaded by YoshiKong.
- **Reported statement:** the user reports that its sheet says to credit
  Yoshikong if used. The full page/download was blocked by a site challenge in
  this environment; unlike the Vampire Wario sheet, that footer has not been
  independently inspected here. Source identity is corroborated by the site's
  indexed asset information.
- **Site terms:** https://www.spriters-resource.com/page/tou/ allows attributed
  custom work in noncommercial projects subject to submission-specific limits,
  and distinguishes it from other content. It does not provide a Nintendo
  graphics licence merely because a game sheet is downloadable.
- **Still missing:** confirmation of the exact sheet condition and the original
  image's status: original custom art versus a crop/rip of Nintendo level-icon
  graphics. The catalogue identifies game level icons, not a custom-art entry.
  If these are extracted Nintendo pixels, uploader credit alone does not clear
  public redistribution and conflicts with the requested asset exclusion.
  That requires a documented applicable grant or an original permitted replacement.

Calling this a "custom launcher icon" describes its use in our application;
it is not evidence that the original drawing was custom-created by YoshiKong.
The available catalogue and the supplied image's level-portrait appearance
suggest extracted game graphics, not newly drawn original fan artwork. This is
an inference: a direct public-image request also returned HTTP 403, so no
independent sheet-footer reading or pixel-for-pixel source match was completed.
It is not appropriate to claim YoshiKong owns the original Nintendo drawing.

## Pixel font: no additional asset permission gap identified

- **Files:** `port/assets/fonts/wl4-pixel.ttf`, generated by the project from
  SDL's debug glyphs; `port/assets/fonts/SDL-LICENSE.txt` retains the SDL notice.
- **Credits:** Marcel Sondaar / IBM public-domain VGA glyphs; SDL contributors;
  project conversion/packaging. This is a private embedded font, not a copied
  installed Windows font. FontTools is build-only and is not shipped.
- **Primary source:** https://github.com/libsdl-org/SDL/blob/release-3.4.18/src/render/SDL_render_debug_font.h
  The exact release header was downloaded and inspected. Its glyph provenance
  explicitly identifies public-domain VGA fonts, alongside SDL's zlib notice.
- **Conditions:** retain SDL's notice and identify the TTF conversion as altered
  packaging, not an original SDL TTF. Those notices are included in the candidate.
- **Missing permission:** none additionally identified for this documented font
  provenance. This conclusion does not clear the two artwork groups above.

The purple/gold UI, rounded controls and window layout are code, not additional
external artwork files. No other loose art is required at runtime. The sprite,
icon and font remain inside the launcher even if no PNG/ICO/TTF appears in the ZIP;
removing loose files would not remove the embedded assets or their rights issues.

## Separate source-licensing blocker

`docs/PC_PORT_PLAN.md` already records a grant gap for later TsilaAllaoui upstream
contributions. The restored lilDavid MIT notice is preserved, but does not by
itself document permission for those other contributions. Local commit `a8f2557`
contains TsilaAllaoui source/header contributions used by the port.
Examples present in that imported snapshot are `src/cutscenes.c`,
`src/wario/normal.c` and `include/cutscenes.h`; CMake compiles the C sources and
uses the headers. The snapshot author does not establish authorship of every
line, and this is not a claim that all of `src/` and `include/` are unlicensed.

A live GitHub API check during this audit reports upstream's licence as null and
its current root listing contains README but no LICENSE. A cached raw-file web
result returned an older MIT notice; that inconsistent cached result was not
used to override the live metadata and existing unresolved record.
The recursive upstream tree exposes `tools/gbafix/COPYING`, but that build-tool
licence does not license the game's source or headers. No alternative grant was
found in the inspected local source/header notices. Vendored runtime notices
(SDL, minicoro/LuaCoco, LLVM/MinGW) are separate and are retained in the package.

https://api.github.com/repos/TsilaAllaoui/warioland4

This code question is independent of the sprite/icon credits. It needs a
documented grant covering the relevant contributions or another reviewed basis
before the requested release-clearance check can pass. This record does not
assert that public availability proves permission or that infringement is proven.
Deleting a licence file does not itself revoke earlier grants. A revision-level
provenance check could establish that particular contributions were already
covered by MIT; absence of a current root LICENSE is not proof to the contrary.
The missing result is a documented basis covering the imported revision as a
whole, not another copy of lilDavid's notice. Obtain that evidence or a relevant
contributor grant before representing the release-wide code licence as cleared.
