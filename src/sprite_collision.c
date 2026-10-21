#include "sprite_collision.h"

#include "block.h"
#include "input.h"
#include "score.h"
#include "sound.h"
#include "sprite.h"
#include "sprite_ai/cuckoo_condor.h"
#include "sprite_ai/golden_diva.h"
#include "voice_set.h"
#include "wario.h"
#include "wario/normal.h"

#ifdef PORTABLE
// agbcc calls func_8023BFC without a prototype here; with one, it narrows the arguments.
#include "sprite_util.h"
#include "port/rumble_snapshot.h"
#endif

extern const struct AnimationFrame sCuckooCondorClawOpenOam[];

#define SpriteCollisionTransformWario(react)                                   \
{                                                                              \
    if (!CheckWarioReactionCollision(react)) {                                          \
        gWarioData.reaction = react;                                           \
        WarioRequestPose(0);                                                   \
    }                                                                          \
}

void func_801E4B0(void)
{
    if (gCurrentCarriedSprite.state != 7) {
        return;
    }

    MPlayStop(GP(gMPlayTable[3].info));
    m4aSongNumStart(SOUND_28);
}

void func_801E4D4(void)
{
    m4aSongNumStart(SOUND_1CD);
    MPlayStop(GP(gMPlayTable[3].info));
    m4aMPlayFadeOut(GP(gMPlayTable[0].info), 2);
    m4aMPlayFadeOut(GP(gMPlayTable[1].info), 2);
    m4aMPlayFadeOut(GP(gMPlayTable[2].info), 2);
    m4aSongNumStop(SE_ESCAPE_SWITCH_COUNTDOWN);
    m4aSongNumStop(SOUND_1C2);
}

void func_801E518(void)
{
    m4aMPlayFadeOut(GP(gMPlayTable[1].info), 4);
}

void SpriteCollisionMakeWarioDropCoins(s32 slot)
{
    s32 amount;

    switch (gSpriteData[slot].globalID) {
        case PSPRITE_SPEAR_MASK_BLUE:
        case PSPRITE_SPIKY:
        case PSPRITE_MAYUBIRD:
        case PSPRITE_TOBAWANI:
            amount = CONVERT_SCORE(-200);
            break;

        case PSPRITE_SPEAR_MASK_RED:
        case PSPRITE_TOGEROBO:
        case PSPRITE_TOGETOGE:
        case PSPRITE_9C:
            amount = CONVERT_SCORE(-400);
            break;

        case PSPRITE_MOGURAMEN:
        case PSPRITE_SPIKY_Z:
        case PSPRITE_9A:
        case PSPRITE_BALL_LIGHTNING_BOLT:
            amount = CONVERT_SCORE(-500);
            break;

        case PSPRITE_TOGEMASUKU:
        case PSPRITE_GOGGLEY_BLADE:
        case PSPRITE_GOGGLEY_BLADE_2:
        case PSPRITE_MAGMA:
            amount = CONVERT_SCORE(-300);
            break;

        case PSPRITE_SPEAR_MASK_YELLOW:
        case PSPRITE_TOTSUMEN:
        case PSPRITE_MENONO:
        case PSPRITE_SHIERAGUTCHI:
        case PSPRITE_DENDEN:
        case PSPRITE_BUTATABI:
        case PSPRITE_ROBOBIRD:
        case PSPRITE_UTSUBOANKO:
        case PSPRITE_TOGENOBI:
        case PSPRITE_ICICLE:
        case PSPRITE_TOY_CAR:
        case PSPRITE_ONOMI:
        case PSPRITE_PETBOTTOM:
        case PSPRITE_B8:
        case PSPRITE_BOWLER_BALL:
        case PSPRITE_DEBURINA_CHILD:
            amount = CONVERT_SCORE(-100);
            break;

        default:
            amount = 0;
            break;
    }
    ChangeStageScoreAndDropCoins(amount);
}

void func_801E884(s32 slot)
{
    if (gSpriteCollisionFlags & SPRITE_COLLISION_VERTICAL) {
        if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
            gSpriteData[slot].pose = SPOSE_33;
        } else {
            gSpriteData[slot].pose = SPOSE_35;
        }
        m4aSongNumStart(SOUND_35);
    } else if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
        if (gSpriteData[slot].warioCollision == 5) {
            gSpriteData[slot].pose = SPOSE_33;
            m4aSongNumStart(SOUND_35);
        } else {
            gSpriteData[slot].pose = SPOSE_PUSHED_RIGHT_INIT;
            m4aSongNumStart(SOUND_34);
        }
    } else if (gSpriteCollisionFlags & SPRITE_COLLISION_RIGHT) {
        if (gSpriteData[slot].warioCollision == 5) {
            gSpriteData[slot].pose = SPOSE_35;
            m4aSongNumStart(SOUND_35);
        } else {
            gSpriteData[slot].pose = SPOSE_PUSHED_LEFT_INIT;
            m4aSongNumStart(SOUND_34);
        }
    }
}

void func_801E92C(s32 slot)
{
    switch (gWarioData.reaction) {
        case REACTION_FLAMING:
            if (gWarioData.pose <= WPOSE_FLAMING_RED_MIDAIR) {
                func_801E884(slot);
                break;
            }
            gSpriteData[slot].pose = SPOSE_6B;
            break;

        case REACTION_FAT:
            if (gWarioData.pose != 0) {
                if (gSpriteData[slot].xPosition > gWarioData.xPosition) {
                    gSpriteData[slot].pose = SPOSE_TACKLED_RIGHT_INIT;
                } else {
                    gSpriteData[slot].pose = SPOSE_TACKLED_LEFT_INIT;
                }
                if (gWarioData.pose != WPOSE_FAT_DETRANSFORMING) {
                    WarioRequestPose(WPOSE_FAT_KILLING_ENEMY);
                }
                m4aSongNumStart(SOUND_38);
                break;
            }
            func_801E884(slot);
            break;

        case REACTION_ZOMBIE:
            if (gSpriteData[slot].xPosition > gWarioData.xPosition) {
                gSpriteData[slot].pose = SPOSE_TACKLED_RIGHT_INIT;
            } else {
                gSpriteData[slot].pose = SPOSE_TACKLED_LEFT_INIT;
            }
            if (gWarioData.unk_05 != 0) {
                WarioRequestPose(WPOSE_ZOMBIE_UNKNOWN_5);
            }
            m4aSongNumStart(SOUND_38);
            break;

        case REACTION_FROZEN:
        case REACTION_SNOWMAN:
            if (gSpriteData[slot].xPosition > gWarioData.xPosition) {
                gSpriteData[slot].pose = SPOSE_TACKLED_RIGHT_INIT;
            } else {
                gSpriteData[slot].pose = SPOSE_TACKLED_LEFT_INIT;
            }
            m4aSongNumStart(SOUND_38);
            break;

        case REACTION_BAT:
            gSpriteData[slot].pose = SPOSE_6B;
            break;

        default:
            func_801E884(slot);
            break;
    }
}

void SpriteCollisionTransformWarioFlaming(void)
{
    SpriteCollisionTransformWario(REACTION_FLAMING);
}

void SpriteCollisionTransformWarioFat(void)
{
    SpriteCollisionTransformWario(REACTION_FAT);
}

void SpriteCollisionTransformWarioZombie(void)
{
    SpriteCollisionTransformWario(REACTION_ZOMBIE);
}

void SpriteCollisionTransformWarioSnowman(void)
{
    SpriteCollisionTransformWario(REACTION_SNOWMAN);
}

void SpriteCollisionTransformWarioPuffy(void)
{
    SpriteCollisionTransformWario(REACTION_PUFFY);
}

void SpriteCollisionTransformWarioBouncy(void)
{
    SpriteCollisionTransformWario(REACTION_BOUNCY);
}

void SpriteCollisionTransformWarioBat(void)
{
    SpriteCollisionTransformWario(REACTION_BAT);
}

void SpriteCollisionTransformWarioFrozen(void)
{
    SpriteCollisionTransformWario(REACTION_FROZEN);
}

void SpriteCollisionTransformWarioFlat(void)
{
    SpriteCollisionTransformWario(REACTION_FLAT);
}

void SpriteCollisionTransformWarioMask(void)
{
    SpriteCollisionTransformWario(REACTION_MASK);
}

void func_801EBCC(s32 slot)
{
    if (gWarioData.reaction == REACTION_NORMAL) {
        if (gWarioData.damageTimer > 0) {
            func_801E884(slot);
            return;
        }

        gWarioData.unk_03 = gWarioData.damageTimer;
        if (gSpriteData[slot].status & SPRITE_STATUS_FACING_RIGHT) {
            gWarioData.horizontalDirection = DPAD_LEFT;
        } else {
            gWarioData.horizontalDirection = DPAD_RIGHT;
        }

        SpriteCollisionTransformWarioFrozen();
        gSpriteData[slot].pose = SPOSE_CRUSHED_OR_COLLECTED_INIT;
    } else {
        func_801E92C(slot);
    }
}

void func_801EC30(s32 slot)
{
    if (gWarioData.reaction == REACTION_NORMAL) {
        if (gWarioData.damageTimer > 0) {
            func_801E884(slot);
            return;
        }

        SpriteCollisionTransformWarioPuffy();
        gSpriteData[slot].pose = SPOSE_CRUSHED_OR_COLLECTED_INIT;
    } else {
        func_801E92C(slot);
    }
}

void func_801EC74(s32 slot)
{
    if (gWarioData.reaction == REACTION_NORMAL) {
        if (gWarioData.damageTimer > 0) {
            func_801E884(slot);
            return;
        }

        SpriteCollisionTransformWarioSnowman();
        gSpriteData[slot].pose = SPOSE_CRUSHED_OR_COLLECTED_INIT;
    } else {
        func_801E92C(slot);
    }
}

void func_801ECB8(s32 slot)
{
    if (gWarioData.reaction == REACTION_NORMAL) {
        if (gWarioData.damageTimer > 0) {
            func_801E884(slot);
            return;
        }

        SpriteCollisionTransformWarioZombie();
        gSpriteData[slot].pose = SPOSE_6B;
    } else {
        func_801E92C(slot);
    }
}

void func_801ECFC(s32 slot)
{
    if (gWarioData.reaction != REACTION_NORMAL) {
        return;
    }
    if (gWarioData.damageTimer > 0) {
        return;
    }

    SpriteCollisionTransformWarioZombie();
}

void func_801ED18(void)
{
    if (gWarioData.reaction != REACTION_ZOMBIE) {
        return;
    }
    if (gWarioData.pose == WPOSE_ZOMBIE_TRANSFORMING || gWarioData.pose == WPOSE_ZOMBIE_TOUCHING_LIGHT_SOURCE) {
        return;
    }

    WarioRequestPose(WPOSE_ZOMBIE_TOUCHING_LIGHT_SOURCE);
}

void func_801ED48(s32 slot)
{
    if (gWarioData.reaction == REACTION_NORMAL) {
        if (gWarioData.damageTimer != 0) {
            func_801E884(slot);
            return;
        }

        SpriteCollisionTransformWarioFat();
        gSpriteData[slot].pose = SPOSE_CRUSHED_OR_COLLECTED_INIT;
    } else {
        func_801E92C(slot);
    }
}

void SpriteCollisionMaybeTransformWarioBubble(s32 slot)
{
    if (gWarioData.damageTimer > 0) {
        return;
    }

    WarioRequestPose(WPOSE_WATER_BUBBLE_WARIO);
    gSpriteData[slot].pose = SPOSE_6B;
}

void SpriteCollisionTakeDamageLeft(void)
{
    if (gWarioData.damageTimer > 0) {
        return;
    }

    gWarioData.horizontalDirection = DPAD_LEFT;
    WarioRequestPose(WPOSE_NORMAL_TAKING_DAMAGE);
}

void SpriteCollisionTakeDamageRight(void)
{
    if (gWarioData.damageTimer > 0) {
        return;
    }

    gWarioData.horizontalDirection = DPAD_RIGHT;
    WarioRequestPose(WPOSE_NORMAL_TAKING_DAMAGE);
}

void SpriteCollisionLandOnEnemyLeft(void)
{
    if (gWarioData.damageTimer > 0) {
        return;
    }

    WarioRequestPose(WPOSE_NORMAL_LANDING_ON_ENEMY);
    if (gWarioData.pose == WPOSE_NORMAL_LANDING_ON_ENEMY) {
        gWarioData.horizontalDirection = DPAD_LEFT;
        gWarioData.xVelocity = 0x10;
    }
    m4aSongNumStart(SOUND_35);
}

void SpriteCollisionLandOnEnemyRight(void)
{
    if (gWarioData.damageTimer > 0) {
        return;
    }

    WarioRequestPose(WPOSE_NORMAL_LANDING_ON_ENEMY);
    if (gWarioData.pose == WPOSE_NORMAL_LANDING_ON_ENEMY) {
        gWarioData.horizontalDirection = DPAD_RIGHT;
        gWarioData.xVelocity = -0x10;
    }
    m4aSongNumStart(SOUND_35);
}

void func_801EE9C(void)
{
    if (gWarioData.damageTimer > 0) {
        return;
    }

    WarioRequestPose(WPOSE_NORMAL_LANDING_ON_ENEMY);
    if (gWarioData.unk_1A != 0) {
        gWarioData.yVelocity = 0;
    }
    if (gWarioData.pose == WPOSE_NORMAL_LANDING_ON_ENEMY) {
        gWarioData.horizontalDirection = DPAD_LEFT;
        gWarioData.xVelocity = 0x10;
    }
    m4aSongNumStart(SOUND_35);
}

void func_801EEE0(void)
{
    if (gWarioData.damageTimer > 0) {
        return;
    }

    WarioRequestPose(WPOSE_NORMAL_LANDING_ON_ENEMY);
    if (gWarioData.unk_1A != 0) {
        gWarioData.yVelocity = 0;
    }
    if (gWarioData.pose == WPOSE_NORMAL_LANDING_ON_ENEMY) {
        gWarioData.horizontalDirection = DPAD_RIGHT;
        gWarioData.xVelocity = -0x10;
    }
    m4aSongNumStart(SOUND_35);
}

void SpriteCollisionLandOnEnemy(void)
{
    WarioRequestPose(WPOSE_NORMAL_LANDING_ON_ENEMY);
    m4aSongNumStart(SOUND_35);
}

void func_801EF50(void)
{
    if (gWarioData.damageTimer > 0) {
        return;
    }

    WarioRequestPose(WPOSE_NORMAL_ROLLING_IN_MIDAIR);
    gWarioData.xVelocity = -0x10;
    func_801E4B0();
    gCurrentCarriedSprite.state = 0;
    m4aSongNumStart(SOUND_34);
}

void func_801EF94(void)
{
    if (gWarioData.damageTimer > 0) {
        return;
    }

    WarioRequestPose(WPOSE_NORMAL_ROLLING_IN_MIDAIR);
    gWarioData.xVelocity = 0x10;
    func_801E4B0();
    gCurrentCarriedSprite.state = 0;
    m4aSongNumStart(SOUND_34);
}

u8 func_801EFD4(void)
{
    u8 pose;

    switch (gWarioData.pose) {
        case WPOSE_NORMAL_STARTING_ROLL:
        case WPOSE_NORMAL_JUMPING_OUT_OF_ROLL:
        case WPOSE_NORMAL_ROLLING:
        case WPOSE_NORMAL_ROLLING_JUMP:
            m4aSongNumStart(SOUND_38);
            if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
                pose = SPOSE_TACKLED_RIGHT_INIT;
            } else {
                pose = SPOSE_TACKLED_LEFT_INIT;
            }
            break;

        default:
            pose = 0;
            break;
    }

    return pose;
}

u8 func_801F00C(s32 slot, u16 direction, u8 arg2)
{
    u8 pose;

    switch (gWarioData.pose) {
        case WPOSE_NORMAL_SHOULDER_BASH:
            if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
                if (direction & DPAD_RIGHT) {
                    pose = SPOSE_TACKLED_RIGHT_INIT;
                    WarioRequestPose(WPOSE_NORMAL_STANDING);
                    m4aSongNumStart(SOUND_38);
                } else {
                    pose = SPOSE_33;
                }
            } else {
                if (direction & DPAD_LEFT) {
                    pose = SPOSE_TACKLED_LEFT_INIT;
                    WarioRequestPose(WPOSE_NORMAL_STANDING);
                    m4aSongNumStart(SOUND_38);
                } else {
                    pose = SPOSE_35;
                }
            }
            break;

        case WPOSE_NORMAL_SHOULDER_BASH_JUMP:
            if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
                if (direction & DPAD_RIGHT) {
                    pose = SPOSE_TACKLED_RIGHT_INIT;
                    WarioRequestPose(-2);
                    m4aSongNumStart(SOUND_38);
                } else {
                    pose = SPOSE_33;
                }
            } else {
                if (direction & DPAD_LEFT) {
                    pose = SPOSE_TACKLED_LEFT_INIT;
                    WarioRequestPose(-2);
                    m4aSongNumStart(SOUND_38);
                } else {
                    pose = SPOSE_35;
                }
            }
            break;

        case WPOSE_NORMAL_DASH_ATTACK:
        case WPOSE_NORMAL_DASH_ATTACK_JUMP:
            if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
                if (direction & DPAD_RIGHT) {
                    pose = SPOSE_4F;
                    m4aSongNumStart(SOUND_38);
                } else {
                    pose = SPOSE_33;
                }
            } else {
                if (direction & DPAD_LEFT) {
                    pose = SPOSE_50;
                    m4aSongNumStart(SOUND_38);
                } else {
                    pose = SPOSE_35;
                }
            }
            break;

        default:
            if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
                pose = SPOSE_33;
                if (arg2) {
                    SpriteCollisionLandOnEnemyRight();
                } else {
                    func_801EEE0();
                }
            } else {
                pose = SPOSE_35;
                if (arg2) {
                    SpriteCollisionLandOnEnemyLeft();
                } else {
                    func_801EE9C();
                }
            }
            break;
    }

    return pose;
}

