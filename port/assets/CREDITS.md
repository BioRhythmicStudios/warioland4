# Current text-only launcher

The current launcher ships no Vampire Wario sprite or YoshiKong game icon.
`neutral-placeholder.ico` is original project geometry generated entirely by
`port/tools/make_neutral_icon.py`, under the project MIT terms. No external
artwork inputs are used. The private pixel font remains: see fonts/README.md
and fonts/SDL-LICENSE.txt for the public-domain glyph provenance and SDL notice.

The older files below remain in the repository as historical assets; they are
not linked into the current launcher or included in the text-only beta package.

# Launcher artwork

`vampirewariosprite.png`: sprite by **MisteryGX**, supplied by the user for this
launcher. The supplied transparent PNG is preserved byte-for-byte. It is displayed
at an integer scale with nearest-neighbour pixel replication.

Source verified for release review:
[Vampire Wario in Wario Land 4 Style](https://www.mfgg.net/?act=resdb&param=02&c=1&id=36502)
by MisteryGX. The [sheet](https://mfgg.net/index.php?act=resdb&param=03&c=1&id=36502)
also identifies its creator as RamenNoodleSprites and states:
"If used, give credit to me and Nintendo." Keep both credits in distributions.
[MFGG's rules](https://www.mfgg.net/index.php?act=page&name=rules) describe
nonprofit resource use; this is not an MIT licence or a Nintendo rights grant.
The user clarified that their permission report referred to this downloadable
sheet and its stated reuse condition, not a separately supplied written grant.

Wario and Wario Land 4 belong to Nintendo.

`launchericon.png`: supplied by the user as the launcher icon. The original PNG is
preserved unchanged. `launchericon.ico` contains nearest-neighbour variants with
transparent padding to preserve the original aspect ratio.
Source: [Level Icons, Wario Land 4](https://www.spriters-resource.com/game_boy_advance/wl4/asset/52837/),
uploaded by **YoshiKong**. The user reports the sheet's instruction to credit
YoshiKong; the launcher and package preserve that attribution. The collection
is game artwork, not evidence that its uploader owns Nintendo's underlying
graphics. The site's [terms](https://www.spriters-resource.com/page/tou/) distinguish
custom work from other material and do not establish a Nintendo redistribution
grant. That underlying rights/no-extracted-game-assets question remains open
for public release review; giving uploader credit alone does not resolve it.

The launcher pixel font is derived from SDL's debug text font, whose 8x8 glyphs
are attributed to Marcel Sondaar / IBM public-domain VGA fonts. The private
TrueType conversion is generated for this launcher; it is not an installed system
font. See [font provenance and regeneration](fonts/README.md).
