// Gameplay observation bridge, compiled with the same layouts as the game.
#include "main.h"
#include "wario.h"
#include "global_data.h"
#include "sprite.h"
#include "port/rumble_snapshot.h"
static int sRock, sBoss, sBossValid, sBossHealth, sBossKey;
static unsigned sEvents;
static int sContactAttack, sContactPose, sContactHead;
void PortRumbleImpact(unsigned events) { sEvents |= events; }
void PortRumbleContactBegin(int slot)
{
    int pose = gWarioData.pose;
    int flags = gSpriteCollisionFlags;
    int side = ((flags & SPRITE_COLLISION_LEFT) && (gWarioData.horizontalDirection & 0x10)) ||
        ((flags & SPRITE_COLLISION_RIGHT) && (gWarioData.horizontalDirection & 0x20));
    sContactAttack = gWarioData.reaction == REACTION_NORMAL &&
        (((pose == WPOSE_NORMAL_GROUND_POUND || pose == WPOSE_NORMAL_SUPER_GROUND_POUND) &&
            (flags & SPRITE_COLLISION_ABOVE)) ||
         (pose >= WPOSE_NORMAL_SHOULDER_BASH && pose <= WPOSE_NORMAL_DASH_ATTACK_JUMP && side));
    sContactHead = gSpriteData[slot].globalID == PSPRITE_ROCK &&
        (flags & SPRITE_COLLISION_BELOW) && gWarioData.yVelocity > 0;
    sContactPose = gSpriteData[slot].pose;
}
void PortRumbleContactEnd(int slot)
{
    int pose = gSpriteData[slot].pose;
    // Require the collision handler to accept an attack, not merely overlapping hitboxes.
    if (sContactAttack && pose != sContactPose &&
        (pose == SPOSE_TACKLED_RIGHT_INIT || pose == SPOSE_TACKLED_LEFT_INIT ||
         pose == SPOSE_4F || pose == SPOSE_50 || pose == SPOSE_CRUSHED_OR_COLLECTED_INIT))
        sEvents |= PORT_RUMBLE_CONTACT;
    if (sContactHead && (gWarioData.yVelocity <= 0 || pose != sContactPose))
        sEvents |= PORT_RUMBLE_HEAD;
    sContactAttack = sContactHead = 0;
}
void PortRumbleBossHealth(void)
{
    int key = (gCurrentPassage << 16) | (gCurrentRoom << 8) | gCurrentSprite.globalID;
    if (gMainGameMode != GM_GAME_SCREEN) { sBossValid = 0; return; }
    if (sBossValid && key == sBossKey && gCurrentSprite.health < sBossHealth) sBoss = 1;
    sBossHealth = gCurrentSprite.health; sBossKey = key; sBossValid = 1;
}
void PortRumbleTileBroken(void)
{
    if (gWarioData.reaction == REACTION_NORMAL &&
        gWarioData.pose >= WPOSE_NORMAL_SHOULDER_BASH && gWarioData.pose <= WPOSE_NORMAL_DASH_ATTACK_JUMP)
        sRock = 1;
    else if (gWarioData.reaction == REACTION_NORMAL && gWarioData.yVelocity > 0)
        sEvents |= PORT_RUMBLE_HEAD;
}
static int sFormValid, sForm, sPose, sFormRoom, sRecoveryAnnounced;
static int Form(int reaction, int pose)
{
    if (reaction >= REACTION_FLAMING && reaction < REACTION_COUNT) return reaction;
    // Bubble Wario is a water pose, not a separate reaction.
    return reaction == REACTION_WATER && pose == WPOSE_WATER_BUBBLE_WARIO ? REACTION_COUNT : 0;
}
static int Rolling(int form, int pose)
{
    return form == REACTION_SNOWMAN && pose >= WPOSE_SNOWMAN_ROLLING_SMALL &&
        pose <= WPOSE_SNOWMAN_ROLLING_LARGE_MIDAIR;
}
static int FirePhase(int pose)
{
    if (pose >= WPOSE_FLAMING_BLUE && pose <= WPOSE_FLAMING_RED_MIDAIR) return (pose - 1) / 2;
    return pose == WPOSE_FLAMING_ENGULFED || pose == WPOSE_FLAMING_ENGULFED_MIDAIR ? 3 : -1;
}
static void SampleTransformation(int active)
{
    int form = Form(gWarioData.reaction, gWarioData.pose);
    int pose = gWarioData.pose;
    int room = (gCurrentPassage << 8) | gCurrentRoom;
    if (!active) { sFormValid = 0; return; }
    if (sFormValid && room == sFormRoom) {
        if (form != sForm) {
            if (Rolling(sForm, sPose)) sEvents |= PORT_RUMBLE_SNOW_END;
            if (form) sEvents |= form == REACTION_FLAMING ? PORT_RUMBLE_FIRE : PORT_RUMBLE_TRANSFORM;
            else if (sForm && !sRecoveryAnnounced)
                sEvents |= sForm == REACTION_FLAMING && sPose == WPOSE_FLAMING_RECOVERING
                    ? PORT_RUMBLE_ASH : PORT_RUMBLE_RECOVER;
            sRecoveryAnnounced = 0;
        } else {
            if (form == REACTION_PUFFY && pose == WPOSE_PUFFY_HITTING_CEILING && pose != sPose) {
                sEvents |= PORT_RUMBLE_RECOVER; sRecoveryAnnounced = 1;
            }
            if (Rolling(form, pose) && !Rolling(sForm, sPose)) sEvents |= PORT_RUMBLE_SNOW;
            if (!Rolling(form, pose) && Rolling(sForm, sPose)) sEvents |= PORT_RUMBLE_SNOW_END;
            if (form == REACTION_FLAMING && FirePhase(sPose) >= 0 && FirePhase(pose) > FirePhase(sPose) && FirePhase(pose) >= 0)
                sEvents |= PORT_RUMBLE_FIRE;
            if (pose != sPose &&
                ((form == REACTION_FROZEN && pose == WPOSE_FROZEN_BONK) ||
                 (form == REACTION_BOUNCY && pose == WPOSE_BOUNCY_HITTING_CEILING) ||
                 (form == REACTION_SNOWMAN && pose == WPOSE_SNOWMAN_BONK) ||
                 (form == REACTION_FAT && pose == WPOSE_FAT_LANDING)))
                sEvents |= PORT_RUMBLE_FORM_IMPACT;
        }
    } else sRecoveryAnnounced = 0;
    sFormValid = 1; sFormRoom = room; sForm = form; sPose = pose;
}
void PortRumbleRead(PortRumbleSnapshot *s)
{
    s->active = gMainGameMode == GM_GAME_SCREEN && gSubGameMode == 2 && !gPauseFlag;
    if (gMainGameMode != GM_GAME_SCREEN) sBossValid = 0;
    SampleTransformation(s->active);
    s->events = sEvents; sEvents = 0;
    s->boss = sBoss; sBoss = 0;
    s->rock = sRock; sRock = 0;
    s->health = gHeartMeter.current;
    s->shakeX = gScreenShakeX.duration;
    s->shakeY = gScreenShakeY.duration;
}