u8 func_801F14C(s32 slot, u8 right)
{
    u8 pose;

    switch (gWarioData.pose) {
        case WPOSE_NORMAL_SHOULDER_BASH:
            if (right) {
                pose = SPOSE_TACKLED_RIGHT_INIT;
            } else {
                pose = SPOSE_TACKLED_LEFT_INIT;
            }
            WarioRequestPose(WPOSE_NORMAL_STANDING);
            m4aSongNumStart(SOUND_38);
            break;

        case WPOSE_NORMAL_SHOULDER_BASH_JUMP:
            if (right) {
                pose = SPOSE_TACKLED_RIGHT_INIT;
            } else {
                pose = SPOSE_TACKLED_LEFT_INIT;
            }
            WarioRequestPose(-2);
            m4aSongNumStart(SOUND_38);
            break;

        case WPOSE_NORMAL_DASH_ATTACK:
        case WPOSE_NORMAL_DASH_ATTACK_JUMP:
            if (right) {
                pose = SPOSE_4F;
            } else {
                pose = SPOSE_50;
            }
            m4aSongNumStart(SOUND_38);
            break;

        default:
            if (gSpriteData[slot].warioCollision == 5) {
                if (right) {
                    pose = SPOSE_33;
                } else {
                    pose = SPOSE_35;
                }
            } else {
                if (right) {
                    pose = SPOSE_PUSHED_RIGHT_INIT;
                } else {
                    pose = SPOSE_PUSHED_LEFT_INIT;
                }
            }
            if (right) {
                func_801EF50();
            } else {
                func_801EF94();
            }
            break;
    }

    return pose;
}

u8 func_801F200(s32 slot, u16 direction)
{
    u8 pose;

    switch (gWarioData.pose) {
        case WPOSE_NORMAL_SHOULDER_BASH:
            if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
                if (direction & DPAD_RIGHT) {
                    pose = SPOSE_TACKLED_RIGHT_INIT;
                    WarioRequestPose(WPOSE_NORMAL_STANDING);
                    m4aSongNumStart(SOUND_38);
                } else {
                    pose = SPOSE_33;
                }
            } else {
                if (direction & DPAD_LEFT) {
                    pose = SPOSE_TACKLED_LEFT_INIT;
                    WarioRequestPose(WPOSE_NORMAL_STANDING);
                    m4aSongNumStart(SOUND_38);
                } else {
                    pose = SPOSE_35;
                }
            }
            break;

        case WPOSE_NORMAL_SHOULDER_BASH_JUMP:
            if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
                if (direction & DPAD_RIGHT) {
                    pose = SPOSE_TACKLED_RIGHT_INIT;
                    WarioRequestPose(-2);
                    m4aSongNumStart(SOUND_38);
                } else {
                    pose = SPOSE_33;
                }
            } else {
                if (direction & DPAD_LEFT) {
                    pose = SPOSE_TACKLED_LEFT_INIT;
                    WarioRequestPose(-2);
                    m4aSongNumStart(SOUND_38);
                } else {
                    pose = SPOSE_35;
                }
            }
            break;

        case WPOSE_NORMAL_DASH_ATTACK:
        case WPOSE_NORMAL_DASH_ATTACK_JUMP:
            if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
                if (direction & DPAD_RIGHT) {
                    pose = SPOSE_4F;
                    m4aSongNumStart(SOUND_38);
                } else {
                    pose = SPOSE_33;
                }
            } else {
                if (direction & DPAD_LEFT) {
                    pose = SPOSE_50;
                    m4aSongNumStart(SOUND_38);
                    break;
                } else {
                    pose = SPOSE_35;
                }
            }
            break;

        case WPOSE_NORMAL_CROUCHING:
        case WPOSE_NORMAL_CROUCH_SLIDE:
        case WPOSE_NORMAL_CRAWLING:
        case WPOSE_NORMAL_TURNING_CROUCHED:
        case WPOSE_NORMAL_CROUCH_JUMP:
        case WPOSE_NORMAL_TAKING_DAMAGE:
        case WPOSE_NORMAL_BUMPING_INTO_ENEMY:
            if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
                pose = SPOSE_33;
            } else {
                pose = SPOSE_35;
            }
            break;

        default:
            if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
                if (direction & DPAD_RIGHT) {
                    if (gSpriteData[slot].status & SPRITE_STATUS_HEAVY) {
                        gCurrentCarriedSprite.unk1 = 2;
                    } else {
                        gCurrentCarriedSprite.unk1 = 1;
                    }
                    gCurrentCarriedSprite.state = 4;
                    gWarioData.unk_08 = 2;
                    WarioRequestPose(-2);
                    pose = SPOSE_CARRIED_RIGHT_INIT;
                    m4aSongNumStart(SOUND_32);
                } else {
                    pose = SPOSE_33;
                    func_801EEE0();
                }
            } else {
                if (direction & DPAD_LEFT) {
                    if (gSpriteData[slot].status & SPRITE_STATUS_HEAVY) {
                        gCurrentCarriedSprite.unk1 = 2;
                    } else {
                        gCurrentCarriedSprite.unk1 = 1;
                    }
                    gCurrentCarriedSprite.state = 4;
                    gWarioData.unk_08 = 2;
                    WarioRequestPose(-2);
                    pose = SPOSE_CARRIED_LEFT_INIT;
                    m4aSongNumStart(SOUND_32);
                } else {
                    pose = SPOSE_35;
                    func_801EE9C();
                }
            }
            break;
    }

    return pose;
}

u8 func_801F43C(s32 slot, u8 right)
{
    u8 pose;

    switch (gWarioData.pose) {
        case WPOSE_NORMAL_SHOULDER_BASH:
            if (right) {
                pose = SPOSE_TACKLED_RIGHT_INIT;
            } else {
                pose = SPOSE_TACKLED_LEFT_INIT;
            }
            WarioRequestPose(WPOSE_NORMAL_STANDING);
            m4aSongNumStart(SOUND_38);
            break;

        case WPOSE_NORMAL_SHOULDER_BASH_JUMP:
            if (right) {
                pose = SPOSE_TACKLED_RIGHT_INIT;
            } else {
                pose = SPOSE_TACKLED_LEFT_INIT;
            }
            WarioRequestPose(-2);
            m4aSongNumStart(SOUND_38);
            break;

        case WPOSE_NORMAL_DASH_ATTACK:
        case WPOSE_NORMAL_DASH_ATTACK_JUMP:
            if (right) {
                pose = SPOSE_4F;
            } else {
                pose = SPOSE_50;
            }
            m4aSongNumStart(SOUND_38);
            break;

        case WPOSE_NORMAL_FALLING:
            if (right) {
                pose = SPOSE_CARRIED_RIGHT_INIT;
            } else {
                pose = SPOSE_CARRIED_LEFT_INIT;
            }
            if (gSpriteData[slot].status & SPRITE_STATUS_HEAVY) {
                gCurrentCarriedSprite.unk1 = 2;
            } else {
                gCurrentCarriedSprite.unk1 = 1;
            }
            gCurrentCarriedSprite.state = 4;
            gWarioData.unk_08 = 2;
            WarioRequestPose(-2);
            m4aSongNumStart(SOUND_32);
            break;

        case WPOSE_NORMAL_WALKING:
        case WPOSE_NORMAL_STANDING:
            gSpriteData[slot].status &= ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES;
            if (right) {
                pose = SPOSE_BEING_LIFTED_RIGHT_INIT;
            } else {
                pose = SPOSE_BEING_LIFTED_LEFT_INIT;
            }
            if (gSpriteData[slot].status & SPRITE_STATUS_HEAVY) {
                WarioRequestPose(WPOSE_NORMAL_LIFTING_HEAVY_SPRITE);
            } else {
                WarioRequestPose(WPOSE_NORMAL_LIFTING_SPRITE);
            }
            m4aSongNumStart(SOUND_32);
            break;

        default:
            if (right) {
                pose = SPOSE_33;
            } else {
                pose = SPOSE_35;
            }
            break;
    }

    return pose;
}

s32 SpriteCollisionCheckObjectsTouching(
    u16 o1Top,
    u16 o1Bottom,
    u16 o1Left,
    u16 o1Right,
    u16 o2Top,
    u16 o2Bottom,
    u16 o2Left,
    u16 o2Right
)
{
    if ((o2Bottom >= o1Top) && (o2Top < o1Bottom) && (o2Right >= o1Left) && (o2Left < o1Right)) {
        return TRUE;
    }
    return FALSE;
}

