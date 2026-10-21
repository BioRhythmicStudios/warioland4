// Native event bridge fixtures: no ROM, SDL device, personal config or SRAM.
#include "main.h"
#include "wario.h"
#include "global_data.h"
#include "sprite.h"
#include "port/rumble_snapshot.h"
#include <stdio.h>
#include <string.h>
s16 gMainGameMode, gSubGameMode;
s8 gPauseFlag;
u8 gCurrentPassage, gCurrentRoom;
struct WarioData gWarioData;
struct WarioLife gHeartMeter;
struct ScreenShakeParameters gScreenShakeX, gScreenShakeY;
struct PrimarySpriteData gCurrentSprite, gSpriteData[MAX_SPRITE_COUNT];
u8 gSpriteCollisionFlags;
static int failures;
#define CHECK(c) do { if (!(c)) {printf("FAIL line %d\n", __LINE__); ++failures;} } while (0)
static unsigned Events(void) { PortRumbleSnapshot s; PortRumbleRead(&s); return s.events; }
int main(void)
{
    gMainGameMode = GM_GAME_SCREEN; gSubGameMode = 2;
    gWarioData.pose = WPOSE_NORMAL_SHOULDER_BASH;
    gWarioData.horizontalDirection = 0x10;
    gSpriteCollisionFlags = SPRITE_COLLISION_LEFT;
    PortRumbleContactBegin(0); gSpriteData[0].pose = SPOSE_TACKLED_RIGHT_INIT; PortRumbleContactEnd(0);
    CHECK(Events() == PORT_RUMBLE_CONTACT); CHECK(Events() == 0);
    PortRumbleContactBegin(0); PortRumbleContactEnd(0); CHECK(Events() == 0);
    gWarioData.horizontalDirection = 0x20;
    PortRumbleContactBegin(0); gSpriteData[0].pose = SPOSE_TACKLED_LEFT_INIT; PortRumbleContactEnd(0);
    CHECK(Events() == 0);
    gWarioData.pose = WPOSE_NORMAL_GROUND_POUND; gSpriteCollisionFlags = SPRITE_COLLISION_ABOVE;
    PortRumbleContactBegin(0); gSpriteData[0].pose = SPOSE_CRUSHED_OR_COLLECTED_INIT; PortRumbleContactEnd(0);
    CHECK(Events() == PORT_RUMBLE_CONTACT);
    gWarioData.pose = WPOSE_NORMAL_STANDING; gSpriteData[0].pose = 16;
    PortRumbleContactBegin(0); gSpriteData[0].pose = SPOSE_TACKLED_RIGHT_INIT; PortRumbleContactEnd(0);
    CHECK(Events() == 0);
    gWarioData.yVelocity = 20; PortRumbleTileBroken(); CHECK(Events() == PORT_RUMBLE_HEAD);
    gWarioData.yVelocity = -20; PortRumbleTileBroken(); CHECK(Events() == 0);
    gSpriteData[0].globalID = PSPRITE_ROCK; gSpriteCollisionFlags = SPRITE_COLLISION_BELOW;
    gWarioData.yVelocity = 20;
    PortRumbleContactBegin(0); gWarioData.yVelocity = 0; PortRumbleContactEnd(0);
    CHECK(Events() == PORT_RUMBLE_HEAD);
    PortRumbleImpact(PORT_RUMBLE_SWITCH); PortRumbleImpact(PORT_RUMBLE_SWITCH);
    CHECK(Events() == PORT_RUMBLE_SWITCH); CHECK(Events() == 0);
    PortRumbleImpact(PORT_RUMBLE_FROG); CHECK(Events() == PORT_RUMBLE_FROG); CHECK(Events() == 0);
    // Real reaction/pose IDs, sampled through the same bridge as the SDL frontend.
    { int form;
      for (form = REACTION_FLAMING; form < REACTION_COUNT; ++form) {
        gSubGameMode = 0; Events(); gSubGameMode = 2;
        gWarioData.reaction = REACTION_NORMAL; gWarioData.pose = 2; Events();
        gWarioData.reaction = form; gWarioData.pose = 0;
        CHECK(Events() == (form == REACTION_FLAMING ? PORT_RUMBLE_FIRE : PORT_RUMBLE_TRANSFORM));
        CHECK(Events() == 0);
        gWarioData.reaction = REACTION_WATER; gWarioData.pose = WPOSE_WATER_SWIMMING_HORIZONTAL;
        CHECK(Events() == PORT_RUMBLE_RECOVER); CHECK(Events() == 0);
        gWarioData.reaction = REACTION_NORMAL; CHECK(Events() == 0);
      }
    }
    gWarioData.reaction = REACTION_PUFFY; gWarioData.pose = WPOSE_PUFFY_TRANSFORMING;
    CHECK(Events() == PORT_RUMBLE_TRANSFORM);
    gWarioData.pose = WPOSE_PUFFY_FLOATING; CHECK(Events() == 0);
    gWarioData.pose = WPOSE_PUFFY_HITTING_CEILING; CHECK(Events() == PORT_RUMBLE_RECOVER);
    CHECK(Events() == 0); gWarioData.reaction = REACTION_NORMAL; CHECK(Events() == 0);
    gWarioData.reaction = REACTION_FLAMING; gWarioData.pose = WPOSE_FLAMING_TRANSFORMING;
    CHECK(Events() == PORT_RUMBLE_FIRE);
    gWarioData.pose = WPOSE_FLAMING_BLUE; CHECK(Events() == 0);
    gWarioData.pose = WPOSE_FLAMING_BLUE_MIDAIR; CHECK(Events() == 0);
    gWarioData.pose = WPOSE_FLAMING_GREEN; CHECK(Events() == PORT_RUMBLE_FIRE);
    gWarioData.pose = WPOSE_FLAMING_GREEN_MIDAIR; CHECK(Events() == 0);
    gWarioData.pose = WPOSE_FLAMING_RED; CHECK(Events() == PORT_RUMBLE_FIRE);
    gWarioData.pose = WPOSE_FLAMING_ENGULFED; CHECK(Events() == PORT_RUMBLE_FIRE);
    gWarioData.pose = WPOSE_FLAMING_RECOVERING; CHECK(Events() == 0);
    gWarioData.reaction = REACTION_NORMAL; CHECK(Events() == PORT_RUMBLE_ASH); CHECK(Events() == 0);
    gWarioData.reaction = REACTION_SNOWMAN; gWarioData.pose = WPOSE_SNOWMAN_TRANSFORMING;
    CHECK(Events() == PORT_RUMBLE_TRANSFORM);
    gWarioData.pose = WPOSE_SNOWMAN_ROLLING_SMALL; CHECK(Events() == PORT_RUMBLE_SNOW);
    gWarioData.pose = WPOSE_SNOWMAN_ROLLING_SMALL_MIDAIR; CHECK(Events() == 0);
    gWarioData.pose = WPOSE_SNOWMAN_ROLLING_MEDIUM; CHECK(Events() == 0);
    gWarioData.pose = WPOSE_SNOWMAN_ROLLING_LARGE; CHECK(Events() == 0);
    gWarioData.pose = WPOSE_SNOWMAN_BONK;
    CHECK(Events() == (PORT_RUMBLE_SNOW_END | PORT_RUMBLE_FORM_IMPACT));
    gWarioData.reaction = REACTION_NORMAL; CHECK(Events() == PORT_RUMBLE_RECOVER);
    gWarioData.reaction = REACTION_WATER; gWarioData.pose = WPOSE_WATER_BUBBLE_WARIO;
    CHECK(Events() == PORT_RUMBLE_TRANSFORM);
    gWarioData.pose = WPOSE_WATER_SWIMMING_HORIZONTAL; CHECK(Events() == PORT_RUMBLE_RECOVER);
    { int form;
      for (form = REACTION_ZOMBIE; form <= REACTION_BAT; form += REACTION_BAT - REACTION_ZOMBIE) {
        gWarioData.reaction = REACTION_NORMAL; gWarioData.pose = 2; Events();
        gWarioData.reaction = form; gWarioData.pose = 0; CHECK(Events() == PORT_RUMBLE_TRANSFORM);
        gWarioData.pose = form == REACTION_BAT ? WPOSE_BAT_TOUCHING_LIGHT_SOURCE : WPOSE_ZOMBIE_TOUCHING_LIGHT_SOURCE;
        CHECK(Events() == 0); CHECK(Events() == 0);
        gWarioData.reaction = REACTION_NORMAL; CHECK(Events() == PORT_RUMBLE_RECOVER); CHECK(Events() == 0);
      }
    }
    gWarioData.reaction = REACTION_FROZEN; gWarioData.pose = WPOSE_FROZEN_SLIDING; Events();
    gWarioData.pose = WPOSE_FROZEN_BONK; CHECK(Events() == PORT_RUMBLE_FORM_IMPACT); CHECK(Events() == 0);
    gWarioData.reaction = REACTION_NORMAL; Events();
    gWarioData.reaction = REACTION_BOUNCY; gWarioData.pose = WPOSE_BOUNCY_JUMPING; Events();
    gWarioData.pose = WPOSE_BOUNCY_HITTING_CEILING; CHECK(Events() == PORT_RUMBLE_FORM_IMPACT); CHECK(Events() == 0);
    gSubGameMode = 0; Events(); gWarioData.reaction = REACTION_BAT; gSubGameMode = 2;
    CHECK(Events() == 0); // Resuming/loading a transformed state must not fake activation.
    gCurrentRoom++; gWarioData.reaction = REACTION_NORMAL; CHECK(Events() == 0);
    printf("Native rumble event checks: %s\n", failures ? "FAILED" : "PASS");
    return failures != 0;
}