void SpriteCollisionProcess(void)
{
    s32 slot;

    register struct WarioData* collisionWario PIN("r6");
    u16 warioY;
    u16 warioX;
    u16 warioTop;
    u16 warioBottom;
    u16 warioLeft;
    u16 warioRight;
    u16 previousX;

    u32 spriteY;
    u16 spriteX;
    u16 hitboxDown;
    u16 spriteTop;
    u16 spriteBottom;
    u16 spriteLeft;
    u16 spriteRight;
    u32 status;

    u16 verticalCenter;
    u16 horizontalCenter;
    u16 statusFlags;
    u16 warioDirection;
    u8 reaction;

    if (gTimerState > 3 || gWarioData.unk_02) {
        return;
    }

    warioY = gWarioData.yPosition;
    warioX = gWarioData.xPosition;
    previousX = gPreviousXPosition;

    warioTop = warioY + gWarioData.hitboxOffsetTop;
    warioBottom = warioY + gWarioData.hitboxOffsetBottom;
    warioLeft = gWarioData.xPosition + gWarioData.hitboxOffsetLeft;
    warioRight = gWarioData.xPosition + gWarioData.hitboxOffsetRight;

    statusFlags = SPRITE_STATUS_EXISTS | SPRITE_STATUS_ONSCREEN;
    for (slot = 0; slot < MAX_SPRITE_COUNT; slot++) {
        status = gSpriteData[slot].status;
        if ((status & statusFlags) == statusFlags) {
            if (gSpriteData[slot].disableWarioCollisionTimer > 0) {
                gSpriteData[slot].disableWarioCollisionTimer -= 1;
                continue;
            }

            spriteY = gSpriteData[slot].yPosition;
            spriteX = gSpriteData[slot].xPosition;
            spriteTop = spriteY - gSpriteData[slot].hitboxExtentUp;
            spriteLeft = spriteX - gSpriteData[slot].hitboxExtentLeft;
            spriteRight = spriteX + gSpriteData[slot].hitboxExtentRight;
            hitboxDown = gSpriteData[slot].hitboxExtentDown;
            /* hitboxExtentDown is sign-magnitude: bit 7 puts the bottom edge above the sprite's origin. */
            if (hitboxDown <= S8_MAX) {
                spriteBottom = spriteY - -(s32)hitboxDown;
            } else {
                /* Both branches must leave the sum in r0 so they share one u16 truncation into r5. */
                register u16 mask PIN("r5") = hitboxDown & S8_MAX;
                register u16 res PIN("r0") = spriteY - mask;
                spriteBottom = res;
            }

            if (!SpriteCollisionCheckObjectsTouching(
                    warioTop, warioBottom, warioLeft, warioRight, spriteTop, spriteBottom, spriteLeft, spriteRight
                )) {
                continue;
            }

            gIgnoreSpriteCollision = FALSE;

            verticalCenter = spriteTop + (spriteBottom - spriteTop) / 2;
            horizontalCenter = spriteLeft + (spriteRight - spriteLeft) / 2;
            gSpriteCollisionFlags = SPRITE_COLLISION_NONE;
            {
                s32 aboveCheck = verticalCenter - PIXEL_SIZE;
                collisionWario = &gWarioData;
                if (aboveCheck > warioBottom && collisionWario->yVelocity <= 0x18) {
                    gSpriteCollisionFlags = SPRITE_COLLISION_ABOVE;
                }
            }
            if ((verticalCenter + PIXEL_SIZE) < warioTop) {
                gSpriteCollisionFlags |= SPRITE_COLLISION_BELOW;
            }
            if (horizontalCenter >= warioX) {
                gSpriteCollisionFlags |= SPRITE_COLLISION_LEFT;
            } else {
                gSpriteCollisionFlags |= SPRITE_COLLISION_RIGHT;
            }

#ifdef PORTABLE
            PortRumbleContactBegin(slot);
#endif
            warioDirection = collisionWario->horizontalDirection;
            if (collisionWario->unk_1A != 3) {
                switch (gSpriteData[slot].warioCollision) {
                    case 0x1:
                        if (collisionWario->reaction == 0) {
                            func_8020B10(slot, warioDirection);
                        } else {
                            func_801E92C(slot);
                        }
                        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
                        gIgnoreSpriteCollision = 1;
                        break;
                    case 0x2:
                        if (collisionWario->reaction == 0) {
                            func_8020BB8(slot, warioDirection);
                        } else {
                            func_801E92C(slot);
                        }
                        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
                        gIgnoreSpriteCollision = 1;
                        break;
                    case 0x3:
                        if (collisionWario->reaction == 0) {
                            func_8020E1C(slot, warioDirection);
                        } else {
                            func_801E92C(slot);
                        }
                        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
                        gIgnoreSpriteCollision = 1;
                        break;
                    case 0x4:
                        if (collisionWario->reaction == 0) {
                            func_8020CEC(slot, warioDirection, spriteLeft, spriteRight, warioX);
                        } else {
                            func_801E92C(slot);
                        }
                        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
                        gIgnoreSpriteCollision = 1;
                        break;
                    case 0x21:
                        if (collisionWario->reaction == 0) {
                            func_8020F28(slot, warioDirection);
                        } else {
                            func_801E92C(slot);
                        }
                        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
                        gIgnoreSpriteCollision = 1;
                        break;
                    case 0xA:
                        if (collisionWario->reaction == 0) {
                            func_80211E0(slot);
                        } else {
                            func_801E92C(slot);
                        }
                        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
                        gIgnoreSpriteCollision = 1;
                        break;
                    case 0x5:
                        if (collisionWario->reaction == REACTION_NORMAL) {
                            if (gCurrentCarriedSprite.state != 0) {
                                func_8020B10(slot, warioDirection);
                            } else {
                                if (!(gSpriteCollisionFlags & SPRITE_COLLISION_BELOW)) {
                                    func_8023BFC(spriteY - 0xA0, spriteX);
                                    if (gSpriteCollisionTileType & 0xF) {
                                        func_8023BFC(warioY - 0x60, warioX);
                                        if (!(0xF & gSpriteCollisionTileType) ||
                                            ((collisionWario->pose < 5) || (collisionWario->pose > 9))) {
                                            func_8020B10(slot, warioDirection);
                                        }
                                    } else {
                                        func_802125C(slot, warioDirection);
                                    }
                                } else {
                                    func_802125C(slot, warioDirection);
                                }
                            }
                        } else {
                            func_801E92C(slot);
                        }
                        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
                        gIgnoreSpriteCollision = 1;
                        break;
                    case 0x3B:
                        if (collisionWario->reaction == 0) {
                            if (gCurrentCarriedSprite.state != 0) {
                                func_8020F28(slot, warioDirection);
                            } else {
                                func_8021318(slot, warioDirection);
                            }
                        } else {
                            func_801E92C(slot);
                        }
                        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
                        gIgnoreSpriteCollision = 1;
                        break;
                    case 0x25:
                        if (collisionWario->reaction == 0) {
                            func_8020FF4(slot, warioDirection);
                        } else {
                            func_801E92C(slot);
                        }
                        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
                        gIgnoreSpriteCollision = 1;
                        break;
                    case 0x26:
                        if (collisionWario->reaction == 0) {
                            func_80210E8(slot, warioDirection);
                        } else {
                            func_801E92C(slot);
                        }
                        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
                        gIgnoreSpriteCollision = 1;
                        break;
                    case 0x6:
                        func_8021720(slot);
                        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
                        gIgnoreSpriteCollision = 1;
                        break;
                    case 0x46:
                        gSpriteData[slot].pose = SPOSE_70;
                        gIgnoreSpriteCollision = 1;
                        break;
                    case 0x7:
                        func_8021734(slot);
                        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
                        gIgnoreSpriteCollision = 1;
                        break;
                    case 0x35:
                        if (collisionWario->reaction == 0) {
                            func_80236C4(slot, warioDirection, spriteLeft, spriteRight);
                        } else {
                            gSpriteData[slot].pose = SPOSE_CRUSHED_OR_COLLECTED_INIT;
                        }
                        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
                        gIgnoreSpriteCollision = 1;
                        break;
                    case 0x8:
                        func_8021564(slot);
                        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
                        gIgnoreSpriteCollision = 1;
                        break;
                    case 0xC:
                        if (collisionWario->reaction == 0) {
                            func_8021650(slot);
                        } else {
                            func_801E92C(slot);
                        }
                        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
                        gIgnoreSpriteCollision = 1;
                        break;
                    case 0x9:
                        if (collisionWario->reaction == 0) {
                            func_80215E8(slot);
                        } else {
                            func_801E92C(slot);
                        }
                        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
                        gIgnoreSpriteCollision = 1;
                        break;
                    case 0xB:
                        reaction = collisionWario->reaction;
                        if (reaction == 0) {
                            func_80216B8(slot);
                        } else if (reaction == 8) {
                            collisionWario->reaction = 0;
                            if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
                                SpriteCollisionTakeDamageRight();
                            } else {
                                SpriteCollisionTakeDamageLeft();
                            }
                            gSpriteData[slot].pose = SPOSE_6C;
                        } else {
                            func_801E92C(slot);
                        }
                        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
                        gIgnoreSpriteCollision = 1;
                        break;
                    case 0xD:
                        func_8021500(slot);
                        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
                        gIgnoreSpriteCollision = 1;
                        break;
                    case 0xE:
                        if ((collisionWario->reaction == 0) && (collisionWario->damageTimer == 0)) {
                            SpriteCollisionTransformWarioFlaming();
                        }
                        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
                        gIgnoreSpriteCollision = 1;
                        break;
                    case 0xF:
                        func_801ED48(slot);
                        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
                        gIgnoreSpriteCollision = 1;
                        break;
                    case 0x10:
                        func_801ECB8(slot);
                        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
                        gIgnoreSpriteCollision = 1;
                        break;
                    case 0x11:
                        func_801ECFC(slot);
                        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
                        gIgnoreSpriteCollision = 1;
                        break;
                    case 0x12:
                        func_801EC74(slot);
                        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
                        gIgnoreSpriteCollision = 1;
                        break;
                    case 0x27:
                        func_801EC30(slot);
                        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
                        gIgnoreSpriteCollision = 1;
                        break;
                    case 0x28:
                        func_8022C64(slot);
                        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
                        gIgnoreSpriteCollision = 1;
                        break;
                    case 0x14:
                        func_801EBCC(slot);
                        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
                        gIgnoreSpriteCollision = 1;
                        break;
                    case 0x13:
                        if ((collisionWario->reaction == 0) && (collisionWario->damageTimer == 0)) {
                            SpriteCollisionTransformWarioBat();
                        }
                        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
                        gIgnoreSpriteCollision = 1;
                        break;
                    case 0x15:
                        if ((collisionWario->reaction == 0) && (collisionWario->damageTimer == 0)) {
                            func_8020C78(slot, warioDirection);
                        }
                        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
                        gIgnoreSpriteCollision = 1;
                        break;
                    case 0x1B:
                        func_8021C30(slot, spriteTop);
                        break;
                    case 0x1C:
                        func_8021CC8(slot, spriteTop);
                        break;
                    case 0x1D:
                        func_8021D5C(slot, spriteTop);
                        break;
                    case 0x30:
                        func_8021DD0(slot, spriteTop);
                        break;
                    case 0x1E:
                        if ((collisionWario->reaction != 0) || (collisionWario->damageTimer != 0)) {
                            func_8021784(
                                slot,
                                warioDirection,
                                spriteTop,
                                spriteBottom,
                                spriteLeft,
                                spriteRight,
                                warioLeft,
                                warioRight
                            );
                        } else {
                            func_8021E6C(slot, spriteBottom, spriteLeft, spriteRight);
                            gIgnoreSpriteCollision = 1;
                        }
                        break;
                    case 0x1F:
                        if (gSpriteCollisionFlags & SPRITE_COLLISION_BELOW) {
                            reaction = collisionWario->reaction;
                            if (reaction == 0 || reaction == 2) {
                                if (collisionWario->damageTimer != 0) {
                                    collisionWario->damageTimer = 0;
                                }
                                SpriteCollisionTransformWarioFlat();
                                gSpriteData[slot].work1 = 1;
                                gSpriteData[slot].disableWarioCollisionTimer = 0xF;
                                collisionWario->yPosition = gSpriteData[slot].yPosition + 0x90;
                            } else if (reaction != 10) {
                                func_8021E6C(slot, spriteBottom, spriteLeft, spriteRight);
                            }
                            gIgnoreSpriteCollision = 1;
                            break;
                        }
                        if (collisionWario->reaction == 0 && collisionWario->damageTimer == 0) {
                            goto solidSpriteCollision;
                        }
                    case 0x16:
                    case 0x17:
                    case 0x18:
                    case 0x19:
                    case 0x1A:
                        func_8021784(
                            slot,
                            warioDirection,
                            spriteTop,
                            spriteBottom,
                            spriteLeft,
                            spriteRight,
                            warioLeft,
                            warioRight
                        );
                        break;
                    solidSpriteCollision:
                        /* Calling it as a value-returning function keeps agbcc from merging this call into the
                         * identical one in case 0x1E. */
                        ((s32 (*)(s32, u16, u16, u16))func_8021E6C)(slot, spriteBottom, spriteLeft, spriteRight);
                        gIgnoreSpriteCollision = 1;
                        break;
                    case 0x20:
                        func_80220F8(slot);
                        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
                        gIgnoreSpriteCollision = 1;
                        break;
                    case 0x22:
                        func_8022118(slot, warioX, previousX);
                        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
                        gIgnoreSpriteCollision = 1;
                        break;
                    case 0x23:
                        if (collisionWario->reaction == 0) {
                            func_8022188(slot, warioDirection);
                        } else {
                            func_801E92C(slot);
                        }
                        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
                        gIgnoreSpriteCollision = 1;
                        break;
                    case 0x24:
                        if (collisionWario->reaction == 0) {
                            func_8022220(slot, warioDirection);
                        } else {
                            func_801E92C(slot);
                        }
                        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
                        gIgnoreSpriteCollision = 1;
                        break;
                    case 0x29:
                    case 0x2A:
                    case 0x2B:
                        if (collisionWario->reaction == 0 && collisionWario->damageTimer == 0) {
                            func_80222D8(slot, spriteBottom, spriteLeft, spriteRight);
                            gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
                            gIgnoreSpriteCollision = 1;
                        }
                        break;
                    case 0x48:
                        func_8022524(slot, spriteTop, spriteBottom, warioTop, warioX);
                        gIgnoreSpriteCollision = 1;
                        break;
                    case 0x42:
                    case 0x43:
                    case 0x44:
                        if (collisionWario->reaction == 0) {
                            func_8022724(slot, warioDirection);
                            gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
                            gIgnoreSpriteCollision = 1;
                        }
                        break;
                    case 0x45:
                        if (collisionWario->reaction == 0 && collisionWario->damageTimer == 0) {
                            func_80215C0();
                            gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
                            gIgnoreSpriteCollision = 1;
                            break;
                        }
                        break;
                    case 0x2C:
                        if (collisionWario->reaction == 0 && collisionWario->damageTimer == 0) {
                            func_8022EF8(slot, spriteLeft, spriteRight);
                            gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
                            gIgnoreSpriteCollision = 1;
                            break;
                        }
                        break;
                    case 0x2D:
                        if (collisionWario->reaction == 0 && collisionWario->damageTimer == 0) {
                            func_8022FC8(slot, spriteLeft, spriteRight);
                            gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
                            gIgnoreSpriteCollision = 1;
                            break;
                        }
                        break;
                    case 0x47:
                        if (collisionWario->reaction == 0 && collisionWario->damageTimer == 0) {
                            func_8023110(slot, spriteLeft, spriteRight);
                            gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
                            gIgnoreSpriteCollision = 1;
                            break;
                        }
                        break;
                    case 0x2E:
                        if (collisionWario->reaction == 0) {
                            func_80233B8(slot, warioDirection);
                        } else {
                            func_801E92C(slot);
                        }
                        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
                        gIgnoreSpriteCollision = 1;
                        break;
                    case 0x2F:
                        if (collisionWario->reaction == 0) {
                            func_802349C(slot, warioDirection);
                        } else {
                            func_801E92C(slot);
                        }
                        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
                        gIgnoreSpriteCollision = 1;
                        break;
                    case 0x31:
                    case 0x32:
                        if (collisionWario->reaction == 0) {
                            func_80235A0(slot, warioDirection);
                        } else {
                            func_801E92C(slot);
                        }
                        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
                        gIgnoreSpriteCollision = 1;
                        break;
                    case 0x33:
                        if (collisionWario->reaction == 0) {
                            func_80231F8(slot, warioDirection);
                        } else {
                            func_801E92C(slot);
                        }
                        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
                        gIgnoreSpriteCollision = 1;
                        break;
                    case 0x34:
                        if (collisionWario->reaction == 0) {
                            func_80232C4(slot, warioDirection);
                        } else {
                            func_801E92C(slot);
                        }
                        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
                        gIgnoreSpriteCollision = 1;
                        break;
                    case 0x36:
                        if (collisionWario->reaction == 0 && collisionWario->damageTimer == 0) {
                            gSpriteData[slot].pose = SPOSE_6C;
                            if (gCurrentCarriedSprite.state != 0) {
                                func_801E4B0();
                                gCurrentCarriedSprite.state = 0;
                            }
                        }
                        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
                        gIgnoreSpriteCollision = 1;
                        break;
                    case 0x37:
                        if (collisionWario->reaction == 0) {
                            func_8020958(slot, warioDirection);
                        } else {
                            func_801E92C(slot);
                        }
                        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
                        gIgnoreSpriteCollision = 1;
                        break;
                    case 0x38:
                        if ((collisionWario->reaction == 0) && (collisionWario->damageTimer == 0)) {
                            func_80213F4(slot, warioX, previousX);
                        }
                        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
                        gIgnoreSpriteCollision = 1;
                        break;
                    case 0x39:
                    case 0x3A:
                        if ((collisionWario->reaction == 0) && (collisionWario->damageTimer == 0)) {
                            func_8021434(slot, spriteLeft, spriteRight);
                        }
                        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
                        gIgnoreSpriteCollision = 1;
                        break;
                    case 0x3C:
                        if (collisionWario->reaction == 0) {
                            func_80237E4(slot);
                        }
                        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
                        gIgnoreSpriteCollision = 1;
                        break;
                    case 0x3D:
                        func_801ED18();
                        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
                        gIgnoreSpriteCollision = 1;
                        break;
                    case 0x3E:
                        if (collisionWario->reaction == 0 && collisionWario->damageTimer == 0) {
                            func_8021F84(slot, spriteLeft, spriteRight, spriteTop);
                        }
                        break;
                    case 0x3F:
                    case 0x40:
                        if (collisionWario->reaction == 0 && collisionWario->damageTimer == 0) {
                            func_8022948(slot, spriteTop);
                        }
                        gIgnoreSpriteCollision = 1;
                        break;
                    case 0x41:
                        if (collisionWario->reaction == 0 && collisionWario->damageTimer == 0) {
                            func_8022AE8(slot, spriteLeft, spriteRight);
                            gIgnoreSpriteCollision = 1;
                        }
                        break;
                    case 0x49:
                        if (collisionWario->reaction == 0 && collisionWario->damageTimer == 0) {
                            func_8022CE8(
                                slot, warioY, warioX, warioLeft, warioRight, spriteLeft, spriteRight, warioDirection
                            );
                        }
                        gIgnoreSpriteCollision = 1;
                        break;
                    default:
                        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
                        gIgnoreSpriteCollision = 1;
                        break;
                }
            } else {
                switch (gSpriteData[slot].warioCollision) {
                    case 0x6:
                        func_8021720(slot);
                        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
                        break;
                    case 0x4A:
                        SpriteCollisionMaybeTransformWarioBubble(slot);
                        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
                        break;
                    case 0x4B:
                    case 0x4C:
                    case 0x4D:
                        if (collisionWario->damageTimer == 0) {
                            func_80204F4(slot, spriteLeft, spriteRight, warioX);
                        }
                        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.5) + DELTA_TIME;
                        break;
                    case 0x53:
                    case 0x54:
                        if (collisionWario->damageTimer == 0) {
                            func_80209E0(slot, spriteLeft, spriteRight, warioX);
                        }
                        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.5) + DELTA_TIME;
                        break;
                    case 0x4E:
                        if (collisionWario->damageTimer == 0) {
                            func_80206B8(slot, spriteLeft, spriteRight, warioX);
                        }
                        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.5) + DELTA_TIME;
                        break;
                    case 0x4F:
                    case 0x50:
                        if (collisionWario->damageTimer == 0) {
                            func_80207D8(slot);
                        }
                        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.5) + DELTA_TIME;
                        break;
                    case 0x51:
                        if (collisionWario->damageTimer == 0) {
                            SpriteCollisionTakeDamageUnderwater(slot);
                        }
                        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.5) + DELTA_TIME;
                        break;
                    case 0x52:
                        if (collisionWario->damageTimer == 0) {
                            SpriteCollisionUnderwaterBonk(slot, spriteLeft, spriteRight, warioX);
                        }
                        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.5) + DELTA_TIME;
                        break;
                    default:
                        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
                        break;
                }
                gIgnoreSpriteCollision = 1;
            }

#ifdef PORTABLE
            PortRumbleContactEnd(slot);
#endif
            if (gIgnoreSpriteCollision) {
                break;
            }
        } else {
            if (gSpriteData[slot].disableWarioCollisionTimer != 0) {
                gSpriteData[slot].disableWarioCollisionTimer -= 1;
            }
        }
    }
}

void SpriteCollisionTakeDamageUnderwater(s32 slot)
{
    WarioRequestPose(WPOSE_WATER_TAKING_DAMAGE);
    SpriteCollisionMakeWarioDropCoins(slot);
    gSpriteData[slot].pose = SPOSE_6C;
}

void SpriteCollisionUnderwaterBonk(s32 slot, u16 left, u16 right, u16 xPosition)
{
    if (gSpriteCollisionFlags & SPRITE_COLLISION_ABOVE) {
        gWarioData.verticalDirection = DPAD_DOWN;
        WarioRequestPose(WPOSE_WATER_BONK_VERTICAL);
    } else if ((gSpriteCollisionFlags & SPRITE_COLLISION_BELOW) && (left < xPosition) && (xPosition < right)) {
        gWarioData.verticalDirection = DPAD_UP;
        WarioRequestPose(WPOSE_WATER_BONK_VERTICAL);
    } else if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
        gWarioData.horizontalDirection = DPAD_RIGHT;
        WarioRequestPose(WPOSE_WATER_BONK_HORIZONTAL);
    } else if (gSpriteCollisionFlags & SPRITE_COLLISION_RIGHT) {
        gWarioData.horizontalDirection = DPAD_LEFT;
        WarioRequestPose(WPOSE_WATER_BONK_HORIZONTAL);
    }
}

void func_80204F4(s32 slot, u16 left, u16 right, u16 xPosition)
{
    u8 warioCollision;
    u8 pose;

    switch (gWarioData.pose) {
        case WPOSE_NORMAL_STARTING_ROLL:
        case WPOSE_NORMAL_JUMPING_OUT_OF_ROLL:
            if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
                gSpriteData[slot].pose = SPOSE_TACKLED_RIGHT_INIT;
            } else {
                gSpriteData[slot].pose = SPOSE_TACKLED_LEFT_INIT;
            }
            m4aSongNumStart(SOUND_3C);
            return;
    }

    warioCollision = gSpriteData[slot].warioCollision;
    if (gSpriteCollisionFlags & SPRITE_COLLISION_ABOVE) {
        if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
            pose = SPOSE_33;
        } else {
            pose = SPOSE_35;
        }
        switch (warioCollision) {
            case 0x4B:
            case 0x4D:
                WarioRequestPose(WPOSE_WATER_TAKING_DAMAGE);
                SpriteCollisionMakeWarioDropCoins(slot);
                break;
        }
    } else if ((gSpriteCollisionFlags & SPRITE_COLLISION_BELOW) && (left < xPosition) && (xPosition < right)) {
        if (warioCollision == 0x4C) {
            if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
                pose = SPOSE_27;
            } else {
                pose = SPOSE_29;
            }
            WarioRequestPose(WPOSE_WATER_TAKING_DAMAGE);
            SpriteCollisionMakeWarioDropCoins(slot);
        } else if (warioCollision == 0x4B) {
            if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
                pose = SPOSE_TACKLED_RIGHT_INIT;
            } else {
                pose = SPOSE_TACKLED_LEFT_INIT;
            }
            gWarioData.verticalDirection = DPAD_UP;
            WarioRequestPose(WPOSE_WATER_BONK_VERTICAL);
            m4aSongNumStart(SOUND_3C);
        } else {
            if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
                pose = SPOSE_27;
            } else {
                pose = SPOSE_29;
            }
            gWarioData.verticalDirection = DPAD_UP;
            WarioRequestPose(WPOSE_WATER_BONK_VERTICAL);
        }
    } else if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
        pose = SPOSE_PUSHED_RIGHT_INIT;
        if (warioCollision != 0x4B || (gSpriteData[slot].status & SPRITE_STATUS_FACING_RIGHT)) {
            if (warioCollision == 0x4C || warioCollision == 0x4D) {
                pose = SPOSE_TACKLED_RIGHT_INIT;
                gWarioData.horizontalDirection = DPAD_RIGHT;
                WarioRequestPose(WPOSE_WATER_BONK_HORIZONTAL);
                m4aSongNumStart(SOUND_3C);
            }
        } else {
            WarioRequestPose(WPOSE_WATER_TAKING_DAMAGE);
            SpriteCollisionMakeWarioDropCoins(slot);
        }
    } else if (gSpriteCollisionFlags & SPRITE_COLLISION_RIGHT) {
        pose = SPOSE_PUSHED_LEFT_INIT;
        if (((warioCollision == 0x4B && (gSpriteData[slot].status & SPRITE_STATUS_FACING_RIGHT)) ||
             (warioCollision == 0x4C || warioCollision == 0x4D))) {
            WarioRequestPose(WPOSE_WATER_TAKING_DAMAGE);
            SpriteCollisionMakeWarioDropCoins(slot);
        }
    } else {
        pose = gSpriteData[slot].pose;
    }
    gSpriteData[slot].pose = pose;
}

void func_80206B8(s32 slot, u16 left, u16 right, u16 xPosition)
{
    u8 pose;

    switch (gWarioData.pose) {
        case WPOSE_NORMAL_STARTING_ROLL:
        case WPOSE_NORMAL_JUMPING_OUT_OF_ROLL:
            if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
                gSpriteData[slot].pose = SPOSE_TACKLED_RIGHT_INIT;
            } else {
                gSpriteData[slot].pose = SPOSE_TACKLED_LEFT_INIT;
            }
            m4aSongNumStart(SOUND_3C);
            return;
    }

    if ((gSpriteCollisionFlags & SPRITE_COLLISION_ABOVE) && (left < xPosition) && (xPosition < right)) {
        pose = SPOSE_74;
        WarioRequestPose(WPOSE_WATER_UNKNOWN_15);
        SpriteCollisionMakeWarioDropCoins(slot);
    } else if ((gSpriteCollisionFlags & SPRITE_COLLISION_BELOW) && (left < xPosition) && (xPosition < right)) {
        if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
            pose = SPOSE_TACKLED_RIGHT_INIT;
        } else {
            pose = SPOSE_TACKLED_LEFT_INIT;
        }
        m4aSongNumStart(SOUND_3C);
        gWarioData.verticalDirection = DPAD_UP;
        WarioRequestPose(WPOSE_WATER_BONK_VERTICAL);
    } else if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
        pose = SPOSE_PUSHED_RIGHT_INIT;
        WarioRequestPose(WPOSE_WATER_TAKING_DAMAGE);
        SpriteCollisionMakeWarioDropCoins(slot);
    } else if (gSpriteCollisionFlags & SPRITE_COLLISION_RIGHT) {
        pose = SPOSE_PUSHED_LEFT_INIT;
        WarioRequestPose(WPOSE_WATER_TAKING_DAMAGE);
        SpriteCollisionMakeWarioDropCoins(slot);
    } else {
        pose = gSpriteData[slot].pose;
    }

    gSpriteData[slot].pose = pose;
}

void func_80207D8(s32 slot)
{
    u8 pose;

    switch (gWarioData.pose) {
        case WPOSE_NORMAL_CRAWLING:
            if (!(gSpriteCollisionFlags & SPRITE_COLLISION_ABOVE)) {
                break;
            }
        case WPOSE_NORMAL_STARTING_ROLL:
        case WPOSE_NORMAL_JUMPING_OUT_OF_ROLL:
            if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
                gSpriteData[slot].pose = SPOSE_TACKLED_RIGHT_INIT;
            } else {
                gSpriteData[slot].pose = SPOSE_TACKLED_LEFT_INIT;
            }
            m4aSongNumStart(SOUND_3C);
            return;
    }

    if (gSpriteCollisionFlags & SPRITE_COLLISION_ABOVE) {
        if (gSpriteData[slot].status & SPRITE_STATUS_FACING_RIGHT) {
            pose = SPOSE_27;
        } else {
            pose = SPOSE_29;
        }
    } else if (gSpriteCollisionFlags & SPRITE_COLLISION_BELOW) {
        if (gSpriteData[slot].status & SPRITE_STATUS_FACING_RIGHT) {
            pose = SPOSE_27;
        } else {
            pose = SPOSE_29;
        }
        gWarioData.verticalDirection = DPAD_UP;
        WarioRequestPose(WPOSE_WATER_BONK_VERTICAL);
    } else if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
        if ((gSpriteData[slot].status & SPRITE_STATUS_FACING_RIGHT) || gSpriteData[slot].warioCollision != 0x4F) {
            pose = SPOSE_27;
            gWarioData.horizontalDirection = DPAD_RIGHT;
            WarioRequestPose(WPOSE_WATER_BONK_HORIZONTAL);
        } else {
            WarioRequestPose(WPOSE_WATER_TAKING_DAMAGE);
            SpriteCollisionMakeWarioDropCoins(slot);
            return;
        }
    } else if (gSpriteCollisionFlags & SPRITE_COLLISION_RIGHT) {
        if ((gSpriteData[slot].status & SPRITE_STATUS_FACING_RIGHT) && gSpriteData[slot].warioCollision == 0x4F) {
            WarioRequestPose(WPOSE_WATER_TAKING_DAMAGE);
            SpriteCollisionMakeWarioDropCoins(slot);
            return;
        } else {
            pose = SPOSE_29;
            gWarioData.horizontalDirection = DPAD_LEFT;
            WarioRequestPose(WPOSE_WATER_BONK_HORIZONTAL);
        }
    } else {
        pose = gSpriteData[slot].pose;
    }
    gSpriteData[slot].pose = pose;
}

void func_8020958(s32 slot, u16 direction)
{
    u8 pose;

    pose = func_801EFD4();
    if (pose == 0) {
        if (gSpriteCollisionFlags & SPRITE_COLLISION_ABOVE) {
            if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
                SpriteCollisionTakeDamageRight();
                SpriteCollisionMakeWarioDropCoins(slot);
                pose = SPOSE_33;
            } else {
                SpriteCollisionTakeDamageLeft();
                SpriteCollisionMakeWarioDropCoins(slot);
                pose = SPOSE_35;
            }
        } else if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
            pose = func_801F14C(slot, 1);
        } else if (gSpriteCollisionFlags & SPRITE_COLLISION_RIGHT) {
            pose = func_801F14C(slot, 0);
        } else {
            pose = gSpriteData[slot].pose;
        }
    }
    gSpriteData[slot].pose = pose;
}

void func_80209E0(s32 slot, u16 left, u16 right, u16 xPosition)
{
    u8 pose;

    switch (gWarioData.pose) {
        case WPOSE_NORMAL_CRAWLING:
            if (!(gSpriteCollisionFlags & SPRITE_COLLISION_ABOVE)) {
                break;
            }
        case WPOSE_NORMAL_STARTING_ROLL:
        case WPOSE_NORMAL_JUMPING_OUT_OF_ROLL:
            gSpriteData[slot].pose = SPOSE_CRUSHED_OR_COLLECTED_INIT;
            m4aSongNumStart(SOUND_3C);
            return;
    }
    if ((gSpriteCollisionFlags & SPRITE_COLLISION_ABOVE) && (left < xPosition) && (xPosition < right)) {
        if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
            pose = SPOSE_PUSHED_RIGHT_INIT;
        } else {
            pose = SPOSE_PUSHED_LEFT_INIT;
        }
    } else if ((gSpriteCollisionFlags & SPRITE_COLLISION_BELOW) && (left < xPosition) && (xPosition < right)) {
        if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
            pose = SPOSE_PUSHED_RIGHT_INIT;
        } else {
            pose = SPOSE_PUSHED_LEFT_INIT;
        }
    } else if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
        if (gSpriteData[slot].warioCollision != 0x54) {
            if (gSpriteData[slot].status & SPRITE_STATUS_FACING_RIGHT) {
                pose = SPOSE_PUSHED_RIGHT_INIT;
            } else {
                pose = SPOSE_27;
                if (gTimerState != 0xB) {
                    WarioRequestPose(6);  // TODO: Unknown reaction
                }
            }
        } else {
            pose = SPOSE_PUSHED_RIGHT_INIT;
        }
    } else if (gSpriteCollisionFlags & SPRITE_COLLISION_RIGHT) {
        if (gSpriteData[slot].warioCollision == 0x54) {
            pose = SPOSE_PUSHED_LEFT_INIT;
        } else {
            if (!(gSpriteData[slot].status & SPRITE_STATUS_FACING_RIGHT)) {
                pose = SPOSE_PUSHED_LEFT_INIT;
            } else {
                pose = SPOSE_29;
                if (gTimerState != 0xB) {
                    WarioRequestPose(6);  // TODO: Unknown reaction
                }
            }
        }
    } else {
        pose = gSpriteData[slot].pose;
    }
    gSpriteData[slot].pose = pose;
}

void func_8020B10(s32 slot, u16 direction)
{
    u8 pose;

    pose = func_801EFD4();
    if (pose == 0) {
        if (gSpriteCollisionFlags & SPRITE_COLLISION_ABOVE) {
            switch (gWarioData.pose) {
                case WPOSE_NORMAL_SUPER_GROUND_POUND:
                    m4aSongNumStart(SOUND_2B);
                case WPOSE_NORMAL_GROUND_POUND:
                    pose = SPOSE_CRUSHED_OR_COLLECTED_INIT;
                    SpriteCollisionLandOnEnemy();
                    break;

                default:
                    pose = func_801F00C(slot, direction, TRUE);
                    break;
            }
        } else if (gSpriteCollisionFlags & SPRITE_COLLISION_BELOW) {
            pose = func_801F00C(slot, direction, FALSE);
        } else if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
            pose = func_801F14C(slot, TRUE);
        } else if (gSpriteCollisionFlags & SPRITE_COLLISION_RIGHT) {
            pose = func_801F14C(slot, FALSE);
        } else {
            pose = gSpriteData[slot].pose;
        }
    }
    gSpriteData[slot].pose = pose;
}

void func_8020BB8(s32 slot, u16 direction)
{
    u8 pose;

    pose = func_801EFD4();
    if (pose == 0) {
        if (gSpriteCollisionFlags & SPRITE_COLLISION_ABOVE) {
            switch (gWarioData.pose) {
                case WPOSE_NORMAL_SUPER_GROUND_POUND:
                    m4aSongNumStart(SOUND_2B);
                case WPOSE_NORMAL_GROUND_POUND:
                    pose = SPOSE_CRUSHED_OR_COLLECTED_INIT;
                    SpriteCollisionLandOnEnemy();
                    break;

                default:
                    pose = func_801F00C(slot, direction, TRUE);
                    break;
            }
        } else if (gSpriteCollisionFlags & SPRITE_COLLISION_BELOW) {
            pose = func_801F00C(slot, direction, FALSE);
        } else if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
            if (direction & DPAD_RIGHT) {
                pose = func_801F14C(slot, TRUE);
            } else {
                pose = SPOSE_33;
            }
        } else if (gSpriteCollisionFlags & SPRITE_COLLISION_RIGHT) {
            if (direction & DPAD_LEFT) {
                pose = func_801F14C(slot, FALSE);
            } else {
                pose = SPOSE_35;
            }
        } else {
            pose = gSpriteData[slot].pose;
        }
    }
    gSpriteData[slot].pose = pose;
}

void func_8020C78(s32 slot, u16 direction)
{
    u8 pose;

    if (gSpriteCollisionFlags & SPRITE_COLLISION_ABOVE) {
        gSpriteData[slot].warioCollision = 1;
        switch (gWarioData.pose) {
            case WPOSE_NORMAL_SUPER_GROUND_POUND:
                m4aSongNumStart(SOUND_2B);
            case WPOSE_NORMAL_GROUND_POUND:
                pose = SPOSE_CRUSHED_OR_COLLECTED_INIT;
                SpriteCollisionLandOnEnemy();
                break;

            default:
                pose = func_801F00C(slot, direction, TRUE);
                break;
        }
    } else {
        SpriteCollisionTransformWarioMask();
        pose = SPOSE_13;
    }
    gSpriteData[slot].pose = pose;
}

void func_8020CEC(s32 slot, u16 direction, u16 left, u16 right, u16 xPosition)
{
    u16 facingRight;
    u8 pose;

    facingRight = gSpriteData[slot].status & SPRITE_STATUS_FACING_RIGHT;
    pose = func_801EFD4();
    if (pose == 0) {
        if ((gSpriteCollisionFlags & SPRITE_COLLISION_ABOVE) && (left < xPosition) && (xPosition < right)) {
            switch (gWarioData.pose) {
                case WPOSE_NORMAL_SUPER_GROUND_POUND:
                    m4aSongNumStart(SOUND_2B);
                case WPOSE_NORMAL_GROUND_POUND:
                    pose = SPOSE_CRUSHED_OR_COLLECTED_INIT;
                    SpriteCollisionLandOnEnemy();
                    break;

                default:
                    pose = func_801F00C(slot, direction, TRUE);
                    break;
            }
        } else if ((gSpriteCollisionFlags & SPRITE_COLLISION_BELOW) && (left < xPosition) && (xPosition < right)) {
            pose = func_801F00C(slot, direction, FALSE);
        } else if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
            if (!facingRight) {
                if (gWarioData.damageTimer) {
                    pose = SPOSE_PUSHED_RIGHT_INIT;
                } else {
                    pose = SPOSE_27;
                    SpriteCollisionTakeDamageRight();
                    SpriteCollisionMakeWarioDropCoins(slot);
                }
            } else {
                pose = func_801F14C(slot, TRUE);
            }
        } else if (gSpriteCollisionFlags & SPRITE_COLLISION_RIGHT) {
            if (facingRight != 0) {
                if (gWarioData.damageTimer) {
                    pose = SPOSE_PUSHED_LEFT_INIT;
                } else {
                    pose = SPOSE_29;
                    SpriteCollisionTakeDamageLeft();
                    SpriteCollisionMakeWarioDropCoins(slot);
                }
            } else {
                pose = func_801F14C(slot, FALSE);
            }
        } else {
            pose = gSpriteData[slot].pose;
        }
    }
    gSpriteData[slot].pose = pose;
}

void func_8020E1C(s32 slot, u16 direction)
{
    u16 facingRight;
    u8 pose;

    facingRight = gSpriteData[slot].status & SPRITE_STATUS_FACING_RIGHT;
    pose = func_801EFD4();
    if (pose == 0) {
        if (gSpriteCollisionFlags & SPRITE_COLLISION_ABOVE) {
            switch (gWarioData.pose) {
                case WPOSE_NORMAL_SUPER_GROUND_POUND:
                    m4aSongNumStart(SOUND_2B);
                case WPOSE_NORMAL_GROUND_POUND:
                    pose = SPOSE_CRUSHED_OR_COLLECTED_INIT;
                    SpriteCollisionLandOnEnemy();
                    break;

                default:
                    pose = func_801F00C(slot, direction, TRUE);
                    break;
            }
        } else {
            if (gSpriteCollisionFlags & SPRITE_COLLISION_BELOW) {
                pose = func_801F00C(slot, direction, FALSE);
            } else if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
                if (facingRight == 0) {
                    if (gWarioData.damageTimer) {
                        pose = SPOSE_PUSHED_RIGHT_INIT;
                    } else {
                        pose = SPOSE_27;
                        SpriteCollisionTakeDamageRight();
                        SpriteCollisionMakeWarioDropCoins(slot);
                    }
                } else {
                    pose = func_801F14C(slot, TRUE);
                }
            } else if (gSpriteCollisionFlags & SPRITE_COLLISION_RIGHT) {
                if (facingRight != 0) {
                    if (gWarioData.damageTimer) {
                        pose = SPOSE_PUSHED_LEFT_INIT;
                    } else {
                        pose = SPOSE_29;
                        SpriteCollisionTakeDamageLeft();
                        SpriteCollisionMakeWarioDropCoins(slot);
                    }
                } else {
                    pose = func_801F14C(slot, FALSE);
                }
            } else {
                pose = gSpriteData[slot].pose;
            }
        }
    }
    gSpriteData[slot].pose = pose;
}

void func_8020F28(s32 slot, u16 direction)
{
    u16 facingRight;
    u8 pose;

    facingRight = gSpriteData[slot].status & SPRITE_STATUS_FACING_RIGHT;
    pose = func_801EFD4();
    if (pose == 0) {
        if (gSpriteCollisionFlags & SPRITE_COLLISION_ABOVE) {
            if (!facingRight) {
                if (gWarioData.damageTimer) {
                    pose = SPOSE_PUSHED_RIGHT_INIT;
                } else {
                    pose = SPOSE_27;
                    SpriteCollisionTakeDamageRight();
                    SpriteCollisionMakeWarioDropCoins(slot);
                }
            } else {
                if (gWarioData.damageTimer) {
                    pose = SPOSE_PUSHED_LEFT_INIT;
                } else {
                    pose = SPOSE_29;
                    SpriteCollisionTakeDamageLeft();
                    SpriteCollisionMakeWarioDropCoins(slot);
                }
            }
        } else if (gSpriteCollisionFlags & SPRITE_COLLISION_BELOW) {
            pose = func_801F00C(slot, direction, FALSE);
        } else if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
            pose = func_801F14C(slot, TRUE);
        } else if (gSpriteCollisionFlags & SPRITE_COLLISION_RIGHT) {
            pose = func_801F14C(slot, FALSE);
        } else {
            pose = gSpriteData[slot].pose;
        }
    }
    gSpriteData[slot].pose = pose;
}

void func_8020FF4(s32 slot, u16 direction)
{
    u16 facingRight;
    u8 pose;

    facingRight = gSpriteData[slot].status & SPRITE_STATUS_FACING_RIGHT;
    pose = func_801EFD4();
    if (pose == 0) {
        if (gSpriteCollisionFlags & SPRITE_COLLISION_ABOVE) {
            if (!facingRight) {
                if (gWarioData.damageTimer) {
                    pose = SPOSE_PUSHED_RIGHT_INIT;
                } else {
                    pose = SPOSE_27;
                    SpriteCollisionTakeDamageRight();
                    SpriteCollisionMakeWarioDropCoins(slot);
                }
            } else {
                if (gWarioData.damageTimer) {
                    pose = SPOSE_PUSHED_LEFT_INIT;
                } else {
                    pose = SPOSE_29;
                    SpriteCollisionTakeDamageLeft();
                    SpriteCollisionMakeWarioDropCoins(slot);
                }
            }
        } else if (gSpriteCollisionFlags & SPRITE_COLLISION_BELOW) {
            pose = func_801F00C(slot, direction, FALSE);
        } else if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
            if (!facingRight) {
                if (gWarioData.damageTimer) {
                    pose = SPOSE_PUSHED_RIGHT_INIT;
                } else {
                    pose = SPOSE_27;
                    SpriteCollisionTakeDamageRight();
                    SpriteCollisionMakeWarioDropCoins(slot);
                }
            } else {
                pose = func_801F14C(slot, TRUE);
            }
        } else if (gSpriteCollisionFlags & SPRITE_COLLISION_RIGHT) {
            if (facingRight) {
                if (gWarioData.damageTimer) {
                    pose = SPOSE_PUSHED_LEFT_INIT;
                } else {
                    pose = SPOSE_29;
                    SpriteCollisionTakeDamageLeft();
                    SpriteCollisionMakeWarioDropCoins(slot);
                }
            } else {
                pose = func_801F14C(slot, FALSE);
            }
        } else {
            pose = gSpriteData[slot].pose;
        }
    }
    gSpriteData[slot].pose = pose;
}

void func_80210E8(s32 slot, u16 direction)
{
    u16 facingRight;
    u8 pose;

    facingRight = gSpriteData[slot].status & SPRITE_STATUS_FACING_RIGHT;
    pose = func_801EFD4();
    if (pose == 0) {
        if (gSpriteCollisionFlags & SPRITE_COLLISION_ABOVE) {
            if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
                if (gWarioData.damageTimer) {
                    pose = SPOSE_PUSHED_RIGHT_INIT;
                } else {
                    pose = SPOSE_27;
                    SpriteCollisionTakeDamageRight();
                    SpriteCollisionMakeWarioDropCoins(slot);
                }
            } else {
                if (gWarioData.damageTimer) {
                    pose = SPOSE_PUSHED_LEFT_INIT;
                } else {
                    pose = SPOSE_29;
                    SpriteCollisionTakeDamageLeft();
                    SpriteCollisionMakeWarioDropCoins(slot);
                }
            }
        } else if (gSpriteCollisionFlags & SPRITE_COLLISION_BELOW) {
            pose = func_801F00C(slot, direction, FALSE);
        } else if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
            if (facingRight) {
                if (gWarioData.damageTimer) {
                    pose = SPOSE_PUSHED_RIGHT_INIT;
                } else {
                    pose = SPOSE_27;
                    SpriteCollisionTakeDamageRight();
                    SpriteCollisionMakeWarioDropCoins(slot);
                }
            } else {
                pose = func_801F14C(slot, TRUE);
            }
        } else if (gSpriteCollisionFlags & SPRITE_COLLISION_RIGHT) {
            if (!facingRight) {
                if (gWarioData.damageTimer) {
                    pose = SPOSE_PUSHED_LEFT_INIT;
                } else {
                    pose = SPOSE_29;
                    SpriteCollisionTakeDamageLeft();
                    SpriteCollisionMakeWarioDropCoins(slot);
                }
            } else {
                pose = func_801F14C(slot, FALSE);
            }
        } else {
            pose = gSpriteData[slot].pose;
        }
    }
    gSpriteData[slot].pose = pose;
}

void func_80211E0(s32 slot)
{
    u8 pose;

    pose = func_801EFD4();
    if (pose == 0) {
        if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
            if (gWarioData.damageTimer) {
                pose = SPOSE_PUSHED_RIGHT_INIT;
            } else {
                pose = SPOSE_6E;
                SpriteCollisionTakeDamageRight();
                SpriteCollisionMakeWarioDropCoins(slot);
            }
        } else if (gSpriteCollisionFlags & SPRITE_COLLISION_RIGHT) {
            if (gWarioData.damageTimer) {
                pose = SPOSE_PUSHED_LEFT_INIT;
            } else {
                pose = SPOSE_70;
                SpriteCollisionTakeDamageLeft();
                SpriteCollisionMakeWarioDropCoins(slot);
            }
        } else {
            pose = gSpriteData[slot].pose;
        }
    }
    gSpriteData[slot].pose = pose;
}

void func_802125C(s32 slot, u16 direction)
{
    u8 pose;

    pose = func_801EFD4();
    if (pose == 0) {
        if (gSpriteCollisionFlags & SPRITE_COLLISION_ABOVE) {
            switch (gWarioData.pose) {
                case WPOSE_NORMAL_SUPER_GROUND_POUND:
                    m4aSongNumStart(SOUND_2B);
                case WPOSE_NORMAL_GROUND_POUND:
                    pose = SPOSE_CRUSHED_OR_COLLECTED_INIT;
                    SpriteCollisionLandOnEnemy();
                    break;

                default:
                    pose = func_801F00C(slot, direction, TRUE);
                    break;
            }
        } else if (gSpriteCollisionFlags & SPRITE_COLLISION_BELOW) {
            pose = func_801F200(slot, direction);
        } else if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
            if (direction & DPAD_RIGHT) {
                pose = func_801F43C(slot, TRUE);
            } else {
                pose = SPOSE_33;
            }
        } else if (gSpriteCollisionFlags & SPRITE_COLLISION_RIGHT) {
            if (direction & DPAD_LEFT) {
                pose = func_801F43C(slot, FALSE);
            } else {
                pose = SPOSE_35;
            }
        } else {
            pose = gSpriteData[slot].pose;
        }
    }
    gSpriteData[slot].pose = pose;
}

void func_8021318(s32 slot, u16 direction)
{
    u8 pose;

    pose = func_801EFD4();
    if (pose == 0) {
        if (gSpriteCollisionFlags & SPRITE_COLLISION_ABOVE) {
            if (!(gSpriteData[slot].status & SPRITE_STATUS_FACING_RIGHT)) {
                if (gWarioData.damageTimer) {
                    pose = SPOSE_PUSHED_RIGHT_INIT;
                } else {
                    pose = SPOSE_27;
                    SpriteCollisionTakeDamageRight();
                    SpriteCollisionMakeWarioDropCoins(slot);
                }
            } else {
                if (gWarioData.damageTimer) {
                    pose = SPOSE_PUSHED_LEFT_INIT;
                } else {
                    pose = SPOSE_29;
                    SpriteCollisionTakeDamageLeft();
                    SpriteCollisionMakeWarioDropCoins(slot);
                }
            }
        } else if (gSpriteCollisionFlags & SPRITE_COLLISION_BELOW) {
            pose = func_801F200(slot, direction);
        } else if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
            if (direction & DPAD_RIGHT) {
                pose = func_801F43C(slot, TRUE);
            } else {
                pose = SPOSE_33;
            }
        } else if (gSpriteCollisionFlags & SPRITE_COLLISION_RIGHT) {
            if (direction & DPAD_LEFT) {
                pose = func_801F43C(slot, FALSE);
            } else {
                pose = SPOSE_35;
            }
        } else {
            pose = gSpriteData[slot].pose;
        }
    }
    gSpriteData[slot].pose = pose;
}

void func_80213F4(s32 slot, u16 currentX, u16 previousX)
{
    if (currentX > previousX) {
        SpriteCollisionTakeDamageRight();
    } else if (currentX < previousX) {
        SpriteCollisionTakeDamageLeft();
    } else if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
        SpriteCollisionTakeDamageRight();
    } else {
        SpriteCollisionTakeDamageLeft();
    }
    SpriteCollisionMakeWarioDropCoins(slot);
}

void func_8021434(s32 slot, u16 left, u16 right)
{
    if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
        if ((gSpriteData[slot].status & SPRITE_STATUS_FACING_RIGHT) || (gSpriteData[slot].warioCollision == 0x3A)) {
            switch (gWarioData.pose) {
                case WPOSE_NORMAL_STARTING_ROLL:
                case WPOSE_NORMAL_JUMPING_OUT_OF_ROLL:
                case WPOSE_NORMAL_ROLLING:
                case WPOSE_NORMAL_ROLLING_JUMP:
                    gWarioData.xPosition = left - gWarioData.hitboxOffsetRight;
                    WarioRequestPose(WPOSE_NORMAL_ROLLING_BONK);
                    break;

                default:
                    func_801EF50();
            }
        } else {
            SpriteCollisionTakeDamageRight();
            SpriteCollisionMakeWarioDropCoins(slot);
        }
    } else {
        if (!(gSpriteData[slot].status & SPRITE_STATUS_FACING_RIGHT) || (gSpriteData[slot].warioCollision == 0x3A)) {
            switch (gWarioData.pose) {
                case WPOSE_NORMAL_STARTING_ROLL:
                case WPOSE_NORMAL_JUMPING_OUT_OF_ROLL:
                case WPOSE_NORMAL_ROLLING:
                case WPOSE_NORMAL_ROLLING_JUMP:
                    gWarioData.xPosition = right - gWarioData.hitboxOffsetLeft + 1;
                    WarioRequestPose(WPOSE_NORMAL_ROLLING_BONK);
                    break;

                default:
                    func_801EF94();
            }
        } else {
            SpriteCollisionTakeDamageLeft();
            SpriteCollisionMakeWarioDropCoins(slot);
        }
    }
}

void func_8021500(s32 slot)
{
    if ((gWarioData.reaction == REACTION_NORMAL) && (gTimerState != 0xB)) {
        if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
            if (gWarioData.damageTimer == 0) {
                SpriteCollisionTakeDamageRight();
                gCuckooCondorPendulumLength = 0x3C;
            }
        } else if (gSpriteCollisionFlags & SPRITE_COLLISION_RIGHT) {
            if (gWarioData.damageTimer == 0) {
                SpriteCollisionTakeDamageLeft();
                gCuckooCondorPendulumLength = 0x3C;
            }
        }
        gCuckooCondorHasCapturedWario = 0;
    }
}

void func_8021564(s32 slot)
{
    if ((gWarioData.reaction == REACTION_NORMAL) && (func_801EFD4() == 0)) {
        if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
            if (gWarioData.damageTimer == 0) {
                SpriteCollisionTakeDamageRight();
                SpriteCollisionMakeWarioDropCoins(slot);
            }
        } else if ((gSpriteCollisionFlags & SPRITE_COLLISION_RIGHT)) {
            if (gWarioData.damageTimer == 0) {
                SpriteCollisionTakeDamageLeft();
                SpriteCollisionMakeWarioDropCoins(slot);
            }
        }
    }
}

void func_80215C0(void)
{
    if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
        SpriteCollisionTakeDamageRight();
    } else if (gSpriteCollisionFlags & SPRITE_COLLISION_RIGHT) {
        SpriteCollisionTakeDamageLeft();
    }
}

void func_80215E8(s32 slot)
{
    u8 pose;

    pose = func_801EFD4();
    if (pose == 0) {
        if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
            if (gWarioData.damageTimer == 0) {
                SpriteCollisionTakeDamageRight();
                SpriteCollisionMakeWarioDropCoins(slot);
            }
            pose = SPOSE_CRUSHED_OR_COLLECTED_INIT;
        } else if (gSpriteCollisionFlags & SPRITE_COLLISION_RIGHT) {
            if (gWarioData.damageTimer == 0) {
                SpriteCollisionTakeDamageLeft();
                SpriteCollisionMakeWarioDropCoins(slot);
            }
            pose = SPOSE_CRUSHED_OR_COLLECTED_INIT;
        }
    }
    gSpriteData[slot].pose = pose;
}

void func_8021650(s32 slot)
{
    u8 pose;

    pose = func_801EFD4();
    if (pose == 0) {
        if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
            if (gWarioData.damageTimer == 0) {
                SpriteCollisionTakeDamageRight();
                SpriteCollisionMakeWarioDropCoins(slot);
            }
        } else if (gSpriteCollisionFlags & SPRITE_COLLISION_RIGHT) {
            if (gWarioData.damageTimer == 0) {
                SpriteCollisionTakeDamageLeft();
                SpriteCollisionMakeWarioDropCoins(slot);
            }
        }
    } else {
        gSpriteData[slot].pose = pose;
    }
}

void func_80216B8(s32 slot)
{
    u8 pose;

    pose = func_801EFD4();
    if (pose == 0) {
        if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
            if (gWarioData.damageTimer == 0) {
                SpriteCollisionTakeDamageRight();
                SpriteCollisionMakeWarioDropCoins(slot);
            }
            pose = SPOSE_6C;
        } else if (gSpriteCollisionFlags & SPRITE_COLLISION_RIGHT) {
            if (gWarioData.damageTimer == 0) {
                SpriteCollisionTakeDamageLeft();
                SpriteCollisionMakeWarioDropCoins(slot);
            }
            pose = SPOSE_6C;
        }
    }
    gSpriteData[slot].pose = pose;
}

void func_8021720(s32 slot)
{
    gSpriteData[slot].pose = SPOSE_CRUSHED_OR_COLLECTED_INIT;
}

void func_8021734(s32 slot)
{
    gSpriteData[slot].pose = SPOSE_CRUSHED_OR_COLLECTED_INIT;
    if ((gWarioData.reaction == REACTION_NORMAL) && (gWarioData.damageTimer == 0)) {
        if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
            func_801EF50();
        } else {
            func_801EF94();
        }
        gWarioData.yVelocity = 0x40;
    }
}

void func_8021784(
    s32 slot,
    u16 direction,
    u16 spriteTop,
    u16 spriteBottom,
    u16 spriteLeft,
    u16 spriteRight,
    u16 warioLeft,
    u16 warioRight
)
{
    u8 pose;

    if (gSpriteCollisionFlags & SPRITE_COLLISION_ABOVE) {
        if ((gWarioData.reaction != REACTION_NORMAL)) {
            if (gWarioData.reaction == REACTION_PUFFY) {
                gSpriteCollisionFlags &= ~1;
            } else if (spriteLeft + 5 > warioRight || spriteRight - 5 < warioLeft) {
                gSpriteCollisionFlags &= ~1;
            }
        }
        if (gPreviousYPosition > gWarioData.yPosition) {
            gSpriteCollisionFlags &= ~1;
        }
    } else if (
        gSpriteCollisionFlags & SPRITE_COLLISION_BELOW && gSpriteData[slot].warioCollision != 0x16 &&
        gPreviousYPosition < gWarioData.yPosition
    ) {
        gSpriteCollisionFlags &= ~2;
        return;
    }

    pose = gSpriteData[slot].pose;
    if (gSpriteCollisionFlags & SPRITE_COLLISION_ABOVE) {
        gWarioData.yPosition = spriteTop + 1;
        if (gWarioData.reaction == REACTION_NORMAL) {
            switch (gWarioData.pose) {
                case WPOSE_NORMAL_SUPER_GROUND_POUND:
                    m4aSongNumStart(SOUND_2B);
                case WPOSE_NORMAL_GROUND_POUND:
                    pose = SPOSE_CRUSHED_OR_COLLECTED_INIT;
                    SpriteCollisionLandOnEnemy();
                    break;

                default:
                    gSpriteData[slot].status |= SPRITE_STATUS_WARIO_STANDING_ON;
                    gSpriteData[slot].work1 = 2;
                    break;
            }
        } else {
            gSpriteData[slot].status |= SPRITE_STATUS_WARIO_STANDING_ON;
            gSpriteData[slot].work1 = 2;
        }
    } else if (gSpriteCollisionFlags & SPRITE_COLLISION_BELOW) {
        if (gSpriteData[slot].warioCollision == 0x16) {
            gSpriteData[slot].yPosition = gWarioData.yPosition + gWarioData.hitboxOffsetTop;
            if ((gWarioData.reaction == REACTION_NORMAL) && (gWarioData.pose != WPOSE_NORMAL_DASH_ATTACK)) {
                gSpriteData[slot].work3 = U8_MAX;
                if (gWarioData.hitboxOffsetTop < -0x50) {
                    WarioRequestPose(WPOSE_NORMAL_UNKNOWN_52);
                }
            }
            gIgnoreSpriteCollision = TRUE;
            return;
        }

        gSpriteCollisionTileType = 0;
        func_8023BFC(spriteBottom + 0x64, gSpriteData[slot].xPosition);
        if (gSpriteCollisionTileType == 0) {
            gWarioData.yPosition = spriteBottom + 1 - gWarioData.hitboxOffsetTop;
        }
        if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
            pose = SPOSE_33;
        } else {
            pose = SPOSE_35;
        }
        if (gWarioData.unk_1A && gWarioData.yVelocity > 0) {
            gWarioData.yVelocity = 0;
        }
    } else if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
        gWarioData.xPosition = spriteLeft - gWarioData.hitboxOffsetRight;
        gWarioData.xVelocity = 0;
        switch (gWarioData.reaction) {
            case REACTION_NORMAL:
                if (gSpriteData[slot].warioCollision == 0x17 || gSpriteData[slot].warioCollision == 0x18) {
                    if (direction & DPAD_RIGHT) {
                        func_801EF50();
                    }
                } else {
                    if (direction & DPAD_RIGHT) {
                        switch (gWarioData.pose) {
                            case WPOSE_NORMAL_SHOULDER_BASH:
                            case WPOSE_NORMAL_DASH_ATTACK:
                                gWarioData.unk_08 = 1;
                                WarioRequestPose(WPOSE_NORMAL_SHOULDER_BASH_BONK);
                                pose = SPOSE_TACKLED_RIGHT_INIT;
                                break;

                            case WPOSE_NORMAL_SHOULDER_BASH_JUMP:
                            case WPOSE_NORMAL_DASH_ATTACK_JUMP:
                                WarioRequestPose(WPOSE_NORMAL_SHOULDER_BASH_BONK);
                                pose = SPOSE_TACKLED_RIGHT_INIT;
                                break;

                            case WPOSE_NORMAL_STARTING_ROLL:
                            case WPOSE_NORMAL_JUMPING_OUT_OF_ROLL:
                            case WPOSE_NORMAL_ROLLING:
                            case WPOSE_NORMAL_ROLLING_JUMP:
                                gWarioData.xPosition = spriteLeft - gWarioData.hitboxOffsetRight;
                                WarioRequestPose(WPOSE_NORMAL_ROLLING_BONK);

                            default:
                                pose = SPOSE_33;
                                break;
                        }
                    }
                }
                break;

            case REACTION_FLAMING:
                if ((direction & DPAD_RIGHT) && gWarioData.pose < WPOSE_FLAMING_ENGULFED) {
                    gWarioData.horizontalDirection ^= (DPAD_LEFT | DPAD_RIGHT);
                    WarioRequestPose(gWarioData.pose + 2);
                }
                break;

            case REACTION_SNOWMAN:
                if (gWarioData.pose >= WPOSE_SNOWMAN_ROLLING_SMALL) {
                    WarioRequestPose(WPOSE_SNOWMAN_BONK);
                }
                break;

            case REACTION_FROZEN:
                WarioRequestPose(WPOSE_FROZEN_BONK);
                break;
        }
    } else if (gSpriteCollisionFlags & SPRITE_COLLISION_RIGHT) {
        gWarioData.xPosition = (spriteRight - gWarioData.hitboxOffsetLeft) + 1;
        gWarioData.xVelocity = 0;
        switch (gWarioData.reaction) {
            case REACTION_NORMAL:
                if (gSpriteData[slot].warioCollision == 0x17 || gSpriteData[slot].warioCollision == 0x18) {
                    if (direction & DPAD_LEFT) {
                        func_801EF94();
                    }
                } else {
                    if (direction & DPAD_LEFT) {
                        switch (gWarioData.pose) {
                            case WPOSE_NORMAL_SHOULDER_BASH:
                            case WPOSE_NORMAL_DASH_ATTACK:
                                gWarioData.unk_08 = 1;
                                WarioRequestPose(WPOSE_NORMAL_SHOULDER_BASH_BONK);
                                pose = SPOSE_TACKLED_LEFT_INIT;
                                break;

                            case WPOSE_NORMAL_SHOULDER_BASH_JUMP:
                            case WPOSE_NORMAL_DASH_ATTACK_JUMP:
                                WarioRequestPose(WPOSE_NORMAL_SHOULDER_BASH_BONK);
                                pose = SPOSE_TACKLED_LEFT_INIT;
                                break;

                            case WPOSE_NORMAL_STARTING_ROLL:
                            case WPOSE_NORMAL_JUMPING_OUT_OF_ROLL:
                            case WPOSE_NORMAL_ROLLING:
                            case WPOSE_NORMAL_ROLLING_JUMP:
                                gWarioData.xPosition = (spriteRight - gWarioData.hitboxOffsetLeft) + 1;
                                WarioRequestPose(WPOSE_NORMAL_ROLLING_BONK);

                            default:
                                pose = SPOSE_35;
                                break;
                        }
                    }
                }
                break;

            case REACTION_FLAMING:
                if ((direction & DPAD_LEFT) && gWarioData.pose < WPOSE_FLAMING_ENGULFED) {
                    gWarioData.horizontalDirection ^= (DPAD_LEFT | DPAD_RIGHT);
                    WarioRequestPose(gWarioData.pose + 2);
                }
                break;

            case REACTION_FROZEN:
                WarioRequestPose(WPOSE_FROZEN_BONK);
                break;

            case REACTION_SNOWMAN:
                if (gWarioData.pose >= WPOSE_SNOWMAN_ROLLING_SMALL) {
                    WarioRequestPose(WPOSE_SNOWMAN_BONK);
                }
                break;
        }
    }

    if ((gSpriteData[slot].warioCollision == 0x18) || (gSpriteData[slot].warioCollision == 0x1A)) {
        gSpriteData[slot].pose = pose;
    }
}

void func_8021C30(s32 slot, u16 top)
{
    if (gWarioData.reaction == REACTION_PUFFY) {
        gSpriteCollisionFlags &= ~1;
    }

    if (gSpriteCollisionFlags & SPRITE_COLLISION_ABOVE) {
        gWarioData.yPosition = top + 1;
        if (gWarioData.reaction == REACTION_NORMAL) {
            switch (gWarioData.pose) {
                case WPOSE_NORMAL_SUPER_GROUND_POUND:
                    m4aSongNumStart(SOUND_2B);
                case WPOSE_NORMAL_GROUND_POUND:
                    gSpriteData[slot].pose = SPOSE_CRUSHED_OR_COLLECTED_INIT;
                    SpriteCollisionLandOnEnemy();
                    if (!CHECK_KEYS_ALL(gButtonsHeld, A_BUTTON)) {
                        gWarioData.yVelocity = 0x20;
                    }
                    break;

                default:
                    gSpriteData[slot].status |= SPRITE_STATUS_WARIO_STANDING_ON;
                    gSpriteData[slot].work1 = 2;
                    break;
            }
        } else {
            gSpriteData[slot].status |= SPRITE_STATUS_WARIO_STANDING_ON;
            gSpriteData[slot].work1 = 2;
        }
    }
}

void func_8021CC8(s32 slot, u16 top)
{
    if (gWarioData.reaction == REACTION_PUFFY) {
        gSpriteCollisionFlags &= ~1;
    }

    if (gSpriteCollisionFlags & SPRITE_COLLISION_ABOVE) {
        gWarioData.yPosition = top + 1;
        if (gWarioData.reaction == REACTION_NORMAL) {
            switch (gWarioData.pose) {
                case WPOSE_NORMAL_SUPER_GROUND_POUND:
                    m4aSongNumStart(SOUND_2B);
                case WPOSE_NORMAL_GROUND_POUND:
                    SpriteCollisionLandOnEnemy();
                    if (!CHECK_KEYS_ALL(gButtonsHeld, A_BUTTON)) {
                        gWarioData.yVelocity = 0x20;
                    }
                    break;

                default:
                    gSpriteData[slot].status |= SPRITE_STATUS_WARIO_STANDING_ON;
                    gSpriteData[slot].work1 = 2;
                    break;
            }
        } else {
            gSpriteData[slot].status |= SPRITE_STATUS_WARIO_STANDING_ON;
            gSpriteData[slot].work1 = 2;
        }
        gSpriteData[slot].pose = SPOSE_CRUSHED_OR_COLLECTED_INIT;
    }
}

void func_8021D5C(s32 slot, u16 top)
{
    if (gWarioData.reaction == REACTION_PUFFY) {
        gSpriteCollisionFlags &= ~1;
    }

    if (gSpriteCollisionFlags & SPRITE_COLLISION_ABOVE) {
        gWarioData.yPosition = top + 1;
        if (gWarioData.reaction == REACTION_NORMAL) {
            switch (gWarioData.pose) {
                case WPOSE_NORMAL_SUPER_GROUND_POUND:
                    m4aSongNumStart(SOUND_2B);
                case WPOSE_NORMAL_GROUND_POUND:
                    SpriteCollisionLandOnEnemy();
                    break;
                default:
                    gSpriteData[slot].status |= SPRITE_STATUS_WARIO_STANDING_ON;
                    gSpriteData[slot].work1 = 2;
                    break;
            }
        } else {
            gSpriteData[slot].status |= SPRITE_STATUS_WARIO_STANDING_ON;
            gSpriteData[slot].work1 = 2;
        }
    }
}

void func_8021DD0(s32 slot, u16 top)
{
    if (gWarioData.reaction == REACTION_PUFFY) {
        gSpriteCollisionFlags &= ~1;
    } else if (gWarioData.yVelocity >= 0) {
        gSpriteCollisionFlags &= ~1;
    }

    if (gSpriteCollisionFlags & SPRITE_COLLISION_ABOVE) {
        gWarioData.yPosition = top + 1;
        if (gWarioData.reaction == REACTION_NORMAL) {
            switch (gWarioData.pose) {
                case WPOSE_NORMAL_SUPER_GROUND_POUND:
                    m4aSongNumStart(SOUND_2B);
                case WPOSE_NORMAL_GROUND_POUND:
                    SpriteCollisionLandOnEnemy();
                    break;
                default:
                    gSpriteData[slot].status |= SPRITE_STATUS_WARIO_STANDING_ON;
                    gSpriteData[slot].work1 = 2;
                    break;
            }
        } else {
            gSpriteData[slot].status |= SPRITE_STATUS_WARIO_STANDING_ON;
            gSpriteData[slot].work1 = 2;
        }
        gSpriteData[slot].pose = SPOSE_11;
    }
}

void func_8021E6C(s32 slot, u16 bottom, u16 left, u16 right)
{
    if (gSpriteCollisionFlags & SPRITE_COLLISION_ABOVE) {
        if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
            func_801EF50();
        } else {
            func_801EF94();
        }
        gWarioData.yVelocity = 0x40;
        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
    } else if (gSpriteCollisionFlags & SPRITE_COLLISION_BELOW) {
        gSpriteCollisionTileType = 0;
        func_8023BFC(bottom + 0x64, gSpriteData[slot].xPosition);
        if (gSpriteCollisionTileType == 0) {
            gWarioData.yPosition = bottom + 1 - gWarioData.hitboxOffsetTop;
        }
        WarioRequestPose(WPOSE_NORMAL_LANDING_ON_ENEMY);
        if (gWarioData.unk_1A != 0) {
            gWarioData.yVelocity = 0;
        }
        if (gWarioData.pose == WPOSE_NORMAL_LANDING_ON_ENEMY) {
            if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
                gWarioData.horizontalDirection = DPAD_RIGHT;
                gWarioData.xVelocity = -0x10;
            } else {
                gWarioData.horizontalDirection = DPAD_LEFT;
                gWarioData.xVelocity = 0x10;
            }
        }
        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
    } else if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
        gWarioData.xPosition = left - gWarioData.hitboxOffsetRight;
        func_801EF50();
        gWarioData.yVelocity = 0x40;
        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
    } else if (gSpriteCollisionFlags & SPRITE_COLLISION_RIGHT) {
        gWarioData.xPosition = (right - gWarioData.hitboxOffsetLeft) + 1;
        func_801EF94();
        gWarioData.yVelocity = 0x40;
        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
    }
}

void func_8021F84(s32 slot, u16 left, u16 right, u16 top)
{
    if (gPreviousYPosition > gWarioData.yPosition) {
        gSpriteCollisionFlags &= 0xFE;
    }

    if (gSpriteCollisionFlags & SPRITE_COLLISION_ABOVE) {
        switch (gWarioData.pose) {
            case WPOSE_NORMAL_SUPER_GROUND_POUND:
                m4aSongNumStart(SOUND_2B);
            case WPOSE_NORMAL_GROUND_POUND:
                SpriteCollisionLandOnEnemy();
                if (!(A_BUTTON & gButtonsHeld)) {
                    gWarioData.yVelocity = 0x20;
                }
                gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
                gIgnoreSpriteCollision = 1;
                break;
            default:
                if (1 & gButtonsHeld) {
                    WarioRequestPose(WPOSE_NORMAL_LANDING_ON_ENEMY);
                    gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
                    gIgnoreSpriteCollision = 1;
                    m4aSongNumStart(SOUND_35);
                } else {
                    gWarioData.yPosition = top + 1;
                    gSpriteData[slot].status |= SPRITE_STATUS_WARIO_STANDING_ON;
                    gSpriteData[slot].work1 = 2;
                }
                break;
        }
    } else if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
        GetSpriteBlockCollisionAtPosition(gWarioData.yPosition, gWarioData.xPosition);
        if (gBackgroundCollisionData.waterType != 1) {
            gWarioData.xPosition = left - gWarioData.hitboxOffsetRight;
            WarioRequestPose(WPOSE_NORMAL_STANDING);
        }
    } else if (gSpriteCollisionFlags & SPRITE_COLLISION_RIGHT) {
        GetSpriteBlockCollisionAtPosition(gWarioData.yPosition, gWarioData.xPosition);
        if (gBackgroundCollisionData.waterType != 1) {
            gWarioData.xPosition = (right - gWarioData.hitboxOffsetLeft) + 1;
            WarioRequestPose(WPOSE_NORMAL_STANDING);
        }
    }
}

void func_80220F8(s32 slot)
{
    if (gWarioData.reaction == REACTION_ZOMBIE) {
        gSpriteData[slot].pose = SPOSE_72;
    }
}

void func_8022118(s32 slot, u16 currentX, u16 previousX)
{
    if (currentX > previousX) {
        if (gWarioData.xVelocity == 0x80) {
            gSpriteData[slot].pose = SPOSE_TACKLED_RIGHT_INIT;
        } else {
            gSpriteData[slot].pose = SPOSE_33;
        }
    } else {
        if (gWarioData.xVelocity == -0x80) {
            gSpriteData[slot].pose = SPOSE_TACKLED_LEFT_INIT;
        } else {
            gSpriteData[slot].pose = SPOSE_35;
        }
    }
}

void func_8022188(s32 slot, u16 direction)
{
    u8 pose;

    pose = func_801EFD4();
    if (pose == 0) {
        if (gSpriteCollisionFlags & SPRITE_COLLISION_BELOW) {
            pose = func_801F00C(slot, direction, FALSE);
        } else if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
            if (gWarioData.damageTimer) {
                pose = SPOSE_PUSHED_RIGHT_INIT;
            } else {
                pose = SPOSE_27;
                SpriteCollisionTakeDamageRight();
                SpriteCollisionMakeWarioDropCoins(slot);
            }
        } else if (gSpriteCollisionFlags & SPRITE_COLLISION_RIGHT) {
            if (gWarioData.damageTimer) {
                pose = SPOSE_PUSHED_LEFT_INIT;
            } else {
                pose = SPOSE_29;
                SpriteCollisionTakeDamageLeft();
                SpriteCollisionMakeWarioDropCoins(slot);
            }
        } else {
            pose = gSpriteData[slot].pose;
        }
    }
    gSpriteData[slot].pose = pose;
}

void func_8022220(s32 slot, u16 direction)
{
    u8 pose;

    pose = func_801EFD4();
    if (pose == 0) {
        if (gSpriteCollisionFlags & SPRITE_COLLISION_ABOVE) {
            switch (gWarioData.pose) {
                case WPOSE_NORMAL_SUPER_GROUND_POUND:
                    m4aSongNumStart(SOUND_2B);
                case WPOSE_NORMAL_GROUND_POUND:
                    pose = SPOSE_CRUSHED_OR_COLLECTED_INIT;
                    SpriteCollisionLandOnEnemy();
                    break;

                default:
                    pose = func_801F00C(slot, direction, TRUE);
                    break;
            }
        } else if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
            if (gWarioData.damageTimer) {
                pose = SPOSE_PUSHED_RIGHT_INIT;
            } else {
                pose = SPOSE_27;
                SpriteCollisionTakeDamageRight();
                SpriteCollisionMakeWarioDropCoins(slot);
            }
        } else if (gSpriteCollisionFlags & SPRITE_COLLISION_RIGHT) {
            if (gWarioData.damageTimer) {
                pose = SPOSE_PUSHED_LEFT_INIT;
            } else {
                pose = SPOSE_29;
                SpriteCollisionTakeDamageLeft();
                SpriteCollisionMakeWarioDropCoins(slot);
            }
        } else {
            pose = gSpriteData[slot].pose;
        }
    }
    gSpriteData[slot].pose = pose;
}

void func_80222D8(s32 slot, u16 bottom, u16 left, u16 right)
{
    s32 facingRight;

    facingRight = gSpriteData[slot].status & SPRITE_STATUS_FACING_RIGHT;
    if (gSpriteCollisionFlags & SPRITE_COLLISION_ABOVE) {
        if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
            func_801EF50();
            gWarioData.xVelocity = -0x20;
        } else {
            func_801EF94();
            gWarioData.xVelocity = 0x20;
        }
        gWarioData.yVelocity = 0x50;
        return;
    }
    if (gSpriteCollisionFlags & SPRITE_COLLISION_BELOW) {
        gSpriteCollisionTileType = 0;
        func_8023BFC(bottom + 0x64, gSpriteData[slot].xPosition);
        if (gSpriteCollisionTileType == 0) {
            gWarioData.yPosition = bottom + 1 - gWarioData.hitboxOffsetTop;
        }
        if (gWarioData.unk_1A) {
            gWarioData.yVelocity = 0;
        } else {
            WarioRequestPose(WPOSE_NORMAL_LANDING_ON_ENEMY);
            if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
                gWarioData.horizontalDirection = 0x10;
                gWarioData.xVelocity = -0x20;
            } else {
                gWarioData.horizontalDirection = 0x20;
                gWarioData.xVelocity = 0x20;
            }
        }
        return;
    }
    switch (gWarioData.pose) {
        case WPOSE_NORMAL_SHOULDER_BASH:
        case WPOSE_NORMAL_DASH_ATTACK:
            gWarioData.unk_08 = 1;
        case WPOSE_NORMAL_SHOULDER_BASH_JUMP:
        case WPOSE_NORMAL_DASH_ATTACK_JUMP:
            if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
                if ((gSpriteData[slot].warioCollision != 0x2B) || facingRight) {
                    gWarioData.xPosition = left - gWarioData.hitboxOffsetRight;
                    {
                        /* Keep the explicit sprite-address multiply in agbcc's r0. */
                        register u32 spriteStride PIN("r0");
                        spriteStride = sizeof(gSpriteData[0]);
                        ((struct PrimarySpriteData*)((u8*)gSpriteData + spriteStride * slot))->pose =
                            SPOSE_TACKLED_RIGHT_INIT;
                    }
                    WarioRequestPose(WPOSE_NORMAL_SHOULDER_BASH_BONK);
                    gWarioData.xVelocity = -0x48;
                    gWarioData.yVelocity = 0x78;
                    m4aSongNumStart(SOUND_38);
                } else {
                    SpriteCollisionTakeDamageRight();
                    gSpriteData[slot].pose = SPOSE_PUSHED_RIGHT_INIT;
                }
            } else {
                if ((gSpriteData[slot].warioCollision != 0x2B) || !facingRight) {
                    {
                        u16 hitboxOffset = gWarioData.hitboxOffsetLeft;
                        u32 rightEdge = right;
                        gWarioData.xPosition = rightEdge - hitboxOffset + 1;
                    }
                    {
                        /* Keep the explicit sprite-address multiply in agbcc's r0. */
                        register u32 spriteStride PIN("r0");
                        spriteStride = sizeof(gSpriteData[0]);
                        ((struct PrimarySpriteData*)((u8*)gSpriteData + spriteStride * slot))->pose =
                            SPOSE_TACKLED_LEFT_INIT;
                    }
                    WarioRequestPose(WPOSE_NORMAL_SHOULDER_BASH_BONK);
                    gWarioData.xVelocity = 0x48;
                    gWarioData.yVelocity = 0x78;
                    m4aSongNumStart(SOUND_38);
                } else {
                    SpriteCollisionTakeDamageLeft();
                    gSpriteData[slot].pose = SPOSE_PUSHED_LEFT_INIT;
                }
            }
            return;
    }
    if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
        if ((gSpriteData[slot].warioCollision == 0x2B) && !facingRight) {
            SpriteCollisionTakeDamageRight();
            gSpriteData[slot].pose = SPOSE_PUSHED_RIGHT_INIT;
        } else {
            gWarioData.xPosition = left - gWarioData.hitboxOffsetRight;
            func_801EF50();
            gWarioData.xVelocity = -0x20;
            if (gSpriteData[slot].warioCollision == 0x29) {
                gSpriteData[slot].pose = SPOSE_PUSHED_RIGHT_INIT;
            }
            gWarioData.yVelocity = 0x50;
        }
    } else {
        if ((gSpriteData[slot].warioCollision == 0x2B) && facingRight) {
            SpriteCollisionTakeDamageLeft();
            gSpriteData[slot].pose = SPOSE_PUSHED_LEFT_INIT;
        } else {
            gWarioData.xPosition = (right - gWarioData.hitboxOffsetLeft) + 1;
            func_801EF94();
            gWarioData.xVelocity = 0x20;
            if (gSpriteData[slot].warioCollision == 0x29) {
                gSpriteData[slot].pose = SPOSE_PUSHED_LEFT_INIT;
            }
            gWarioData.yVelocity = 0x50;
        }
    }
}

void func_8022524(s32 slot, u16 spriteTop, u16 spriteBottom, u16 warioTop, u16 warioX)
{
    if ((gWarioData.reaction == REACTION_BOUNCY && gWarioData.pose == WPOSE_BOUNCY_JUMPING) &&
        !(gSpriteCollisionFlags & SPRITE_COLLISION_ABOVE) && (spriteTop <= warioTop)) {
        if (gSpriteCollisionFlags & SPRITE_COLLISION_BELOW) {
            WarioRequestPose(WPOSE_BOUNCY_HITTING_CEILING);
            AdvanceGoldenDivaMaskGraphics();
            SpawnSecondarySprite(warioTop, warioX, SSPRITE_40);
            SpriteSpawnAsChild(PSPRITE_0B, 0, 0, gSpriteData[slot].yPosition, gSpriteData[slot].xPosition);
            VoiceSetPlay(VS_WARIO_TREASURE);
            gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
        } else {
            AdvanceGoldenDivaMaskGraphics();
            SpriteSpawnAsChild(PSPRITE_0B, 0, 0, gSpriteData[slot].yPosition, gSpriteData[slot].xPosition);
            VoiceSetPlay(VS_WARIO_TREASURE);
            gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(1);
            if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
                SpawnSecondarySprite(warioTop, warioX + BLOCK_SIZE - PIXEL_SIZE, SSPRITE_40);
            } else {
                SpawnSecondarySprite(warioTop, warioX - (BLOCK_SIZE - PIXEL_SIZE), SSPRITE_40);
            }
        }
        return;
    }

    if (gWarioData.reaction != REACTION_NORMAL) {
        return;
    }
    if (gWarioData.damageTimer != 0) {
        return;
    }

    if (gSpriteCollisionFlags & SPRITE_COLLISION_ABOVE) {
        if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
            func_801EF50();
            gWarioData.xVelocity = -0x20;
        } else {
            func_801EF94();
            gWarioData.xVelocity = 0x20;
        }
        gWarioData.yVelocity = 0x40;
    } else if (gSpriteCollisionFlags & SPRITE_COLLISION_BELOW) {
        gSpriteCollisionTileType = 0;
        func_8023BFC(spriteBottom + 0x64, gSpriteData[slot].xPosition);
        if (gSpriteCollisionTileType == 0) {
            gWarioData.yPosition = spriteBottom + 1 - gWarioData.hitboxOffsetTop;
        }
        if (gWarioData.unk_1A != 0) {
            gWarioData.yVelocity = 0;
        } else {
            WarioRequestPose(WPOSE_NORMAL_LANDING_ON_ENEMY);
            if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
                gWarioData.horizontalDirection = 0x10;
                gWarioData.xVelocity = -0x10;
            } else {
                gWarioData.horizontalDirection = 0x20;
                gWarioData.xVelocity = 0x10;
            }
        }
    } else if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
        func_801EF50();
        gWarioData.xVelocity = -0x20;
        gWarioData.yVelocity = 0x40;
    } else {
        func_801EF94();
        gWarioData.xVelocity = 0x20;
        gWarioData.yVelocity = 0x40;
    }
    gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
}

void func_8022724(s32 slot, u16 direction)
{
    if (gSpriteCollisionFlags & SPRITE_COLLISION_ABOVE) {
        if (gWarioData.damageTimer != 0) {
            return;
        }
        if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
            func_801EF50();
            gWarioData.xVelocity = -0x20;
        } else {
            func_801EF94();
            gWarioData.xVelocity = 0x20;
        }
        gWarioData.yVelocity = 0x50;
        return;
    } else if (gSpriteCollisionFlags & SPRITE_COLLISION_BELOW && gSpriteData[slot].warioCollision == 0x43) {
        if (gWarioData.damageTimer != 0) {
            return;
        }
        WarioRequestPose(WPOSE_NORMAL_UNKNOWN_60);
        gWarioData.xVelocity = 0;
        gWarioData.yVelocity = 0;
        gWarioData.yPosition = gSpriteData[slot].yPosition - gWarioData.hitboxOffsetTop;
        gWarioData.xPosition = gSpriteData[slot].xPosition;
        gCuckooCondorHasCapturedWario = 1;
        GP_SET(gSpriteData[slot].pOamData, sCuckooCondorClawOpenOam);
        gSpriteData[slot].currentAnimationFrame = 0;
        gSpriteData[slot].animationTimer = 0;
        gSpriteData[slot].warioCollision = 0;
        VoiceSetPlay(VS_WARIO_HURT);
        return;
    }

    if ((gWarioData.damageTimer != 0) && (gSpriteData[slot].warioCollision != 0x44)) {
        return;
    }

    switch (gWarioData.pose) {
        case WPOSE_NORMAL_SHOULDER_BASH:
        case WPOSE_NORMAL_DASH_ATTACK:
            gWarioData.unk_08 = 1;
        case WPOSE_NORMAL_SHOULDER_BASH_JUMP:
        case WPOSE_NORMAL_DASH_ATTACK_JUMP:
            if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
                if (!(direction & DPAD_RIGHT)) {
                    break;
                }
                if (gSpriteData[slot].warioCollision == 0x44) {
                    gSpriteData[slot].pose = SPOSE_TACKLED_RIGHT_INIT;
                    gBossState = 1;
                }
                WarioRequestPose(WPOSE_NORMAL_SHOULDER_BASH_BONK);
                gWarioData.xVelocity = -0x20;
                gWarioData.yVelocity = 0x40;
                gSpriteData[slot].warioCollision = 0;
            } else if (direction & DPAD_LEFT) {
                if (gSpriteData[slot].warioCollision == 0x44) {
                    gSpriteData[slot].pose = SPOSE_TACKLED_LEFT_INIT;
                    gBossState = 1;
                }
                WarioRequestPose(WPOSE_NORMAL_SHOULDER_BASH_BONK);
                gWarioData.xVelocity = 0x20;
                gWarioData.yVelocity = 0x40;
                gSpriteData[slot].warioCollision = 0;
            }
            break;

        default:
            if (gWarioData.damageTimer) {
                break;
            }
            if ((gSpriteCollisionFlags & SPRITE_COLLISION_BELOW) && (gSpriteData[slot].warioCollision == 0x42) &&
                (gWarioData.unk_1A != 0)) {
                WarioRequestPose(WPOSE_NORMAL_LANDING_ON_ENEMY);
                gWarioData.yVelocity = 0;
            } else if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
                func_801EF50();
                gWarioData.xVelocity = -0x28;
                gWarioData.yVelocity = 0x48;
            } else {
                func_801EF94();
                gWarioData.xVelocity = 0x28;
                gWarioData.yVelocity = 0x48;
            }
    }
}

void func_8022948(s32 slot, u16 top)
{
    if ((gPreviousYPosition > gWarioData.yPosition) || (gWarioData.yPosition > top + HALF_BLOCK_SIZE)) {
        gSpriteCollisionFlags &= 0xFE;
    }

    if (gSpriteCollisionFlags & SPRITE_COLLISION_ABOVE) {
        if (gSpriteData[slot].warioCollision == 0x3F) {
            switch (gWarioData.pose) {
                case 0x1C:
                    m4aSongNumStart(SOUND_2B);
                case 0x1B:
                    if (gCuckooCondorMoveRight == 0) {
                        gSpriteData[slot].pose = SPOSE_CRUSHED_OR_COLLECTED_INIT;
                    }
                    SpriteCollisionLandOnEnemy();
                    gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
                    return;
            }
        }
        if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
            func_801EF50();
            gWarioData.xVelocity = -0x20;
        } else {
            func_801EF94();
            gWarioData.xVelocity = 0x20;
        }
        gWarioData.yVelocity = 0x50;
    } else {
        switch (gWarioData.pose) {
            case WPOSE_NORMAL_SHOULDER_BASH:
            case WPOSE_NORMAL_DASH_ATTACK:
                gWarioData.unk_08 = 1;
            case WPOSE_NORMAL_SHOULDER_BASH_JUMP:
            case WPOSE_NORMAL_DASH_ATTACK_JUMP:
                if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
                    if (!(gSpriteData[slot].status & SPRITE_STATUS_FACING_RIGHT) && (gCuckooCondorMoveRight == 0)) {
                        gSpriteData[slot].pose = SPOSE_TACKLED_RIGHT_INIT;
                    }
                    WarioRequestPose(WPOSE_NORMAL_SHOULDER_BASH_BONK);
                    gWarioData.xVelocity = -0x30;
                } else {
                    if ((gSpriteData[slot].status & SPRITE_STATUS_FACING_RIGHT) && (gCuckooCondorMoveRight == 0)) {
                        gSpriteData[slot].pose = SPOSE_TACKLED_LEFT_INIT;
                    }
                    WarioRequestPose(WPOSE_NORMAL_SHOULDER_BASH_BONK);
                    gWarioData.xVelocity = 0x30;
                }
                gWarioData.yVelocity = 0x60;
                m4aSongNumStart(SOUND_38);
                break;

            default:
                if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
                    func_801EF50();
                    gWarioData.xVelocity = -0x20;
                } else {
                    func_801EF94();
                    gWarioData.xVelocity = 0x20;
                }
                gWarioData.yVelocity = 0x50;
                break;
        }
    }
    gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
}

void func_8022AE8(s32 slot, u16 left, u16 right)
{
    u8 childSlot;

    childSlot = gSpriteData[slot].work3;
    switch (gWarioData.pose) {
        case WPOSE_NORMAL_SHOULDER_BASH:
        case WPOSE_NORMAL_DASH_ATTACK:
            gWarioData.unk_08 = 1;
        case WPOSE_NORMAL_SHOULDER_BASH_JUMP:
        case WPOSE_NORMAL_DASH_ATTACK_JUMP:
            if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
                gWarioData.xPosition = left - gWarioData.hitboxOffsetRight;
                WarioRequestPose(WPOSE_NORMAL_SHOULDER_BASH_BONK);
                gWarioData.xVelocity = -0x48;
                gWarioData.yVelocity = 0x78;
                SpawnSecondarySprite(
                    gSpriteData[slot].yPosition, gSpriteData[slot].xPosition - HALF_BLOCK_SIZE, SSPRITE_06
                );
            } else {
                gWarioData.xPosition = right - gWarioData.hitboxOffsetLeft + 1;
                WarioRequestPose(WPOSE_NORMAL_SHOULDER_BASH_BONK);
                gWarioData.xVelocity = 0x48;
                gWarioData.yVelocity = 0x78;
                SpawnSecondarySprite(
                    gSpriteData[slot].yPosition, gSpriteData[slot].xPosition + HALF_BLOCK_SIZE, SSPRITE_06
                );
            }
            if (gCuckooCondorMoveRight == 0) {
                gSpriteData[childSlot].pose = SPOSE_6C;
            }
            m4aSongNumStart(SOUND_38);
            break;

        default:
            if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
                gWarioData.xPosition = left - gWarioData.hitboxOffsetRight;
                func_801EF50();
                gWarioData.xVelocity = -0x20;
                gWarioData.yVelocity = 0x50;
                if (gCuckooCondorMoveRight == 0) {
                    gSpriteData[childSlot].pose = SPOSE_TACKLED_RIGHT_INIT;
                }
            } else {
                gWarioData.xPosition = right - gWarioData.hitboxOffsetLeft + 1;
                func_801EF94();
                gWarioData.xVelocity = 0x20;
                gWarioData.yVelocity = 0x50;
                if (gCuckooCondorMoveRight == 0) {
                    gSpriteData[childSlot].pose = SPOSE_TACKLED_LEFT_INIT;
                }
            }
            break;
    }
    gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
    gSpriteData[childSlot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
}

void func_8022C64(s32 slot)
{
    if (gTimerState == 0xB) {
        return;
    }

    if (gWarioData.reaction == REACTION_NORMAL) {
        if (gWarioData.damageTimer) {
            func_801E884(slot);
            return;
        }

        if (gSpriteAiDynamicGraphicsTimer) {
            if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
                SpriteCollisionTakeDamageRight();
            } else {
                SpriteCollisionTakeDamageLeft();
            }
        } else {
            SpriteCollisionTransformWarioPuffy();
        }

        gSpriteData[slot].pose = SPOSE_CRUSHED_OR_COLLECTED_INIT;
        gCuckooCondorHasCapturedWario = 0x14;
        return;
    }

    func_801E92C(slot);
}

void func_8022CE8(
    s32 slot,
    u16 warioY,
    u16 warioX,
    u16 warioLeft,
    u16 warioRight,
    u16 spriteLeft,
    u16 spriteRight,
    u16 direction
)
{
    if ((warioX < spriteLeft) || (warioX > spriteRight)) {
        gSpriteCollisionFlags &= 0xFD;
    }

    if (gSpriteCollisionFlags & SPRITE_COLLISION_ABOVE) {
        if (!(gSpriteCollisionFlags & SPRITE_COLLISION_LEFT)) {
            func_801EF94();
            gSpriteData[slot].pose = SPOSE_PUSHED_LEFT_INIT;
            gWarioData.xVelocity = 0x20;
        } else {
            func_801EF50();
            gSpriteData[slot].pose = SPOSE_PUSHED_RIGHT_INIT;
            gWarioData.xVelocity = -0x20;
        }
        gWarioData.yVelocity = 0x40;
    } else if (gSpriteCollisionFlags & SPRITE_COLLISION_BELOW) {
        if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
            gSpriteData[slot].pose = SPOSE_PUSHED_RIGHT_INIT;
            func_801EEE0();
        } else {
            gSpriteData[slot].pose = SPOSE_PUSHED_LEFT_INIT;
            func_801EE9C();
        }
    } else {
        switch (gWarioData.pose) {
            case WPOSE_NORMAL_SHOULDER_BASH:
            case WPOSE_NORMAL_DASH_ATTACK:
                gWarioData.unk_08 = 1;
            case WPOSE_NORMAL_SHOULDER_BASH_JUMP:
            case WPOSE_NORMAL_DASH_ATTACK_JUMP:
                if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
                    if (direction & DPAD_RIGHT) {
                        gSpriteData[slot].pose = SPOSE_TACKLED_RIGHT_INIT;
                        WarioRequestPose(WPOSE_NORMAL_SHOULDER_BASH_BONK);
                        gWarioData.xVelocity = -0x20;
                        gWarioData.yVelocity = 0x40;
                        SpawnSecondarySprite(warioY - BLOCK_SIZE, warioRight + HALF_BLOCK_SIZE, SSPRITE_40);
                        m4aSongNumStart(SOUND_38);
                    } else {
                        gSpriteData[slot].pose = SPOSE_PUSHED_RIGHT_INIT;
                    }
                } else {
                    if (direction & DPAD_LEFT) {
                        gSpriteData[slot].pose = SPOSE_TACKLED_LEFT_INIT;
                        WarioRequestPose(WPOSE_NORMAL_SHOULDER_BASH_BONK);
                        gWarioData.xVelocity = 0x20;
                        gWarioData.yVelocity = 0x40;
                        SpawnSecondarySprite(warioY - BLOCK_SIZE, warioLeft - HALF_BLOCK_SIZE, SSPRITE_40);
                        m4aSongNumStart(SOUND_38);
                    } else {
                        gSpriteData[slot].pose = SPOSE_PUSHED_LEFT_INIT;
                    }
                }
                break;

            default:
                if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
                    func_801EF50();
                    gSpriteData[slot].pose = SPOSE_PUSHED_RIGHT_INIT;
                    gWarioData.xVelocity = -0x20;
                } else {
                    func_801EF94();
                    gSpriteData[slot].pose = SPOSE_PUSHED_LEFT_INIT;
                    gWarioData.xVelocity = 0x20;
                }
                gWarioData.yVelocity = 0x40;
                break;
        }
    }
    gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
}

void func_8022EF8(s32 slot, u16 left, u16 right)
{
    if (gSpriteCollisionFlags & SPRITE_COLLISION_ABOVE) {
        if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
            func_801EF50();
        } else {
            func_801EF94();
        }
        gWarioData.yVelocity = 0x40;
        return;
    }

    if (gSpriteCollisionFlags & SPRITE_COLLISION_BELOW) {
        if (gWarioData.unk_1A != 0) {
            gWarioData.yVelocity = 0;
        }
        if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
            gSpriteData[slot].pose = SPOSE_PUSHED_RIGHT_INIT;
        } else {
            gSpriteData[slot].pose = SPOSE_PUSHED_LEFT_INIT;
        }
        return;
    }

    if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
        gWarioData.xPosition = left - gWarioData.hitboxOffsetRight;
        func_801EF50();
        gSpriteData[slot].pose = SPOSE_PUSHED_RIGHT_INIT;
    } else {
        gWarioData.xPosition = right - gWarioData.hitboxOffsetLeft + 1;
        func_801EF94();
        gSpriteData[slot].pose = SPOSE_PUSHED_LEFT_INIT;
    }
    gWarioData.yVelocity = 0x40;
}

void func_8022FC8(s32 slot, u16 left, u16 right)
{
    if (gSpriteCollisionFlags & SPRITE_COLLISION_ABOVE) {
        if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
            func_801EF50();
        } else {
            func_801EF94();
        }
        gWarioData.yVelocity = 0x40;
    } else if (gSpriteCollisionFlags & SPRITE_COLLISION_BELOW) {
        if (gWarioData.unk_1A != 0) {
            gWarioData.yVelocity = 0;
        }
        if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
            gSpriteData[slot].pose = SPOSE_PUSHED_RIGHT_INIT;
        } else {
            gSpriteData[slot].pose = SPOSE_PUSHED_LEFT_INIT;
        }
    } else {
        switch (gWarioData.pose) {
            case WPOSE_NORMAL_SHOULDER_BASH:
            case WPOSE_NORMAL_DASH_ATTACK:
                gWarioData.unk_08 = 1;
            case WPOSE_NORMAL_SHOULDER_BASH_JUMP:
            case WPOSE_NORMAL_DASH_ATTACK_JUMP:
                if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
                    gWarioData.xPosition = left - gWarioData.hitboxOffsetRight;
                    gSpriteData[slot].pose = SPOSE_TACKLED_RIGHT_INIT;
                } else {
                    gWarioData.xPosition = (right - gWarioData.hitboxOffsetLeft) + 1;
                    gSpriteData[slot].pose = SPOSE_TACKLED_LEFT_INIT;
                }
                WarioRequestPose(WPOSE_NORMAL_SHOULDER_BASH_BONK);
                m4aSongNumStart(SOUND_38);
                break;

            default:
                if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
                    gWarioData.xPosition = left - gWarioData.hitboxOffsetRight;
                    func_801EF50();
                    gSpriteData[slot].pose = SPOSE_PUSHED_RIGHT_INIT;
                } else {
                    gWarioData.xPosition = (right - gWarioData.hitboxOffsetLeft) + 1;
                    func_801EF94();
                    gSpriteData[slot].pose = SPOSE_PUSHED_LEFT_INIT;
                }
                gWarioData.yVelocity = 0x40;
                break;
        }
    }
}

void func_8023110(s32 slot, u16 left, u16 right)
{
    if (gSpriteCollisionFlags & SPRITE_COLLISION_ABOVE) {
        if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
            func_801EF50();
        } else {
            func_801EF94();
        }
        gWarioData.yVelocity = 0x40;
    } else {
        switch (gWarioData.pose) {
            case WPOSE_NORMAL_SHOULDER_BASH:
            case WPOSE_NORMAL_DASH_ATTACK:
                gWarioData.unk_08 = 1;
            case WPOSE_NORMAL_SHOULDER_BASH_JUMP:
            case WPOSE_NORMAL_DASH_ATTACK_JUMP:
                if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
                    gWarioData.xPosition = left - gWarioData.hitboxOffsetRight;
                    WarioRequestPose(WPOSE_NORMAL_SHOULDER_BASH_BONK);
                    gWarioData.xVelocity = -0x30;
                } else {
                    gWarioData.xPosition = right - gWarioData.hitboxOffsetLeft + 1;
                    WarioRequestPose(WPOSE_NORMAL_SHOULDER_BASH_BONK);
                    gWarioData.xVelocity = 0x30;
                }
                gWarioData.yVelocity = 0x50;
                break;

            default:
                if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
                    func_801EF50();
                    gWarioData.xVelocity = -0x20;
                } else {
                    func_801EF94();
                    gWarioData.xVelocity = 0x20;
                }
                gWarioData.yVelocity = 0x50;
                break;
        }
    }
}

void func_80231F8(s32 slot, u16 direction)
{
    u8 pose;

    pose = func_801EFD4();
    if (pose == 0) {
        if (gSpriteCollisionFlags & SPRITE_COLLISION_ABOVE) {
            switch (gWarioData.pose) {
                case WPOSE_NORMAL_SUPER_GROUND_POUND:
                    m4aSongNumStart(SOUND_2B);
                case WPOSE_NORMAL_GROUND_POUND:
                    pose = SPOSE_CRUSHED_OR_COLLECTED_INIT;
                    SpriteCollisionLandOnEnemy();
                    break;

                default:
                    pose = func_801F00C(slot, direction, TRUE);
                    break;
            }
        } else if (gSpriteCollisionFlags & SPRITE_COLLISION_BELOW) {
            if (!(gSpriteData[slot].status & SPRITE_STATUS_FACING_RIGHT)) {
                pose = SPOSE_33;
                if (gWarioData.damageTimer == 0) {
                    SpriteCollisionTransformWarioBouncy();
                }
            } else {
                pose = SPOSE_35;
                if (gWarioData.damageTimer == 0) {
                    SpriteCollisionTransformWarioBouncy();
                }
            }
        } else if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
            pose = func_801F14C(slot, TRUE);
        } else if (gSpriteCollisionFlags & SPRITE_COLLISION_RIGHT) {
            pose = func_801F14C(slot, FALSE);
        } else {
            pose = gSpriteData[slot].pose;
        }
    }
    gSpriteData[slot].pose = pose;
}

void func_80232C4(s32 slot, u16 direction)
{
    u16 status;
    u8 pose;

    status = gSpriteData[slot].status & SPRITE_STATUS_FACING_RIGHT;
    pose = func_801EFD4();
    if (!pose) {
        if (gSpriteCollisionFlags & SPRITE_COLLISION_ABOVE) {
            switch (gWarioData.pose) {
                case WPOSE_NORMAL_SUPER_GROUND_POUND:
                    m4aSongNumStart(SOUND_2B);
                case WPOSE_NORMAL_GROUND_POUND:
                    pose = SPOSE_CRUSHED_OR_COLLECTED_INIT;
                    SpriteCollisionLandOnEnemy();
                    break;

                default:
                    pose = func_801F00C(slot, direction, TRUE);
                    break;
            }
        } else if (gSpriteCollisionFlags & SPRITE_COLLISION_BELOW) {
            if (!(gSpriteData[slot].status & SPRITE_STATUS_FACING_RIGHT)) {
                pose = SPOSE_33;
                if (gWarioData.damageTimer == 0) {
                    SpriteCollisionTransformWarioBouncy();
                }
            } else {
                pose = SPOSE_35;
                if (gWarioData.damageTimer == 0) {
                    SpriteCollisionTransformWarioBouncy();
                }
            }
        } else if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
            if (!status) {
                pose = SPOSE_33;
                SpriteCollisionTransformWarioBouncy();
            } else {
                pose = func_801F14C(slot, TRUE);
            }
        } else if (gSpriteCollisionFlags & SPRITE_COLLISION_RIGHT) {
            if (status) {
                pose = SPOSE_35;
                if (gWarioData.damageTimer == 0) {
                    SpriteCollisionTransformWarioBouncy();
                }
            } else {
                pose = func_801F14C(slot, FALSE);
            }
        } else {
            pose = gSpriteData[slot].pose;
        }
    }
    gSpriteData[slot].pose = pose;
}

void func_80233B8(s32 slot, u16 direction)
{
    u8 pose;

    pose = func_801EFD4();
    if (pose == 0) {
        if (gSpriteCollisionFlags & SPRITE_COLLISION_ABOVE) {
            switch (gWarioData.pose) {
                case WPOSE_NORMAL_SUPER_GROUND_POUND:
                    m4aSongNumStart(SOUND_2B);
                case WPOSE_NORMAL_GROUND_POUND:
                    pose = SPOSE_CRUSHED_OR_COLLECTED_INIT;
                    SpriteCollisionLandOnEnemy();
                    break;

                default:
                    pose = func_801F00C(slot, direction, TRUE);
                    break;
            }
        } else if (gSpriteCollisionFlags & SPRITE_COLLISION_BELOW) {
            if (!(gSpriteData[slot].status & SPRITE_STATUS_FACING_RIGHT)) {
                if (gWarioData.damageTimer) {
                    pose = SPOSE_PUSHED_RIGHT_INIT;
                } else {
                    pose = SPOSE_27;
                    SpriteCollisionTakeDamageRight();
                    SpriteCollisionMakeWarioDropCoins(slot);
                }
            } else {
                if (gWarioData.damageTimer) {
                    pose = SPOSE_PUSHED_LEFT_INIT;
                } else {
                    pose = SPOSE_29;
                    SpriteCollisionTakeDamageLeft();
                    SpriteCollisionMakeWarioDropCoins(slot);
                }
            }
        } else if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
            pose = func_801F14C(slot, TRUE);
        } else if (gSpriteCollisionFlags & SPRITE_COLLISION_RIGHT) {
            pose = func_801F14C(slot, FALSE);
        } else {
            pose = gSpriteData[slot].pose;
        }
    }
    gSpriteData[slot].pose = pose;
}

void func_802349C(s32 slot, u16 direction)
{
    u16 status;
    u8 pose;

    status = gSpriteData[slot].status & SPRITE_STATUS_FACING_RIGHT;
    pose = func_801EFD4();
    if (!pose) {
        if (gSpriteCollisionFlags & SPRITE_COLLISION_ABOVE) {
            switch (gWarioData.pose) {
                case WPOSE_NORMAL_SUPER_GROUND_POUND:
                    m4aSongNumStart(SOUND_2B);
                case WPOSE_NORMAL_GROUND_POUND:
                    pose = SPOSE_CRUSHED_OR_COLLECTED_INIT;
                    SpriteCollisionLandOnEnemy();
                    break;

                default:
                    pose = func_801F00C(slot, direction, TRUE);
                    break;
            }
        } else if (gSpriteCollisionFlags & SPRITE_COLLISION_BELOW) {
            if (!(gSpriteData[slot].status & SPRITE_STATUS_FACING_RIGHT)) {
                if (gWarioData.damageTimer) {
                    pose = SPOSE_PUSHED_RIGHT_INIT;
                } else {
                    pose = SPOSE_27;
                    SpriteCollisionTakeDamageRight();
                }
            } else {
                if (gWarioData.damageTimer) {
                    pose = SPOSE_PUSHED_LEFT_INIT;
                } else {
                    pose = SPOSE_29;
                    SpriteCollisionTakeDamageLeft();
                }
            }
        } else if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
            if (!status) {
                if (gWarioData.damageTimer) {
                    pose = SPOSE_PUSHED_RIGHT_INIT;
                } else {
                    pose = SPOSE_27;
                    SpriteCollisionTakeDamageRight();
                }
            } else {
                pose = func_801F14C(slot, TRUE);
            }
        } else if (gSpriteCollisionFlags & SPRITE_COLLISION_RIGHT) {
            if (status) {
                if (gWarioData.damageTimer) {
                    pose = SPOSE_PUSHED_LEFT_INIT;
                } else {
                    pose = SPOSE_29;
                    SpriteCollisionTakeDamageLeft();
                }
            } else {
                pose = func_801F14C(slot, FALSE);
            }
        } else {
            pose = gSpriteData[slot].pose;
        }
    }
    gSpriteData[slot].pose = pose;
}

void func_80235A0(s32 slot, u16 direction)
{
    u8 pose;

    pose = func_801EFD4();
    if (pose != 0) {
        gSpriteData[slot].pose = pose;
        return;
    }

    if (gSpriteCollisionFlags & SPRITE_COLLISION_ABOVE) {
        switch (gWarioData.pose) {
            case WPOSE_NORMAL_SUPER_GROUND_POUND:
                m4aSongNumStart(SOUND_2B);
            case WPOSE_NORMAL_GROUND_POUND:
                pose = SPOSE_CRUSHED_OR_COLLECTED_INIT;
                SpriteCollisionLandOnEnemy();
                break;

            case WPOSE_NORMAL_SHOULDER_BASH:
            case WPOSE_NORMAL_DASH_ATTACK:
            case WPOSE_NORMAL_SHOULDER_BASH_JUMP:
            case WPOSE_NORMAL_DASH_ATTACK_JUMP:
                pose = func_801F00C(slot, direction, TRUE);
                break;

            default:
                if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
                    pose = SPOSE_33;
                    func_801EF50();
                } else {
                    pose = SPOSE_35;
                    func_801EF94();
                }
                gWarioData.yVelocity = 0x40;
                break;
        }
    } else if (gSpriteCollisionFlags & SPRITE_COLLISION_BELOW) {
        if (!(gSpriteData[slot].status & SPRITE_STATUS_FACING_RIGHT)) {
            if ((gWarioData.damageTimer != 0) || (gSpriteData[slot].warioCollision == 0x32)) {
                pose = SPOSE_PUSHED_RIGHT_INIT;
            } else {
                pose = SPOSE_27;
                SpriteCollisionTransformWarioPuffy();
            }
        } else if ((gWarioData.damageTimer != 0) || (gSpriteData[slot].warioCollision == 0x32)) {
            pose = SPOSE_PUSHED_LEFT_INIT;
        } else {
            pose = SPOSE_29;
            SpriteCollisionTransformWarioPuffy();
        }
    } else if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
        pose = func_801F14C(slot, TRUE);
    } else if (gSpriteCollisionFlags & SPRITE_COLLISION_RIGHT) {
        pose = func_801F14C(slot, FALSE);
    } else {
        pose = gSpriteData[slot].pose;
    }
    gSpriteData[slot].pose = pose;
}

void func_80236C4(s32 slot, u16 direction, u16 left, u16 right)
{
    u8 pose;

    pose = 0;
    switch (gWarioData.pose) {
        case WPOSE_NORMAL_STARTING_ROLL:
        case WPOSE_NORMAL_JUMPING_OUT_OF_ROLL:
        case WPOSE_NORMAL_ROLLING:
        case WPOSE_NORMAL_ROLLING_JUMP:
            if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
                pose = SPOSE_TACKLED_RIGHT_INIT;
                gWarioData.xPosition = left - gWarioData.hitboxOffsetRight;
            } else {
                pose = SPOSE_TACKLED_LEFT_INIT;
                gWarioData.xPosition = right - gWarioData.hitboxOffsetLeft + 1;
            }
            WarioRequestPose(WPOSE_NORMAL_ROLLING_BONK);
    }

    if (pose == 0) {
        if (gSpriteCollisionFlags & SPRITE_COLLISION_ABOVE) {
            switch (gWarioData.pose) {
                case WPOSE_NORMAL_SUPER_GROUND_POUND:
                    m4aSongNumStart(SOUND_2B);
                case WPOSE_NORMAL_GROUND_POUND:
                    pose = SPOSE_CRUSHED_OR_COLLECTED_INIT;
                    SpriteCollisionLandOnEnemy();
                    break;

                default:
                    pose = func_801F00C(slot, direction, TRUE);
            }
        } else if (gSpriteCollisionFlags & SPRITE_COLLISION_BELOW) {
            pose = func_801F00C(slot, direction, FALSE);
        } else if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
            gWarioData.xPosition = left - gWarioData.hitboxOffsetRight;
            func_801EF50();
            gWarioData.yVelocity = 0x40;
            pose = SPOSE_PUSHED_RIGHT_INIT;
        } else if (gSpriteCollisionFlags & SPRITE_COLLISION_RIGHT) {
            gWarioData.xPosition = (right - gWarioData.hitboxOffsetLeft) + 1;
            func_801EF94();
            gWarioData.yVelocity = 0x40;
            pose = SPOSE_PUSHED_LEFT_INIT;
        } else {
            pose = gSpriteData[slot].pose;
        }
    }
    gSpriteData[slot].pose = pose;
}

void func_80237E4(s32 slot)
{
    if (gSpriteCollisionFlags & SPRITE_COLLISION_ABOVE) {
        switch (gWarioData.pose) {
            case WPOSE_NORMAL_SUPER_GROUND_POUND:
                m4aSongNumStart(SOUND_2B);
            case WPOSE_NORMAL_GROUND_POUND:
                gSpriteData[slot].pose = SPOSE_CRUSHED_OR_COLLECTED_INIT;
                SpriteCollisionLandOnEnemy();
                break;

            default:
                if (gWarioData.damageTimer == 0) {
                    if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
                        WarioRequestPose(WPOSE_NORMAL_LANDING_ON_ENEMY);
                        gWarioData.xVelocity = -0x14;
                    } else {
                        WarioRequestPose(WPOSE_NORMAL_LANDING_ON_ENEMY);
                        gWarioData.xVelocity = 0x14;
                    }
                    gWarioData.yVelocity = 0x64;
                }
                break;
        }
    } else if (gSpriteCollisionFlags & SPRITE_COLLISION_LEFT) {
        if (gWarioData.damageTimer == 0) {
            SpriteCollisionTakeDamageRight();
            gCuckooCondorPendulumLength = 0x3C;
            gCuckooCondorHasCapturedWario = 0;
        }
    }
}
