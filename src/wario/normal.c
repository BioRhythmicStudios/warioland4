#include "gba/m4a.h"
#include "global_data.h"
#include "block.h"
#include "door.h"
#include "input.h"
#include "main.h"
#include "score.h"
#include "screen_shake.h"
#include "sound.h"
#define CopyWarioPalette NormalHeader_CopyWarioPalette
#define GetAdjustedWarioXVelocity NormalHeader_GetAdjustedWarioXVelocity
#define func_8016614 NormalHeader_func_8016614
#include "wario/normal.h"
#include "wario/swimming.h"
#undef CopyWarioPalette
#undef GetAdjustedWarioXVelocity
#undef func_8016614
#include "voice_set.h"
#include "wario_palette.h"

#ifdef PORTABLE
#include <string.h>
#endif

extern const u16 sWarioCollisionPointTable[];
#define sWarioCollisionPoints ((const u16 (*)[3])sWarioCollisionPointTable)
#define WARIO_POINT(side, index) ((sWarioCollisionPointTable + (index))[(side) * 3])
extern s32 GetBackgroundCollisionAtPosition(u16 y, u16 x);
#ifdef PORTABLE
extern void CopyWarioPalette(const void *source, int destinationIndex, int size);
#else
extern void CopyWarioPalette(const u16 *source, u16 destinationIndex, u16 count);
#endif
extern void UpdateWarioPositionHistory(void);
extern void PrepareWarioUpdate(void);
extern void UpdateWarioHorizontalCollisionOffset(void);
extern u32 GetAdjustedWarioXVelocity(void);
extern void func_8016614(u32 value);
extern u32 ProcessSideTileInteraction(u32 y, u32 x);
extern u32 ProcessPointTileInteraction(u32 y, u32 x);
extern u32 ProcessHorizontalTileInteraction(u32 y, u32 x);

u8 UpdateNormalWarioPose(void)
{
    u8 pose;

    pose = 0xFF;
    if ((gScreenShakeY.amplitude & 0x80) && gScreenShakeY.duration > 44) {
        switch (gWarioData.pose) {
            case WPOSE_NORMAL_GROUND_POUND_LANDING:
            case WPOSE_NORMAL_SUPER_GROUND_POUND_LANDING:
                break;

            case WPOSE_NORMAL_CLIMBING_LADDER:
            case WPOSE_NORMAL_IDLE_ON_LADDER:
            case WPOSE_NORMAL_CLIMBING_FENCE_VERTICAL:
            case WPOSE_NORMAL_CLIMBING_FENCE_HORIZONTAL:
            case WPOSE_NORMAL_IDLE_ON_FENCE:
                pose = WPOSE_NORMAL_UNKNOWN_48;
                break;

            default:
            case WPOSE_NORMAL_LANDING_ON_ENEMY:
            case WPOSE_NORMAL_LIFTING_SPRITE:
            case WPOSE_NORMAL_LIFTING_HEAVY_SPRITE:
            case WPOSE_NORMAL_CHARGING_THROW_1:
            case WPOSE_NORMAL_CHARGING_THROW_2:
            case WPOSE_NORMAL_WEAK_THROW:
            case WPOSE_NORMAL_CHARGED_THROW:
            case WPOSE_NORMAL_WEAK_THROW_UPWARDS:
            case WPOSE_NORMAL_TURNING_WHILE_CHARING_THROW:
            case WPOSE_NORMAL_SLIDING_DOWN_LADDER:
                if (gWarioData.unk_1A <= 1) {
                    pose = WPOSE_NORMAL_STUNNED;
                }
                break;
        }

    }

    if (pose == 0xFF) {
        pose = GF(sWarioNormalPoseTable[gWarioData.pose])();
    }
    return pose;
}


u8 UpdateNormalWarioWalking(void)
{
  struct WarioData *wario;
  register s32 acceleration PIN("r4");
  s32 mask;
  const struct WarioAnimationFrame *animation;
  u16 pressed;
  register u16 held PIN("r3");
  int dustType;
  register u32 direction PIN("r2");
  s32 value;
  struct WarioData **warioPointer;
  u8 frame;
  pressed = gButtonsPressed;
  value = pressed & 0x0001;
  wario = &gWarioData;
  if ((value != 0) || (wario->unk_07 != 0))
  {
    wario->unk_08 = 1;
    return 0xFE;
  }
  {
    u32 button;
    button = 0x0002;
    button &= pressed;
    if (button != 0)
    {
      if (gCurrentCarriedSprite.state != 0)
      {
        return WPOSE_NORMAL_CHARGING_THROW_1;
      }
      return WPOSE_NORMAL_SHOULDER_BASH;
    }
  }
  {
    register u16 input PIN("r1");

    input = gButtonsHeld;
    value = 0x0080;
    value &= input;
    if (value != 0)
    {
      return WPOSE_NORMAL_CROUCHING;
    }
  }
  value = GetBackgroundCollisionAtPosition(wario->yPosition + 1, wario->xPosition - 30);
  ASM_BARRIER(: "=r"(acceleration) : "0"(value));
  acceleration >>= 16;
  mask = 0xFF;
  acceleration &= mask;
  value = GetBackgroundCollisionAtPosition(wario->yPosition + 1, wario->xPosition + 30);
  value >>= 16;
  value = value & mask;
  if ((acceleration == 11) || (value == 11))
  {
    acceleration = 1;
  }
  else
  {
    acceleration = 6;
  }
  {
    struct WarioData *base;
    u16 *buttons;
    buttons = &gButtonsHeld;
    base = &gWarioData;
    held = *buttons;
    direction = *(volatile u16 *)&base->horizontalDirection;
    value = held;
    value &= direction;
    wario = base;
  }
  if (value != 0)

  {
    if (direction & 0x0010)
    {
      {
        register u32 branchRawVelocity PIN("r1");
        s32 branchVelocity;

        branchRawVelocity = *(volatile u16 *)&wario->xVelocity;
        branchVelocity = (s16) wario->xVelocity;
        if (branchVelocity <= 19)
        {
          value = branchRawVelocity + acceleration;
          goto store_accelerated_velocity;
        }
        value = held & (0x0200 | 0x0100);
        if ((value != 0) && (gCurrentCarriedSprite.state == 0))
        {
          if (branchVelocity <= 63)
          {
            /* agbcc otherwise updates r1 in place instead of producing the required r0 result. */
            value = branchRawVelocity + 6;
            wario->xVelocity = value;
            goto update_animation;
          }
          /* agbcc otherwise updates r1 in place instead of producing the required r0 result. */
          value = branchRawVelocity + 2;
          wario->xVelocity = value;
          if (((s16) value) > 127)
          {
            wario->xVelocity = 128;
            return WPOSE_NORMAL_DASH_ATTACK;
          }
          goto update_animation;
        }
      }
      {
        register u16 branchRawVelocity PIN("r0");
        s32 branchVelocity;

        branchRawVelocity = wario->xVelocity;
        branchVelocity = (s16) wario->xVelocity;
        if (branchVelocity <= 63)
        {
          branchRawVelocity += 6;
          wario->xVelocity = branchRawVelocity;
          if (((s16) branchRawVelocity) <= 64)
          {
            goto update_animation;
          }
        }
        else
        {
          if (branchVelocity <= 64)
          {
            goto update_animation;
          }
          branchRawVelocity -= 6;
          wario->xVelocity = branchRawVelocity;
          if (((s16) branchRawVelocity) > 63)
          {
            goto update_animation;
          }
        }
        wario->xVelocity = 64;
        goto update_animation;
      }
    }
    else
    {
      {
        u16 branchRawVelocity;
        s32 branchVelocity;

        branchRawVelocity = wario->xVelocity;
        branchVelocity = (s16) wario->xVelocity;
        if (branchVelocity > (-20))
        {
          value = branchRawVelocity - acceleration;
store_accelerated_velocity:
          wario->xVelocity = value;
          goto update_animation;
        }
        value = held & (0x0200 | 0x0100);
        if ((value != 0) && (gCurrentCarriedSprite.state == 0))
        {
          if (branchVelocity > (-64))
          {
            value = branchRawVelocity - 6;
            wario->xVelocity = value;
            goto update_animation;
          }
          value = branchRawVelocity - 2;
          wario->xVelocity = value;
          if (((s16) value) <= (-128))
          {
            wario->xVelocity = -128;
            return WPOSE_NORMAL_DASH_ATTACK;
          }
          goto update_animation;
        }
      }
      {
        register u16 branchRawVelocity PIN("r0");
        s32 branchVelocity;
        register s32 velocityLimit PIN("r2");

        branchRawVelocity = wario->xVelocity;
        branchVelocity = (s16) wario->xVelocity;
        velocityLimit = -64;
        if (branchVelocity > velocityLimit)
        {
          branchRawVelocity -= 6;
          wario->xVelocity = branchRawVelocity;
          if (((s16) branchRawVelocity) >= velocityLimit)
          {
            goto update_animation;
          }
          (*(warioPointer = &wario))->xVelocity = -64;
          goto update_animation;
        }
        else
        {
          if (branchVelocity >= velocityLimit)
          {
            goto update_animation;
          }
          branchRawVelocity += 6;
          wario->xVelocity = branchRawVelocity;
          if (((s16) branchRawVelocity) <= velocityLimit)
          {
            goto update_animation;
          }
          wario->xVelocity = velocityLimit;
          goto update_animation;
        }
      }
    }
  }
  else
  {
    if (((u16) (wario->xVelocity + 96)) > 192)
    {
      return WPOSE_NORMAL_STOPPING_FROM_DASH;
    }
    {
      register s32 turnTest;
      turnTest = 0x30;
      turnTest ^= direction;
      turnTest &= held;
      value = turnTest;
    }
    value <<= 16;
    if (value != 0)
    {
      return WPOSE_NORMAL_TURNING;
    }
    if (acceleration == 1)
    {
      return WPOSE_NORMAL_SLIDING_ON_ICE;
    }
    return WPOSE_NORMAL_STANDING;
  }
  update_animation:
  if ((gCurrentCarriedSprite.unk1 & 0xF) > 1)
  {
    animation = sWarioFastWalkingAnimation;
    if (wario->unk_1E >= animation[wario->unk_1F].time)
    {
      wario->unk_1E = 0;
      wario->unk_1F++;
      frame = wario->unk_1F;
      if (animation[frame].time == 0)
      {
        wario->unk_1F = 0;
        gWarioDustEffect1.unk0 = 5;
      }
      else
      {
        value = frame;
        if ((value == 2) || (value == 9))
        {
          m4aSongNumStart(1);
        }
      }
    }
  }
  else
  {
    animation = sWarioWalkingAnimation;
    if (wario->unk_1E >= animation[wario->unk_1F].time)
    {
      wario->unk_1E = 0;
      wario->unk_1F++;
      frame = wario->unk_1F;
      dustType = 5;
      if (animation[frame].time == 0)
      {
        wario->unk_1F = 0;
        gWarioDustEffect1.unk0 = dustType;
      }
      else
      {
        {
          register u32 frameValue PIN("r0");
          frameValue = frame;
          value = frameValue;
        }
        if ((value == 2) || (value == 9))
        {
          m4aSongNumStart(1);
        }
      }
    }
  }

  return 0xFF;
}


u8 UpdateNormalWarioTurning(void)
{
    struct WarioData* wario;
    struct WarioData* data;
    const struct WarioAnimationFrame* animation;
    u16 pressed;
    u8 pose;
    u8 frame;

    data = &gWarioData;
    pose = data->pose;
    wario = data;
    if (pose == WPOSE_NORMAL_TURNING) {
        pressed = gButtonsPressed;
        if (pressed & A_BUTTON) {
            return WPOSE_NORMAL_JUMPING;
        }
        {
            u32 button;

            button = B_BUTTON;
            button &= pressed;
            if (button != 0) {
                if (gCurrentCarriedSprite.state != 0) {
                    return WPOSE_NORMAL_CHARGING_THROW_1;
                }
                return WPOSE_NORMAL_SHOULDER_BASH;
            }
        }
    }

    animation = sWarioTurningAnimation;
    if (wario->unk_1E >= animation[wario->unk_1F].time) {
        wario->unk_1E = 0;
        wario->unk_1F++;
        frame = wario->unk_1F;
        if (animation[frame].time == 0) {
            if (wario->pose == WPOSE_NORMAL_TURNING) {
                return WPOSE_NORMAL_STANDING;
            }
            return WPOSE_NORMAL_UNKNOWN_64;
        }
    }

    return 0xFF;
}


u8 UpdateNormalWarioStanding(void)
{
    register struct WarioData* wario PIN("r4");
    register int one PIN("r5");
    const struct WarioAnimationFrame* animation;
    u16 pressed;
    u16 held;
    u16 direction;
    register unsigned int value PIN("r0");
    int tile;
    u8 zero;

    pressed = gButtonsPressed;
    one = 1;
    value = one;
    value &= pressed;
    if ((value != 0) || (gWarioData.unk_07 != 0)) {
        return WPOSE_NORMAL_JUMPING;
    }

    wario = &gWarioData;
    value = B_BUTTON;
    value &= pressed;
    if (value != 0) {
        if (gCurrentCarriedSprite.state != 0) {
            return WPOSE_NORMAL_CHARGING_THROW_1;
        }
        return WPOSE_NORMAL_SHOULDER_BASH;
    }

    held = gButtonsHeld;
    if (held & DPAD_UP) {
        if (*(u8*)&gWarioCanEnterDoor != 0) {
            tile = GetBackgroundCollisionAtPosition(wario->yPosition, wario->xPosition) >> 16;
            if (tile == 1) {
                tile = GetBackgroundCollisionAtPosition(wario->yPosition - 119, wario->xPosition) >> 16;
                if (tile == 1) {
                    return WPOSE_NORMAL_ENTERING_DOOR;
                }
            }
        }

        if (gCurrentCarriedSprite.state == 0) {
            wario = &gWarioData;
            tile = GetBackgroundCollisionAtPosition(wario->yPosition - 119, wario->xPosition) >> 16;
            if ((u32)(tile - 2) <= 1) {
                wario->xPosition = (wario->xPosition & 0xFFC0) + 32;
                wario->yPosition -= 4;
                return WPOSE_NORMAL_CLIMBING_LADDER;
            }
            if (tile == 4) {
                wario->yPosition -= 4;
                return WPOSE_NORMAL_CLIMBING_FENCE_VERTICAL;
            }
        }
        return WPOSE_NORMAL_LOOKING_UP;
    }

    if (held & DPAD_DOWN) {
        tile = GetBackgroundCollisionAtPosition(wario->yPosition + 1, wario->xPosition) >> 16;
        if (gCurrentCarriedSprite.state == 0) {
            if (tile == 7) {
                wario->xPosition = (wario->xPosition & 0xFFC0) + 64;
                return WPOSE_NORMAL_ENTERING_PIPE_DOWN;
            }
            if (tile == 8) {
                wario->xPosition &= 0xFFC0;
                return WPOSE_NORMAL_ENTERING_PIPE_DOWN;
            }
            if (tile == 3) {
                wario->xPosition = (wario->xPosition & 0xFFC0) + 32;
                wario->yPosition += 4;
                return WPOSE_NORMAL_CLIMBING_LADDER;
            }
        }
        return WPOSE_NORMAL_CROUCHING;
    }

    direction = wario->horizontalDirection;
    value = held;
    value &= direction;
    if (value != 0) {
        return WPOSE_NORMAL_WALKING;
    }
    value = DPAD_LEFT | DPAD_RIGHT;
    value ^= direction;
    value &= held;
    if (value != 0) {
        return WPOSE_NORMAL_TURNING;
    }

    if (gCurrentCarriedSprite.state == 0) {
        animation = sWarioStandingAnimation;
        if (wario->unk_1E >= animation[wario->unk_1F].time) {
            wario->unk_1E = 0;
            wario->unk_1F++;
            if (animation[wario->unk_1F].time == 0) {
                zero = gSwitchPressed;
                if (zero != 0) {
                    return WPOSE_NORMAL_UNKNOWN_53;
                }
                value = wario->unk_0A;
                if (value > 6) {
                    zero = gMainTimer;
                    value = one;
                    value &= zero;
                    if (value != 0) {
                        return WPOSE_NORMAL_JUMP_ROPE;
                    }
                    return WPOSE_NORMAL_DUMBBELLS_1;
                }
                wario->unk_1F = zero;
                value++;
                wario->unk_0A = value;
            }
        }
    }

    return 0xFF;
}


u8 UpdateNormalWarioJumping(void)
{
    struct WarioData* wario;
    const struct WarioAnimationFrame* animation;
    u16 pressed;
    int value;

    pressed = gButtonsPressed;
    value = B_BUTTON;
    value &= pressed;
    if ((value != 0) && (gCurrentCarriedSprite.state != 0)) {
        return WPOSE_NORMAL_CHARGING_THROW_1;
    }

    wario = &gWarioData;
    animation = sWarioJumpingAnimation;
    if (wario->unk_1E >= animation[wario->unk_1F].time) {
        wario->unk_1E = 0;
        wario->unk_1F++;
        if (animation[wario->unk_1F].time == 0) {
            wario->unk_08 = 1;
            return 0xFE;
        }
    }

    return 0xFF;
}

u8 UpdateNormalWarioFalling(void)
{
    register int value PIN("r4");
    struct WarioData* wario;
    u16* heldPtr;
    register int test PIN("r0");
    int tile;

    {
        struct CarriedSprite* carried;
        register u32 input PIN("r2");
        u16 pressed;

        carried = &gCurrentCarriedSprite;
        test = carried->state;
        input = (u32)&gButtonsPressed;
        if (test != 0) {
            if (gButtonsHeld & DPAD_UP) {
                carried->unk1 |= 0x80;
            } else {
                carried->unk1 &= 0xF;
            }
            input = (u32)&gButtonsPressed;
            pressed = *(u16*)input;
            test = B_BUTTON;
            test &= pressed;
            if (test != 0) {
                return WPOSE_NORMAL_WEAK_THROW_UPWARDS;
            }
        }

        input = *(u16*)input;
        test = DPAD_DOWN;
        test &= input;
        if (test != 0) {
            return WPOSE_NORMAL_GROUND_POUND;
        }

        test = A_BUTTON;
        test &= input;
        wario = &gWarioData;
        if (test != 0) {
            wario->unk_07 = 1;
        }
    }

    {
        int temp;

        temp = gButtonsHeld;
        test = B_BUTTON | DPAD_UP;
        test &= temp;
        if (test == DPAD_UP) {
            tile = GetBackgroundCollisionAtPosition(wario->yPosition - 119, wario->xPosition) >> 16;
            if ((gCurrentCarriedSprite.state == 0) && (wario->yVelocity <= 0)) {
                if ((u32)(tile - 2) <= 1) {
                    return WPOSE_NORMAL_IDLE_ON_LADDER;
                }
                if (tile == 4) {
                    return WPOSE_NORMAL_IDLE_ON_FENCE;
                }
            }
        }
    }

    {
        u16* heldSource;
        struct WarioData* warioSource;
        u16 held;
        u16 direction;
        u16 movementResult;

        heldSource = &gButtonsHeld;
        warioSource = &gWarioData;
        held = *heldSource;
        direction = warioSource->horizontalDirection;
        value = held;
        value &= direction;
        heldPtr = heldSource;
        wario = warioSource;
        if (value != 0) {
            test = DPAD_RIGHT;
            test &= direction;
            if (test != 0) {
                u16 rawVelocity;
                s32 signedVelocity;

                rawVelocity = wario->xVelocity;
                signedVelocity = (s16)wario->xVelocity;
                if (signedVelocity <= 63) {
                    signedVelocity = rawVelocity + 6;
                    wario->xVelocity = signedVelocity;
                    if ((s16)signedVelocity > 64) {
                        wario->xVelocity = 64;
                    }
                }
            } else {
                u16 rawVelocity;
                s32 signedVelocity;
                s32 velocityLimit;

                rawVelocity = wario->xVelocity;
                signedVelocity = (s16)wario->xVelocity;
                velocityLimit = -64;
                if (signedVelocity > velocityLimit) {
                    signedVelocity = rawVelocity - 6;
                    wario->xVelocity = signedVelocity;
                    if ((s16)signedVelocity < velocityLimit) {
                        wario->xVelocity = velocityLimit;
                    }
                }
            }
        } else {
            u16 newDirection;

            test = DPAD_LEFT | DPAD_RIGHT;
            newDirection = direction;
            newDirection ^= test;
            /* agbcc otherwise folds the copy into the AND and overwrites held. */
            test = newDirection;
            test &= held;
            ASM_BARRIER(: "+r"(test));
            movementResult = (s16)test;
            if (movementResult != 0) {
                wario->horizontalDirection = newDirection;
                wario->xVelocity = value;
            } else {
                register u16 rawVelocity PIN("r0");
                s32 signedVelocity;

                rawVelocity = wario->xVelocity;
                signedVelocity = (s16)wario->xVelocity;
                if (signedVelocity > 0) {
                    rawVelocity -= 10;
                    wario->xVelocity = rawVelocity;
                    if ((s32)(rawVelocity << 16) < 0) {
                        wario->xVelocity = movementResult;
                    }
                } else if (signedVelocity < 0) {
                    rawVelocity += 10;
                    wario->xVelocity = rawVelocity;
                    if ((s32)(rawVelocity << 16) > 0) {
                        wario->xVelocity = movementResult;
                    }
                }
            }
        }
    }

    {
        u16 heldInput;
        int zero;

        heldInput = *heldPtr;
        zero = A_BUTTON;
        zero &= heldInput;
        if ((zero == 0) && (wario->yVelocity > 0)) {
            wario->yVelocity = zero;
        }
    }

    return 0xFF;
}


u8 UpdateNormalWarioCrouching(void)
{
    int collision;
    struct WarioData* wario;
    int value;
    u32 input;

    collision = ((int (*)(u8, u16, u8))CheckWarioVerticalCollision)(0, sWarioCollisionPointTable[2], 1);
    gCurrentCarriedSprite.state = 0;
    wario = &gWarioData;
    if (wario->unk_06 == 0) {
        input = gButtonsPressed;
        value = A_BUTTON;
        value &= input;
        if (value != 0) {
            wario->unk_08 = 1;
            return 0xFE;
        }
        value = B_BUTTON;
        value &= input;
        if ((value != 0) && (collision == 0)) {
            return WPOSE_NORMAL_SHOULDER_BASH;
        }
        input = gButtonsHeld;
        value = DPAD_DOWN;
        value &= input;
        if ((value == 0) && (collision == 0)) {
            return WPOSE_NORMAL_STANDING;
        }
    }

    {
        int held;
        u32 dir;

        value = (u32)&gButtonsHeld;
        dir = (u32)&gWarioData;
        held = *(u16*)value;
        dir = ((struct WarioData*)dir)->horizontalDirection;
        value = held;
        value &= dir;
        if (value != 0) {
            return WPOSE_NORMAL_CRAWLING;
        }
        value = DPAD_LEFT | DPAD_RIGHT;
        value ^= dir;
        value &= held;
        if (value != 0) {
            return WPOSE_NORMAL_CROUCH_JUMP;
        }
    }
    return 0xFF;
}

u8 UpdateNormalWarioCrouchSliding(void)
{
    struct WarioData* wario;
    register int one PIN("r3");
    int value;
    register int input PIN("r1");
    int velocity;

    ((int (*)(u8, u16, u8))CheckWarioVerticalCollision)(0, sWarioCollisionPointTable[2], 1);
    wario = &gWarioData;
    input = *(u16*)&wario->unk_1C;
    one = 1;
    value = one;
    value &= input;
    if (value != 0) {
        return WPOSE_NORMAL_STARTING_ROLL;
    }

    input = gButtonsPressed;
    value = one;
    value &= input;
    if (value != 0) {
        wario->unk_08 = 1;
        return 0xFE;
    }

    input = wario->horizontalDirection;
    value = DPAD_RIGHT;
    value &= input;
    if (value != 0) {
        velocity = *(u16*)&wario->xVelocity;
        velocity -= 2;
        wario->xVelocity = velocity;
        if ((s32)(velocity << 16) <= 0) {
            return WPOSE_NORMAL_CROUCHING;
        }
    } else {
        velocity = *(u16*)&wario->xVelocity;
        velocity += 2;
        wario->xVelocity = velocity;
        if ((s32)(velocity << 16) >= 0) {
            return WPOSE_NORMAL_CROUCHING;
        }
    }

    if (gWarioDustEffect1.unk0 == 0) {
        gWarioDustEffect1.unk0 = 5;
    }
    return 0xFF;
}


u8 UpdateNormalWarioCrawling(void)
{
    int value;

    {
        register struct WarioData* wario PIN("r4");
        int collision;
        register int one PIN("r2");
        register int input PIN("r1");

        wario = &gWarioData;
        if (wario->unk_06 == 0) {
            collision = ((int (*)(u8, u16, u8))CheckWarioVerticalCollision)(0, sWarioCollisionPointTable[2], 1);
            input = *(u16*)&wario->unk_1C;
            one = 1;
            value = one;
            value &= input;
            if (value != 0) {
                return WPOSE_NORMAL_STARTING_ROLL;
            }
            input = gButtonsPressed;
            value = one;
            value &= input;
            if (value != 0) {
                wario->unk_08 = 1;
                return 0xFE;
            }
            value = B_BUTTON;
            value &= input;
            if ((value != 0) && (collision == 0)) {
                return WPOSE_NORMAL_SHOULDER_BASH;
            }
            input = gButtonsHeld;
            value = DPAD_DOWN;
            value &= input;
            if ((value == 0) && (collision == 0)) {
                return WPOSE_NORMAL_STANDING;
            }
        }
    }

    {
        register int held PIN("r4");
        register int direction PIN("r2");
        struct WarioData* wario;
        u32 heldAddress;
        u32 warioAddress;
        int velocity;

        heldAddress = (u32)&gButtonsHeld;
        warioAddress = (u32)&gWarioData;
        held = *(u16*)heldAddress;
        direction = ((struct WarioData*)warioAddress)->horizontalDirection;
        value = held;
        value &= direction;
        wario = (struct WarioData*)warioAddress;
        if (value != 0) {
            value = DPAD_RIGHT;
            value &= direction;
            if (value != 0) {
                velocity = *(u16*)&wario->xVelocity;
                velocity += 6;
                wario->xVelocity = velocity;
                if ((s16)velocity > 32) {
                    wario->xVelocity = 32;
                }
            } else {
                velocity = *(u16*)&wario->xVelocity;
                velocity -= 6;
                wario->xVelocity = velocity;
                if ((s16)velocity < -32) {
                    wario->xVelocity = -32;
                }
            }
        } else {
            value = DPAD_LEFT | DPAD_RIGHT;
            value ^= direction;
            value &= held;
            value <<= 16;
            if (value != 0) {
                return WPOSE_NORMAL_CROUCH_JUMP;
            }
            return WPOSE_NORMAL_CROUCHING;
        }

        {
            const struct WarioAnimationFrame* animation;
            int frame;

            animation = sWarioCrawlingAnimation;
            if (wario->unk_1E >= animation[wario->unk_1F].time) {
                wario->unk_1E = 0;
                wario->unk_1F++;
                frame = wario->unk_1F;
                if (animation[frame].time == 0) {
                    wario->unk_1F = 0;
                    gWarioDustEffect1.unk0 = 5;
                } else {
                    register int soundFrame PIN("r0");

                    soundFrame = frame;
                    if ((soundFrame == 1) || (soundFrame == 4)) {
                        m4aSongNumStart(2);
                    }
                }
            }
        }
    }

    return 0xFF;
}

u8 UpdateNormalWarioTurningCrouched(void)
{
    int zero;
    register int value PIN("r0");

    gCurrentCarriedSprite.state = 0;
    {
        struct WarioData* wario;
        register u16* buttons PIN("r3");
        register int input PIN("r1");

        int yVelocity;

        wario = &gWarioData;
        zero = wario->unk_09;
        if (zero != 0) {
            return 0xFF;
        }
        {
            yVelocity = wario->yVelocity;
            buttons = &gButtonsHeld;
            if (yVelocity > 0) {
                input = *buttons;
                value = A_BUTTON;
                value &= input;
                if ((value == 0) || (wario->unk_06 != 0)) {
                    wario->yVelocity = zero;
                }
            }

            input = *buttons;
            value = DPAD_DOWN;
            value &= input;
            if (value == 0) {
                const u16* collisionData;

                collisionData = sWarioCollisionPointTable;
                if (((int (*)(u8, u16, u8))CheckWarioVerticalCollision)(0, collisionData[5], 1) == 0) {
                    if (((int (*)(u8, u16, u8))CheckWarioVerticalCollision)(0, collisionData[8], 1) == 0) {
                        return WPOSE_NORMAL_FALLING;
                    }
                }
            }
        }
    }

    {
        register int held PIN("r3");
        register int direction PIN("r1");
        struct WarioData* wario;
        int velocity;

        value = (u32)&gButtonsHeld;
        wario = &gWarioData;
        held = *(u16*)value;
        direction = wario->horizontalDirection;
        zero = held;
        zero &= direction;
        if (zero != 0) {
            value = DPAD_RIGHT;
            value &= direction;
            if (value != 0) {
                velocity = *(u16*)&wario->xVelocity;
                velocity += 6;
                wario->xVelocity = velocity;
                if ((s16)velocity > 32) {
                    wario->xVelocity = 32;
                }
            } else {
                velocity = *(u16*)&wario->xVelocity;
                velocity -= 6;
                wario->xVelocity = velocity;
                if ((s16)velocity < -32) {
                    wario->xVelocity = -32;
                }
            }
        } else {
            value = DPAD_LEFT | DPAD_RIGHT;
            direction ^= value;
            value = direction;
            value &= held;
            value <<= 16;
            if (value != 0) {
                wario->horizontalDirection = direction;
            }
            wario->xVelocity = zero;
        }
    }

    return 0xFF;
}


u8 UpdateNormalWarioCrouchJump(void)
{
    register u32 value PIN("r0");

    gCurrentCarriedSprite.state = 0;
    {
        struct WarioData* wario;
        int input;

        wario = &gWarioData;
        if (wario->unk_06 == 0) {
            input = gButtonsPressed;
            value = A_BUTTON;
            value &= input;
            if (value != 0) {
                wario->unk_08 = 1;
                return 0xFE;
            }

            value = B_BUTTON;
            value &= input;
            if (value != 0) {
                if (((int (*)(u8, u16, u8))CheckWarioVerticalCollision)(0, sWarioCollisionPointTable[2], 1) == 0) {
                    return WPOSE_NORMAL_SHOULDER_BASH;
                }
            }
        }
    }

    {
        struct WarioData* wario;
        const struct WarioAnimationFrame* animation;
        int frame;

        wario = &gWarioData;
        animation = sWarioCrouchJumpAnimation;
        value = wario->unk_1F;
        if (wario->unk_1E >= animation[value].time) {
            wario->unk_1E = 0;
            wario->unk_1F++;
            frame = wario->unk_1F;
            if (animation[frame].time == 0) {
                return WPOSE_NORMAL_CROUCHING;
            }
        }
    }

    return 0xFF;
}

u8 UpdateNormalWarioSlidingOnIce(void)
{
    int acceleration;
    register int mask PIN("r5");
    int input;
    input = gButtonsPressed;
    if (input & A_BUTTON) {
        return WPOSE_NORMAL_JUMPING;
    }
    if (input & B_BUTTON) {
        if (gCurrentCarriedSprite.state != 0) {
            return WPOSE_NORMAL_CHARGING_THROW_1;
        }
        return WPOSE_NORMAL_SHOULDER_BASH;
    }

    {
        struct WarioData* wario;
        register int value PIN("r0");

        wario = &gWarioData;
        value = wario->unk_0A;
        if (value != 0) {
            value--;
            wario->unk_0A = value;
            if ((value << 24) == 0) {
                return WPOSE_NORMAL_TURNING;
            }
        } else {
            value = wario->horizontalDirection;
            value ^= DPAD_LEFT | DPAD_RIGHT;
            value &= input;
            if (value != 0) {
                wario->unk_0A = 14;
            }
        }
    }

    {
        struct WarioData* wario;
        register int value PIN("r0");

        wario = &gWarioData;
        acceleration = GetBackgroundCollisionAtPosition(wario->yPosition + 1, wario->xPosition - 30);
        acceleration >>= 16;
        mask = 0xFF;
        acceleration &= mask;
        value = GetBackgroundCollisionAtPosition(wario->yPosition + 1, wario->xPosition + 30);
        value = (s32)value >> 16;
        value &= mask;
        if ((acceleration == 11) || (value == 11)) {
            acceleration = 2;
            value = wario->unk_0C;
            if (value == 0) {
                value++;
                wario->unk_0C = value;
                m4aSongNumStart(11);
            }
        } else {
            acceleration = 32;
        }
    }

    {
        struct WarioData* wario;
        int velocity;

        wario = &gWarioData;
        if (wario->horizontalDirection & DPAD_RIGHT) {
            velocity = *(u16*)&wario->xVelocity;
            velocity -= acceleration;
            wario->xVelocity = velocity;
            if ((s32)(velocity << 16) > 0) {
                goto velocity_continues;
            }
        } else {
            velocity = *(u16*)&wario->xVelocity;
            velocity += acceleration;
            wario->xVelocity = velocity;
            if ((s32)(velocity << 16) < 0) {
                goto velocity_continues;
            }
        }

        if (wario->unk_0A != 0) {
            return WPOSE_NORMAL_TURNING;
        }
        return WPOSE_NORMAL_STANDING;

velocity_continues:
        return 0xFF;
    }
}


u8 UpdateNormalWarioStoppingFromDash(void)
{
    int acceleration;
    register int mask PIN("r5");
    register int value PIN("r0");

    {
        struct WarioData* wario;

        wario = &gWarioData;
        acceleration = GetBackgroundCollisionAtPosition(wario->yPosition + 1, wario->xPosition - 30);
        acceleration >>= 16;
        mask = 0xFF;
        acceleration &= mask;
        value = GetBackgroundCollisionAtPosition(wario->yPosition + 1, wario->xPosition + 30);
        value >>= 16;
        value &= mask;
        if ((acceleration == 11) || (value == 11)) {
            acceleration = 1;
            value = wario->unk_0C;
            if (value == 0) {
                value++;
                wario->unk_0C = value;
                m4aSongNumStart(11);
            }
        } else {
            acceleration = 32;
            value = wario->unk_0C;
            if (value == 0) {
                value++;
                wario->unk_0C = value;
                m4aSongNumStart(9);
            }
        }
    }

    {
        register struct WarioData* wario PIN("r2");
        wario = &gWarioData;
        if (wario->horizontalDirection & DPAD_RIGHT) {
            int velocity;

            velocity = *(u16*)&wario->xVelocity;
            value = wario->xVelocity;
            if (value > 48) {
                value = velocity - 3;
            } else {
                value = velocity - acceleration;
            }
            wario->xVelocity = value;
            if (wario->xVelocity <= 0) {
                return WPOSE_NORMAL_STANDING;
            }
        } else {
            int velocity;
            int signedVelocity;

            velocity = *(u16*)&wario->xVelocity;
            signedVelocity = wario->xVelocity;
            if (signedVelocity < -48) {
                value = velocity + 3;
            } else {
                value = velocity + acceleration;
            }
            wario->xVelocity = value;
            if (wario->xVelocity >= 0) {
                return WPOSE_NORMAL_STANDING;
            }
        }

        {
            const struct WarioAnimationFrame* animation;

            int frame;

            animation = sWarioStoppingFromDashAnimation;
            value = wario->unk_1F;
            if (wario->unk_1E >= animation[value].time) {
                wario->unk_1E = 0;
                wario->unk_1F++;
                frame = wario->unk_1F;
                if (animation[frame].time == 0) {
                    wario->unk_1F = 0;
                    gWarioDustEffect1.unk0 = 5;
                }
            }
        }
    }

    return 0xFF;
}


u8 UpdateNormalWarioStartingRoll(void)
{
    register u32 value PIN("r0");

    {
        int input;

        input = gButtonsPressed;
        value = A_BUTTON;
        value &= input;
        if (value != 0) {
            struct WarioData* wario;

            wario = &gWarioData;
            value = 1;
            wario->unk_08 = value;
            return 0xFE;
        }
    }

    {
        struct WarioData* wario;
        int direction;

        wario = &gWarioData;
        direction = wario->horizontalDirection;
        value = DPAD_RIGHT;
        value &= direction;
        if (value != 0) {
            value = *(u16*)&wario->xVelocity;
            value += 6;
            wario->xVelocity = value;
            if ((s16)value > 127) {
                value = 128;
                wario->xVelocity = value;
                return WPOSE_NORMAL_JUMPING_OUT_OF_ROLL;
            }
        } else {
            value = *(u16*)&wario->xVelocity;
            value -= 6;
            wario->xVelocity = value;
            if ((s16)value <= -128) {
                direction = -128;
                wario->xVelocity = direction;
                return WPOSE_NORMAL_JUMPING_OUT_OF_ROLL;
            }
        }

        {
            const struct WarioAnimationFrame* animation;
            int frame;

            animation = sWarioStartingRollAnimation;
            value = wario->unk_1F;
            if (wario->unk_1E >= animation[value].time) {
                wario->unk_1E = 0;
                wario->unk_1F++;
                frame = wario->unk_1F;
                if (animation[frame].time == 0) {
                    wario->unk_1F = 0;
                    gWarioDustEffect1.unk0 = 5;
                }
            }
        }
    }

    return 0xFF;
}


u8 UpdateNormalWarioJumpingOutOfRoll(void)
{
    struct WarioData* wario;
    const struct WarioAnimationFrame* animation;
    u32 value;
    int frame;

    wario = &gWarioData;
    animation = sWarioJumpingOutOfRollAnimation;
    value = wario->unk_1F;
    if (wario->unk_1E >= animation[value].time) {
        wario->unk_1E = 0;
        wario->unk_1F++;
        frame = wario->unk_1F;
        if (animation[frame].time == 0) {
            return WPOSE_NORMAL_ROLLING;
        }
    }
    return 0xFF;
}

u8 UpdateNormalWarioRolling(void)
{
    register u32 value PIN("r0");
    struct WarioData* wario;
    register struct WarioData* wario2 PIN("r2");
    int zero;
    const struct WarioAnimationFrame* animation;
    int frame;

    {
        int input;

        input = gButtonsPressed;
        value = A_BUTTON;
        value &= input;
    }
    wario = &gWarioData;
    if (value != 0) {
        goto request_pose;
    }
    wario2 = wario;
    zero = wario2->unk_07;
    if (zero == 0) {
        goto update_animation;
    }

request_pose:
    value = 1;
    wario->unk_08 = value;
    return 0xFE;

update_animation:
    animation = sWarioRollingAnimation;
    value = wario2->unk_1F;
    if (wario2->unk_1E >= animation[value].time) {
        wario2->unk_1E = zero;
        wario2->unk_1F++;
        frame = wario2->unk_1F;
        if (animation[frame].time == 0) {
            wario2->unk_1F = 0;
            gWarioDustEffect1.unk0 = 5;
        }
    }
    return 0xFF;
}

u8 UpdateNormalWarioRollingJump(void)
{
    register u32 value PIN("r0");
    struct WarioData* wario;
    const struct WarioAnimationFrame* animation;
    int frame;

    {
        int input;

        input = gButtonsPressed;
        value = A_BUTTON;
        value &= input;
    }
    wario = &gWarioData;
    if (value != 0) {
        value = 1;
        wario->unk_07 = value;
    }

    animation = sWarioRollingAnimation;
    value = wario->unk_1F;
    if (wario->unk_1E >= animation[value].time) {
        wario->unk_1E = 0;
        wario->unk_1F++;
        frame = wario->unk_1F;
        if (animation[frame].time == 0) {
            wario->unk_1F = 0;
        }
    }
    return 0xFF;
}

u8 UpdateNormalWarioStunned(void)
{
    u32 value;
    struct WarioData* wario;

    gCurrentCarriedSprite.unk1 = 0;
    gCurrentCarriedSprite.state = 0;
    wario = &gWarioData;
    if (wario->unk_1E > 59) {
        goto request_pose;
    }

    {
        register int velocity PIN("r1");

        velocity = wario->yVelocity;
        value = -120;
        if (velocity >= (s32)value) {
            goto no_change;
        }
    }

    {
        int input;

        input = gButtonsPressed;
        value = DPAD_UP;
        value &= input;
        if (value != 0) {
            goto request_pose;
        }
        input = gButtonsHeld;
        value = DPAD_LEFT | DPAD_RIGHT;
        value |= input;
        if (value == 0) {
            goto no_change;
        }
    }

request_pose:
    value = 2;
    wario->unk_08 = value;
    return 0xFE;

no_change:
    return 0xFF;
}

u8 UpdateNormalWarioEnteringPipe(void)
{
    struct WarioData* wario;
    const struct WarioAnimationFrame* animation;
    u32 value;
    int frame;

    wario = &gWarioData;
    animation = sWarioEnteringPipeAnimation;
    value = wario->unk_1F;
    if (wario->unk_1E >= animation[value].time) {
        wario->unk_1E = 0;
        wario->unk_1F++;
        frame = wario->unk_1F;
        if (animation[frame].time == 0) {
            if (wario->pose == WPOSE_NORMAL_ENTERING_PIPE_UP) {
                return WPOSE_NORMAL_STANDING;
            }
            return 0xFE;
        }
    }
    return 0xFF;
}

u8 UpdateNormalWarioTakingDamage(void)
{
    struct WarioData* wario;
    const struct WarioAnimationFrame* animation;
    u32 value;
    int frame;

    wario = &gWarioData;
    animation = sWarioTakingDamageAnimation;
    value = wario->unk_1F;
    if (wario->unk_1E >= animation[value].time) {
        wario->unk_1E = 0;
        wario->unk_1F++;
        frame = wario->unk_1F;
        if (animation[frame].time == 0) {
            wario->unk_1F = 0;
        }
    }
    return 0xFF;
}

u8 UpdateNormalWarioRollingMidair(void)
{
    struct WarioData* wario;
    const struct WarioAnimationFrame* animation;
    u32 value;
    int frame;

    wario = &gWarioData;
    animation = sWarioRollingMidairAnimation;
    value = wario->unk_1F;
    if (wario->unk_1E >= animation[value].time) {
        wario->unk_1E = 0;
        wario->unk_1F++;
        frame = wario->unk_1F;
        if (animation[frame].time == 0) {
            wario->unk_1F = frame - 1;
        }
    }
    return 0xFF;
}


u8 UpdateNormalWarioShoulderBash(void)
{
    struct WarioData* wario;
    int acceleration;
    int mask;
    register u32 value PIN("r0");
    int input;
    register int one PIN("r2");

    {
        struct WarioData* base;

        base = &gWarioData;
        input = base->unk_0A;
        wario = base;
    }

    if ((u32)input <= 45) {
        value = input + 1;
        wario->unk_0A = value;
        input = gButtonsPressed;
        one = 1;
        value = one;
        value &= input;
        if (value != 0) {
            wario->unk_08 = one;
            return 0xFE;
        }
    } else {
        input = gButtonsHeld;
        value = wario->horizontalDirection;
        value &= input;
        if (value != 0) {
            return WPOSE_NORMAL_WALKING;
        }

        value = GetBackgroundCollisionAtPosition(wario->yPosition + 1, wario->xPosition - 30);
        acceleration = value;
        acceleration >>= 16;
        mask = 0xFF;
        acceleration &= mask;
        value = GetBackgroundCollisionAtPosition(wario->yPosition + 1, wario->xPosition + 30);
        value = (s32)value >> 16;
        value &= mask;
        if ((acceleration == 11) || (value == 11)) {
            input = wario->xVelocity;
            value = (s32)((s32)input * 3) >> 2;
            wario->xVelocity = value;
            return WPOSE_NORMAL_SLIDING_ON_ICE;
        }
        return WPOSE_NORMAL_STANDING;
    }

    value = DPAD_DOWN;
    value &= input;
    if (value != 0) {
        return WPOSE_NORMAL_CROUCH_SLIDE;
    }

    {
        u16* heldPointer;
        int direction;

        heldPointer = &gButtonsHeld;
        direction = wario->horizontalDirection;
        value = DPAD_LEFT | DPAD_RIGHT;
        value ^= direction;
        input = *heldPointer;
        value &= input;
        if (value != 0) {
            return WPOSE_NORMAL_TURNING;
        }

        value = DPAD_RIGHT;
        value &= direction;
        if (value != 0) {
            value = *(u16*)&wario->xVelocity;
            value += 4;
            wario->xVelocity = value;
            if ((s16)value > 96) {
                value = 96;
                wario->xVelocity = value;
            }
        } else {
            value = *(u16*)&wario->xVelocity;
            value -= 4;
            wario->xVelocity = value;
            if ((s16)value < -96) {
                input = -96;
                wario->xVelocity = input;
            }
        }
    }

    {
        const struct WarioAnimationFrame* animation;
        int frame;

        animation = sWarioShoulderBashAnimation;
        value = wario->unk_1F;
        if (wario->unk_1E >= animation[value].time) {
            wario->unk_1E = 0;
            wario->unk_1F++;
            frame = wario->unk_1F;
            if (animation[frame].time == 0) {
                wario->unk_1F = 0;
            } else if (frame == 1) {
                gWarioDustEffect1.unk0 = 5;
            }
        }
    }
    return 0xFF;
}


u8 UpdateNormalWarioDashAttack(void)
{
    int zero;
    register u32 value PIN("r0");

    value = gButtonsPressed;
    zero = A_BUTTON;
    zero &= value;
    if (zero != 0) {
        struct WarioData* wario;

        wario = &gWarioData;
        value = 1;
        wario->unk_08 = value;
        return 0xFE;
    }

    {
        int held;
        struct WarioData* wario;
        int direction;
        int velocity;

        value = (u32)&gButtonsHeld;
        wario = &gWarioData;
        held = *(u16*)value;
        direction = wario->horizontalDirection;
        value = held;
        value &= direction;
        if (value == 0) {
            value = DPAD_RIGHT;
            value &= direction;
            if (value != 0) {
                velocity = *(u16*)&wario->xVelocity;
                velocity -= 3;
            } else {
                velocity = *(u16*)&wario->xVelocity;
                velocity += 3;
            }
            wario->xVelocity = velocity;
            return WPOSE_NORMAL_STOPPING_FROM_DASH;
        }

        value = 0x300;
        value &= held;
        if (value == 0) {
            value = DPAD_RIGHT;
            value &= direction;
            if (value != 0) {
                velocity = *(u16*)&wario->xVelocity;
                velocity -= 6;
            } else {
                velocity = *(u16*)&wario->xVelocity;
                velocity += 6;
            }
            wario->xVelocity = velocity;
            return WPOSE_NORMAL_WALKING;
        }

        {
            const struct WarioAnimationFrame* animation;
            int frame;

            animation = sWarioDashAttackAnimation;
            value = wario->unk_1F;
            if (wario->unk_1E >= animation[value].time) {
                wario->unk_1E = zero;
                wario->unk_1F++;
                frame = wario->unk_1F;
                if (animation[frame].time == 0) {
                    wario->unk_1F = 0;
                } else if (frame == 1) {
                    gWarioDustEffect1.unk0 = 5;
                }
            }
        }
    }
    return 0xFF;
}


u8 UpdateNormalWarioShoulderBashJump(void)
{
    register u32 value PIN("r0");
    struct WarioData* wario;

    {
        int input;

        input = gButtonsPressed;
        value = A_BUTTON;
        value &= input;
    }
    wario = &gWarioData;
    if (value != 0) {
        value = 1;
        wario->unk_07 = value;
    }

    {
        int direction;

        direction = wario->horizontalDirection;
        value = DPAD_RIGHT;
        value &= direction;
        if (value != 0) {
            value = *(u16*)&wario->xVelocity;
            value += 10;
            wario->xVelocity = value;
            if ((s16)value > 96) {
                value = 96;
                wario->xVelocity = value;
            }
        } else {
            value = *(u16*)&wario->xVelocity;
            value -= 10;
            wario->xVelocity = value;
            if ((s16)value < -96) {
                direction = -96;
                wario->xVelocity = direction;
            }
        }
    }

    {
        u16* heldPointer;
        register struct WarioData* current PIN("r3");
        register int direction PIN("r1");
        register int held PIN("r0");
        int opposite;

        heldPointer = &gButtonsHeld;
        current = wario;
        direction = current->horizontalDirection;
        direction ^= DPAD_LEFT | DPAD_RIGHT;
        held = *heldPointer;
        opposite = direction;
        opposite &= held;
        if (opposite != 0) {
            current->horizontalDirection = direction;
            value = current->yVelocity;
            if ((s32)value < 0) {
                value = 2;
                current->unk_08 = value;
            }
            return 0xFE;
        }

        {
            const struct WarioAnimationFrame* animation;
            int frame;

            animation = sWarioShoulderBashJumpAnimation;
            value = wario->unk_1F;
            if (wario->unk_1E >= animation[value].time) {
                wario->unk_1E = opposite;
                wario->unk_1F++;
                frame = wario->unk_1F;
                if (animation[frame].time == 0) {
                    wario->unk_1F = 0;
                }
            }
        }
    }
    return 0xFF;
}


u8 UpdateNormalWarioDashAttackJump(void)
{
    struct WarioData* wario;
    const struct WarioAnimationFrame* animation;
    int frame;

    wario = &gWarioData;
    animation = sWarioDashAttackAnimation;
    frame = wario->unk_1F;
    if (wario->unk_1E >= animation[frame].time) {
        wario->unk_1E = 0;
        wario->unk_1F++;
        {
            int nextFrame;

            nextFrame = wario->unk_1F;
            if (animation[nextFrame].time == 0) {
                wario->unk_1F = 0;
            }
        }
    }
    return 0xFF;
}

u8 UpdateNormalWarioShoulderBashBonk(void)
{
    struct WarioData* wario;
    int held;
    u32 buttons;

    wario = &gWarioData;
    if (wario->unk_1E > 59) {
        goto transition;
    }
    if (wario->yVelocity >= -120) {
        goto noTransition;
    }
    buttons = gButtonsHeld;
    held = DPAD_LEFT | DPAD_RIGHT;
    held &= buttons;
    if (held == 0) {
        goto noTransition;
    }
    wario->horizontalDirection = held;

transition:
    wario->unk_08 = 2;
    return 0xFE;

noTransition:
    return 0xFF;
}


u8 UpdateNormalWarioGroundPound(void)
{
    struct WarioData* wario;
    u32 value;
    register u32 held PIN("r1");
    register u32 direction PIN("r2");
    register u32 zero PIN("r3");

    {
        struct WarioData* base;
        int offset;
        int velocity;

        base = &gWarioData;
        offset = 24;
        velocity = *(s16*)((u8*)base + offset);
        wario = base;
        if (velocity > 0) {
            value = 0;
            wario->yVelocity = value;
        }
    }

    if (wario->pose == 27) {
        held = gButtonsHeld;
        value = DPAD_DOWN;
        value &= held;
        if (value == 0) {
            wario->unk_08 = 2;
            return 0xFE;
        }
        value = wario->unk_0A;
        if (value > 23) {
            return 28;
        }
        value++;
        wario->unk_0A = value;
    }

    held = gButtonsHeld;
    direction = wario->horizontalDirection;
    zero = held;
    zero &= direction;
    if (zero != 0) {
        value = DPAD_RIGHT;
        value &= direction;
        if (value != 0) {
            wario->xVelocity = 32;
        } else {
            wario->xVelocity = -32;
        }
    } else {
        value = DPAD_LEFT | DPAD_RIGHT;
        direction ^= value;
        value = direction;
        value &= held;
        value <<= 16;
        if (value != 0) {
            wario->horizontalDirection = direction;
        }
        wario->xVelocity = zero;
    }

    {
        register struct WarioData* animationWario PIN("r2");
        const struct WarioAnimationFrame* animation;
        u32 frame;
        u32 nextFrame;

        animationWario = wario;
        animation = sWarioGroundPoundAnimation;
        frame = animationWario->unk_1F;
        if (animationWario->unk_1E >= animation[frame].time) {
            animationWario->unk_1E = 0;
            animationWario->unk_1F++;
            nextFrame = animationWario->unk_1F;
            if (animation[nextFrame].time == 0) {
                animationWario->unk_1F = 1;
            } else if ((wario->pose == 27) && (nextFrame == 1)) {
                m4aSongNumStart(0x21);
            }

            if (gWarioData.unk_1F != 0) {
                gWarioMotionAfterimage.unk0 = 2;
            }
        }
    }

    if (gWarioMotionAfterimage.unk0 != 0) {
        struct WarioAfterimage* afterimage;
        const struct WarioEffectFrame* animation;
        u32 frame;
        u32 nextFrame;

        afterimage = &gWarioMotionAfterimage;
        animation = sWarioAfterimageFrames;
        frame = afterimage->unk2;
        if (afterimage->unk1 >= animation[frame].time) {
            afterimage->unk1 = 0;
            afterimage->unk2++;
            nextFrame = afterimage->unk2;
            if (animation[nextFrame].time == 0) {
                afterimage->unk2 = nextFrame - 1;
            }
        }
    }

    return 0xFF;
}


u8 UpdateNormalWarioGroundPoundLanding(void)
{
    struct WarioData* wario;
    register const struct WarioAnimationFrame* animation PIN("r3");
    u32 frame;

    wario = &gWarioData;
    if (wario->pose == 29) {
        animation = sWarioGroundPoundLandingAnimation;
    } else {
        animation = sWarioSuperGroundPoundLandingAnimation;
    }

    frame = wario->unk_1F;
    if (wario->unk_1E >= animation[frame].time) {
        wario->unk_1E = 0;
        wario->unk_1F++;
        {
            u32 nextFrame;

            nextFrame = wario->unk_1F;
            if (animation[nextFrame].time == 0) {
                return 2;
            }
        }
    }

    {
        struct WarioAfterimage* afterimage;
        const struct WarioEffectFrame* afterimageAnimation;
        u32 afterimageFrame;

        afterimage = &gWarioMotionAfterimage;
        afterimageAnimation = sWarioGroundPoundLandingAfterimageFrames;
        afterimageFrame = afterimage->unk2;
        if (afterimage->unk1 >= afterimageAnimation[afterimageFrame].time) {
            afterimage->unk1 = 0;
            afterimage->unk2++;
            afterimageFrame = afterimage->unk2;
            if (afterimageAnimation[afterimageFrame].time == 0) {
                afterimage->unk2 = 0;
            }
        }
    }
    return 0xFF;
}

u8 UpdateNormalWarioLandingOnEnemy(void)
{
    struct WarioData* wario;
    u32 value;
    u32 input;

    wario = &gWarioData;
    if (wario->unk_1E > 39) {
        goto transition;
    }
    if (wario->yVelocity >= -120) {
        goto noTransition;
    }
    value = (u32)&gButtonsPressed;
    input = *(u16*)value;
    value = DPAD_UP;
    value &= input;
    if (value != 0) {
        goto transition;
    }
    value = (u32)&gButtonsHeld;
    input = *(u16*)value;
    value = DPAD_LEFT | DPAD_RIGHT;
    value &= input;
    if (value == 0) {
        goto noTransition;
    }

transition:
    wario->unk_08 = 2;
    return 0xFE;

noTransition:
    return 0xFF;
}


u8 UpdateNormalWarioLiftingSprite(void)
{
    struct CarriedSprite* carried;
    register u32 value PIN("r0");
    u32 zero;

    carried = &gCurrentCarriedSprite;
    if (carried->state == 0) {
        return 2;
    }

    value = gButtonsHeld;
    zero = A_BUTTON;
    zero &= value;
    if (zero != 0) {
        struct WarioData* wario;
        u32 frame;

        value = (u32)&gWarioData;
        frame = ((struct WarioData*)value)->unk_1F;
        wario = (struct WarioData*)value;
        if (frame <= 1) {
            value = 0;
        } else {
            value = 4;
        }
        carried->state = value;
        value = 1;
        wario->unk_08 = value;
        return 0xFE;
    }

    {
        const u8* states;
        struct WarioData* wario;

        states = sWarioLiftingSpriteStateTable;
        wario = &gWarioData;
        value = wario->unk_1F;
        value += (u32)states;
        carried->state = *(u8*)value;
        value = wario->unk_1E;
        if (value > 2) {
            wario->unk_1E = zero;
            value = wario->unk_1F;
            value++;
            wario->unk_1F = value;
            {
                const struct WarioAnimationFrame* animation;
                u32 frame;

                animation = sWarioLiftingSpriteAnimation;
                frame = wario->unk_1F;
                if (animation[frame].time == 0) {
                    return 2;
                }
            }
        }
    }
    return 0xFF;
}


u8 UpdateNormalWarioLiftingHeavySprite(void)
{
    struct CarriedSprite* carried;
    register u32 value PIN("r0");
    u32 zero;

    carried = &gCurrentCarriedSprite;
    if (carried->state == 0) {
        return 2;
    }

    value = gButtonsPressed;
    zero = A_BUTTON;
    zero &= value;
    if (zero != 0) {
        struct WarioData* wario;
        u32 frame;

        value = (u32)&gWarioData;
        frame = ((struct WarioData*)value)->unk_1F;
        wario = (struct WarioData*)value;
        if (frame <= 2) {
            value = 0;
        } else {
            value = 4;
        }
        carried->state = value;
        value = 1;
        wario->unk_08 = value;
        return 0xFE;
    }

    {
        const u8* states;
        struct WarioData* wario;

        states = sWarioLiftingHeavySpriteStateTable;
        wario = &gWarioData;
        value = wario->unk_1F;
        value += (u32)states;
        carried->state = *(u8*)value;
        value = wario->unk_1E;
        if (value > 3) {
            wario->unk_1E = zero;
            value = wario->unk_1F;
            value++;
            wario->unk_1F = value;
            {
                const struct WarioAnimationFrame* animation;
                u32 frame;

                animation = sWarioLiftingHeavySpriteAnimation;
                frame = wario->unk_1F;
                if (animation[frame].time == 0) {
                    return 2;
                }
            }
        }
    }
    return 0xFF;
}


u8 UpdateNormalWarioChargingThrow1(void)
{
    struct CarriedSprite* carried;
    register u32 value PIN("r0");
    u32 input;
    u16* heldPointer;

    {
        struct CarriedSprite* base;

        base = &gCurrentCarriedSprite;
        input = base->state;
        carried = base;
        if (input == 0) {
            return 2;
        }
    }

    heldPointer = &gButtonsHeld;
    input = *heldPointer;
    value = DPAD_UP;
    value &= input;
    if (value != 0) {
        input = carried->unk1;
        value = 0x80;
        value |= input;
    } else {
        input = carried->unk1;
        value = 0x0F;
        value &= input;
    }
    carried->unk1 = value;

    input = *heldPointer;
    value = B_BUTTON;
    value &= input;
    if (value == 0) {
        value = gWarioData.unk_1F;
        if (value != 0) {
            return 37;
        }
        return 36;
    }

    {
        const u8* states;
        struct WarioData* wario;
        const struct WarioAnimationFrame* animation;

        states = sWarioChargingThrowStateTable;
        wario = &gWarioData;
        value = wario->unk_1F;
        value += (u32)states;
        carried->state = *(u8*)value;
        animation = sWarioChargingThrowAnimation;
        {
            u32 frame;

            frame = wario->unk_1F;
            value = frame << 1;
            value += frame;
            value <<= 2;
            value += (u32)animation;
            frame = wario->unk_1E;
            value = *(u8*)(value + 8);
            if (frame >= value) {
                value = 0;
                wario->unk_1E = value;
                value = wario->unk_1F;
                value++;
                wario->unk_1F = value;
                frame = wario->unk_1F;
                value = frame << 1;
                value += frame;
                value <<= 2;
                value += (u32)animation;
                value = *(u8*)(value + 8);
                if (value == 0) {
                    return 35;
                }
            }
        }
    }
    return 0xFF;
}

u8 UpdateNormalWarioChargingThrow2(void)
{
    struct CarriedSprite* carried;
    register u32 value PIN("r0");
    u32 input;

    carried = &gCurrentCarriedSprite;
    if (carried->state == 0) {
        return 2;
    }

    value = (u32)&gButtonsPressed;
    input = *(u16*)value;
    value = A_BUTTON;
    value &= input;
    if (value != 0) {
        carried->state = 0;
        {
            struct WarioData* wario;

            wario = &gWarioData;
            value = 1;
            wario->unk_08 = value;
        }
        return 0xFE;
    }

    value = DPAD_DOWN;
    value &= input;
    if (value != 0) {
        return 5;
    }

    {
        register u16* heldPointer PIN("r2");
        u32 held;

        heldPointer = &gButtonsHeld;
        input = *heldPointer;
        value = DPAD_UP;
        value &= input;
        if (value != 0) {
            input = carried->unk1;
            value = 0x80;
            value |= input;
        } else {
            input = carried->unk1;
            value = 0x0F;
            value &= input;
        }
        carried->unk1 = value;

        held = *heldPointer;
        value = B_BUTTON;
        value &= held;
        if (value == 0) {
            return 37;
        }

        value = gWarioData.horizontalDirection;
        input = DPAD_LEFT | DPAD_RIGHT;
        value ^= input;
        value &= held;
        if (value != 0) {
            return 39;
        }
    }
    return 0xFF;
}


u8 UpdateNormalWarioWeakThrow(void)
{
    struct CarriedSprite* carried;
    const u8* states;
    struct WarioData* wario;
    register u32 value PIN("r0");

    carried = &gCurrentCarriedSprite;
    states = sWarioWeakThrowStateTable;
    wario = &gWarioData;
    value = wario->unk_1F;
    value += (u32)states;
    carried->state = *(u8*)value;

    {
        const struct WarioAnimationFrame* animation;
        u32 frame;

        animation = sWarioWeakThrowAnimation;
        frame = wario->unk_1F;
        value = frame << 1;
        value += frame;
        value <<= 2;
        value += (u32)animation;
        frame = wario->unk_1E;
        value = *(u8*)(value + 8);
        if (frame >= value) {
            value = 0;
            wario->unk_1E = value;
            value = wario->unk_1F;
            value++;
            wario->unk_1F = value;
            frame = wario->unk_1F;
            value = frame << 1;
            value += frame;
            value <<= 2;
            value += (u32)animation;
            value = *(u8*)(value + 8);
            if (value == 0) {
                value = (u32)&gButtonsHeld;
                frame = *(u16*)value;
                value = DPAD_UP;
                value &= frame;
                if (value != 0) {
                    return 51;
                }
                return 2;
            }
        }
    }
    return 0xFF;
}


u8 UpdateNormalWarioChargedThrow(void)
{
    struct CarriedSprite* carried;
    const u8* states;
    struct WarioData* wario;
    register u32 value PIN("r0");

    carried = &gCurrentCarriedSprite;
    states = sWarioChargedThrowStateTable;
    wario = &gWarioData;
    value = wario->unk_1F;
    value += (u32)states;
    carried->state = *(u8*)value;

    {
        const struct WarioAnimationFrame* animation;
        u32 frame;

        animation = sWarioChargedThrowAnimation;
        frame = wario->unk_1F;
        value = frame << 1;
        value += frame;
        value <<= 2;
        value += (u32)animation;
        frame = wario->unk_1E;
        value = *(u8*)(value + 8);
        if (frame >= value) {
            value = 0;
            wario->unk_1E = value;
            value = wario->unk_1F;
            value++;
            wario->unk_1F = value;
            frame = wario->unk_1F;
            value = frame << 1;
            value += frame;
            value <<= 2;
            value += (u32)animation;
            value = *(u8*)(value + 8);
            if (value == 0) {
                value = (u32)&gButtonsHeld;
                frame = *(u16*)value;
                value = DPAD_UP;
                value &= frame;
                if (value != 0) {
                    return 51;
                }
                return 2;
            }
        }
    }
    return 0xFF;
}


u8 UpdateNormalWarioWeakThrowUpwards(void)
{
    struct WarioData* wario;
    const struct WarioAnimationFrame* animation;
    u32 value;
    u32 frame;
    u32 zero;

    value = (u32)&gCurrentCarriedSprite;
    zero = 0;
    ((struct CarriedSprite*)value)->state = zero;
    wario = &gWarioData;
    animation = sWarioWeakThrowUpwardsAnimation;
    frame = wario->unk_1F;
    value = frame << 1;
    value += frame;
    value <<= 2;
    value += (u32)animation;
    frame = wario->unk_1E;
    value = *(u8*)(value + 8);
    if (frame >= value) {
        wario->unk_1E = zero;
        value = wario->unk_1F;
        value++;
        wario->unk_1F = value;
        frame = wario->unk_1F;
        if (animation[frame].time == 0) {
            value = 2;
            wario->unk_08 = value;
            return 0xFE;
        }
    }
    return 0xFF;
}


u8 UpdateNormalWarioTurningWhileChargingThrow(void)
{
    struct CarriedSprite* carried;
    register struct WarioData* wario PIN("r4");
    register u32 value PIN("r0");
    u32 input;

    carried = &gCurrentCarriedSprite;
    if (carried->state == 0) {
        return 2;
    }

    {
        const u8* states;
        register struct WarioData* base PIN("r2");

        states = sWarioTurningThrowStateTable;
        base = &gWarioData;
        value = base->unk_1F;
        value += (u32)states;
        carried->state = *(u8*)value;
        value = (u32)&gButtonsPressed;
        input = *(u16*)value;
        value = A_BUTTON;
        value &= input;
        wario = base;
    }
    if (value != 0) {
        carried->state = 0;
        value = 1;
        wario->unk_08 = value;
        return 0xFE;
    }
    value = DPAD_DOWN;
    value &= input;
    if (value != 0) {
        return 5;
    }

    {
        u16* heldPointer;

        heldPointer = &gButtonsHeld;
        input = *heldPointer;
        value = DPAD_UP;
        value &= input;
        if (value != 0) {
            value = carried->unk1;
            input = 0x80;
            value |= input;
        } else {
            input = carried->unk1;
            value = 0x0F;
            value &= input;
        }
        carried->unk1 = value;
        input = *heldPointer;
        value = B_BUTTON;
        value &= input;
        if (value == 0) {
            return 37;
        }
    }

    {
        const struct WarioAnimationFrame* animation;
        u32 frame;

        animation = sWarioTurningThrowAnimation;
        frame = wario->unk_1F;
        if (wario->unk_1E >= animation[frame].time) {
            wario->unk_1E = 0;
            wario->unk_1F++;
            {
                u32 nextFrame;

                nextFrame = wario->unk_1F;
                if (animation[nextFrame].time == 0) {
                    return 35;
                }
            }
        }
    }
    return 0xFF;
}


u8 UpdateNormalWarioClimbingLadder(void)
{
    struct WarioData* wario;
    const struct WarioAnimationFrame* animation;
    register u32 value PIN("r0");
    u32 input;
    register u32 held PIN("r2");

    input = (u32)&gButtonsHeld;
    held = *(u16*)input;
    value = B_BUTTON;
    value &= held;
    if (value != 0) {
        value = DPAD_DOWN;
        value &= held;
        if (value != 0) {
            return 42;
        }
        value = DPAD_LEFT | DPAD_RIGHT;
        value &= held;
        if (value != 0) {
            return 0xFE;
        }
    }

    input = *(u16*)input;
    value = DPAD_UP;
    value &= input;
    if (value != 0) {
        struct WarioData* base;

        base = &gWarioData;
        value = base->yPosition;
        value -= 4;
        base->yPosition = value;
        wario = base;
    } else {
        value = DPAD_DOWN;
        value &= input;
        if (value == 0) {
            return 41;
        }
        {
            struct WarioData* base;

            base = &gWarioData;
            value = base->yPosition;
            value += 4;
            base->yPosition = value;
            wario = base;
        }
    }

    {
        register struct WarioData* animationWario PIN("r2");
        u32 frame;

        animationWario = wario;
        animation = sWarioVerticalClimbingAnimation;
        frame = animationWario->unk_1F;
        if (animationWario->unk_1E >= animation[frame].time) {
            animationWario->unk_1E = 0;
            value = ++animationWario->unk_1F;
            value = (u8)value;
            if (value == 1) {
                m4aSongNumStart(5);
            }
            {
                u32 nextFrame;

                nextFrame = wario->unk_1F;
                if (animation[nextFrame].time == 0) {
                    wario->unk_1F = 0;
                    gWarioDustEffect1.unk0 = 7;
                }
            }
        }
    }
    return 0xFF;
}

u8 UpdateNormalWarioIdleOnLadder(void)
{
    u32 value;
    u32 input;
    register u32 horizontal PIN("r2");

    input = (u32)&gButtonsHeld;
    horizontal = *(u16*)input;
    value = B_BUTTON;
    value &= horizontal;
    if (value != 0) {
        value = DPAD_DOWN;
        value &= horizontal;
        if (value != 0) {
            return 42;
        }
        value = DPAD_LEFT | DPAD_RIGHT;
        value &= horizontal;
        if (value != 0) {
            return 0xFE;
        }
    }

    input = *(u16*)input;
    value = DPAD_UP | DPAD_DOWN;
    value &= input;
    if (value != 0) {
        return 40;
    }

    horizontal = DPAD_LEFT | DPAD_RIGHT;
    horizontal &= input;
    if (horizontal != 0) {
        struct WarioData* wario;

        wario = &gWarioData;
        value = wario->unk_0A;
        if (value > 10) {
            return 0xFE;
        }
        value++;
        wario->unk_0A = value;
    } else {
        value = (u32)&gWarioData;
        ((struct WarioData*)value)->unk_0A = horizontal;
    }
    return 0xFF;
}

u8 UpdateNormalWarioSlidingDownLadder(void)
{
    struct WarioData* wario;
    s32 value;
    register u32 input PIN("r1");
    register u32 mask PIN("r2");

    wario = &gWarioData;
    value = wario->yPosition;
    value -= 111;
    value = (u16)value;
    input = wario->xPosition;
    value = GetBackgroundCollisionAtPosition(value, input);
    value >>= 16;
    value -= 2;
    if ((u32)value <= 1) {
        value = (u32)&gButtonsPressed;
        input = *(u16*)value;
        mask = DPAD_UP;
        value = mask;
        value &= input;
        if (value != 0) {
            return 41;
        }
        value = (u32)&gButtonsHeld;
        input = *(u16*)value;
        value = mask;
        value &= input;
        if (value != 0) {
            value = wario->unk_0A;
            if ((u32)value > 14) {
                return 41;
            }
            goto incrementTimer;
        }
        goto checkTimer;
    }

    value = wario->yPosition;
    input = wario->xPosition;
    value = GetBackgroundCollisionAtPosition(value, input);
    value >>= 16;
    value -= 2;
    if ((u32)value > 1) {
        value = 2;
        wario->unk_08 = value;
        return 0xFE;
    }

checkTimer:
    value = (u32)&gWarioData;
    input = ((struct WarioData*)value)->unk_0A;
    mask = value;
    if (input > 14) {
        goto updateEffect;
    }

incrementTimer:
    input = (u32)&gWarioData;
    value = ((struct WarioData*)input)->unk_0A;
    value++;
    ((struct WarioData*)input)->unk_0A = value;
    mask = input;

updateEffect:
    value = ((struct WarioData*)mask)->unk_1E;
    if ((u32)value > 5) {
        value = 0;
        ((struct WarioData*)mask)->unk_1E = value;
        input = (u32)&gWarioDustEffect1;
        value = 11;
        ((struct WarioDustEffect*)input)->unk0 = value;
    }
    return 0xFF;
}

u8 UpdateNormalWarioClimbingFenceVertical(void)
{
    u32 value;
    u32 input;
    u32 held;

    value = (u32)&gButtonsPressed;
    input = *(u16*)value;
    value = A_BUTTON;
    value &= input;
    if (value != 0) {
        input = (u32)&gWarioData;
        value = 1;
        ((struct WarioData*)input)->unk_08 = value;
        return 0xFE;
    }

    input = (u32)&gButtonsHeld;
    held = *(u16*)input;
    value = B_BUTTON;
    value &= held;
    if (value != 0) {
        value = DPAD_DOWN;
        value &= held;
        if (value != 0) {
            return 46;
        }
    }

    input = *(u16*)input;
    value = DPAD_UP | DPAD_DOWN;
    value &= input;
    if (value != 0) {
        value = DPAD_UP;
        value &= input;
        if (value != 0) {
            register struct WarioData* wario PIN("r1");

            wario = &gWarioData;
            value = wario->yPosition;
            value -= 4;
            wario->yPosition = value;
            held = (u32)wario;
            goto updateAnimation;
        }

        value = DPAD_DOWN;
        value &= input;
        held = (u32)&gWarioData;
        if (value != 0) {
            value = ((struct WarioData*)held)->yPosition;
            value += 4;
            ((struct WarioData*)held)->yPosition = value;
        }
        goto updateAnimation;
    }

    held = DPAD_LEFT | DPAD_RIGHT;
    held &= input;
    if (held != 0) {
        value = (u32)&gWarioData;
        ((struct WarioData*)value)->horizontalDirection = held;
        return 44;
    }
    return 45;

updateAnimation:
    {
        struct WarioData* wario;
        const struct WarioAnimationFrame* animation;
        u32 frame;

        wario = (struct WarioData*)held;
        animation = sWarioVerticalClimbingAnimation;
        frame = wario->unk_1F;
        if (wario->unk_1E >= animation[frame].time) {
            wario->unk_1E = 0;
            wario->unk_1F++;
            {
                u32 nextFrame;

                nextFrame = wario->unk_1F;
                if (animation[nextFrame].time == 0) {
                    wario->unk_1F = 0;
                    gWarioDustEffect1.unk0 = 7;
                }
            }
        }
    }
    return 0xFF;
}


u8 UpdateNormalWarioClimbingFenceHorizontal(void)
{
    register u32 value PIN("r0");
    u32 input;
    register u32 direction PIN("r2");

    value = (u32)&gButtonsPressed;
    input = *(u16*)value;
    value = A_BUTTON;
    value &= input;
    if (value != 0) {
        input = (u32)&gWarioData;
        value = 1;
        ((struct WarioData*)input)->unk_08 = value;
        return 0xFE;
    }

    input = (u32)&gButtonsHeld;
    direction = *(u16*)input;
    value = B_BUTTON;
    value &= direction;
    if (value != 0) {
        value = DPAD_DOWN;
        value &= direction;
        if (value != 0) {
            return 46;
        }
    }

    input = *(u16*)input;
    value = DPAD_UP | DPAD_DOWN;
    value &= input;
    if (value != 0) {
        return 43;
    }

    direction = DPAD_RIGHT;
    value = direction;
    value &= input;
    if (value != 0) {
        struct WarioData* wario;

        wario = &gWarioData;
        wario->horizontalDirection = direction;
        value = wario->xPosition;
        value += 4;
        wario->xPosition = value;
        direction = (u32)wario;
    } else {
        direction = DPAD_LEFT;
        value = direction;
        value &= input;
        if (value == 0) {
            return 45;
        }
        {
            struct WarioData* wario;

            wario = &gWarioData;
            wario->horizontalDirection = direction;
            value = wario->xPosition;
            value -= 4;
            wario->xPosition = value;
            direction = (u32)wario;
        }
    }

    {
        struct WarioData* wario;
        const struct WarioAnimationFrame* animation;
        u32 frame;

        wario = (struct WarioData*)direction;
        animation = sWarioHorizontalFenceAnimation;
        frame = wario->unk_1F;
        if (wario->unk_1E >= animation[frame].time) {
            wario->unk_1E = 0;
            wario->unk_1F++;
            {
                u32 nextFrame;

                nextFrame = wario->unk_1F;
                if (animation[nextFrame].time == 0) {
                    wario->unk_1F = 0;
                    gWarioDustEffect1.unk0 = 7;
                }
            }
        }
    }
    return 0xFF;
}

u8 UpdateNormalWarioIdleOnFence(void)
{
    u32 value;
    u32 input;
    u32 held;

    value = (u32)&gButtonsPressed;
    input = *(u16*)value;
    value = A_BUTTON;
    value &= input;
    if (value != 0) {
        input = (u32)&gWarioData;
        value = 1;
        ((struct WarioData*)input)->unk_08 = value;
        return 0xFE;
    }

    input = (u32)&gButtonsHeld;
    held = *(u16*)input;
    value = B_BUTTON;
    value &= held;
    if (value != 0) {
        value = DPAD_DOWN;
        value &= held;
        if (value != 0) {
            return 46;
        }
    }

    input = *(u16*)input;
    value = DPAD_UP | DPAD_DOWN;
    value &= input;
    if (value != 0) {
        return 43;
    }

    value = DPAD_LEFT | DPAD_RIGHT;
    value &= input;
    if (value != 0) {
        return 44;
    }
    return 0xFF;
}

u8 UpdateNormalWarioUnknown46(void)
{
    struct WarioData* wario;
    s32 value;
    register u32 input PIN("r1");

    wario = &gWarioData;
    value = wario->yPosition;
    value -= 111;
    value = (u16)value;
    input = wario->xPosition;
    value = GetBackgroundCollisionAtPosition(value, input);
    value >>= 16;
    if (value != 4) {
        goto transition;
    }

    value = (u32)&gButtonsHeld;
    input = *(u16*)value;
    value = DPAD_UP;
    value &= input;
    if (value != 0) {
        return 45;
    }
    goto noTransition;

transition:
    value = 2;
    wario->unk_08 = value;
    return 0xFE;

noTransition:
    value = wario->unk_0A;
    if ((u32)value <= 14) {
        value++;
        wario->unk_0A = value;
    }
    return 0xFF;
}


u8 UpdateNormalWarioUnknown48(void)
{
    struct WarioData* wario;
    s32 value;
    u32 input;

    wario = &gWarioData;
    {
        register s32 velocity PIN("r1");

        value = 24;
        velocity = *(s16*)((u8*)wario + value);
        value = 128;
        value = -value;
        if (velocity > value) {
            goto updateAnimation;
        }
    }
    {
        value = wario->yPosition;
        value -= 119;
        value = (u16)value;
        input = wario->xPosition;
        value = GetBackgroundCollisionAtPosition(value, input);
        value >>= 16;
        if (value == 4) {
            return 46;
        }
        value -= 2;
        if ((u32)value <= 1) {
            return 42;
        }
        value = 2;
        wario->unk_08 = value;
        return 0xFE;
    }

updateAnimation:
    {
        const struct WarioAnimationFrame* animation;
        u32 frame;

        animation = sWarioDoorAnimation;
        frame = wario->unk_1F;
        if (wario->unk_1E >= animation[frame].time) {
            wario->unk_1E = 0;
            wario->unk_1F++;
            {
                u32 nextFrame;

                nextFrame = wario->unk_1F;
                if (animation[nextFrame].time == 0) {
                    wario->unk_1F = 0;
                }
            }
        }
    }
    return 0xFF;
}


u8 UpdateNormalWarioEnteringDoor(void)
{
    u32 zero;
    register u32 value PIN("r0");
    register u32 frame PIN("r1");
    struct WarioData* wario;

    value = (u32)&gWarioData;
    frame = ((struct WarioData*)value)->unk_0C;
    zero = frame;
    wario = (struct WarioData*)value;
    if (zero != 0) {
        value = frame - 1;
        wario->unk_0C = value;
        return 0xFF;
    }

    {
        register const struct WarioAnimationFrame* animation PIN("r2");

        animation = sWarioEnteringDoorAnimation;
        frame = *(volatile u8*)&wario->unk_1F;
        value = animation[frame].time;
        frame = wario->unk_1E;
        if (frame >= value) {
            wario->unk_1E = zero;
            value = wario->unk_1F;
            frame = value + 1;
            wario->unk_1F = frame;
            value = gCurrentStageNumber;
            if (value == 4) {
                value = (u8)frame;
                if (value == 2) {
                    frame = wario->xPosition;
                    value = 192;
                    value <<= 3;
                    if (frame > value) {
                        value = 0xFF;
                        wario->unk_0C = value;
                    }
                    return 0xFF;
                }
            }

            value = wario->unk_1F;
            frame = value << 1;
            frame += value;
            frame <<= 2;
            frame += (u32)animation;
            value = *(u8*)(frame + 8);
            if (value == 0) {
                return 50;
            }
        }
    }
    return 0xFF;
}

u8 UpdateNormalWarioExitingDoor(void)
{
    struct WarioData* wario;
    const struct WarioAnimationFrame* animation;
    register u32 value PIN("r0");
    u32 frame;

    wario = &gWarioData;
    animation = sWarioDoorAnimation;
    value = wario->unk_1F;
    if (wario->unk_1E >= animation[value].time) {
        wario->unk_1E = 0;
        wario->unk_1F++;
        frame = wario->unk_1F;
        if (animation[frame].time == 0) {
            value = gCurrentShopItem;
            if (value != 0) {
                value = DPAD_RIGHT;
                wario->horizontalDirection = value;
                return 58;
            }
            return 2;
        }
    }
    return 0xFF;
}

u8 UpdateNormalWarioLookingUp(void)
{
    struct WarioData* wario;
    s32 value;
    register u32 input PIN("r1");
    register u32 held PIN("r2");

    value = (u32)&gButtonsPressed;
    input = *(u16*)value;
    value = A_BUTTON;
    value &= input;
    if (value != 0) {
        return 3;
    }

    value = B_BUTTON;
    value &= input;
    if (value != 0) {
        value = gCurrentCarriedSprite.state;
        if (value != 0) {
            return 34;
        }
        return 22;
    }

    value = (u8)gWarioCanEnterDoor;
    if (value != 0) {
        value = (u32)&gButtonsHeld;
        input = *(u16*)value;
        value = DPAD_UP;
        value &= input;
        if (value != 0) {
            wario = &gWarioData;
            value = wario->yPosition;
            input = wario->xPosition;
            value = GetBackgroundCollisionAtPosition(value, input);
            value >>= 16;
            if (value == 1) {
                value = wario->yPosition;
                value -= 119;
                value = (u16)value;
                input = wario->xPosition;
                value = GetBackgroundCollisionAtPosition(value, input);
                value >>= 16;
                if (value == 1) {
                    return 49;
                }
            }
        }
    }

    value = (u32)&gButtonsHeld;
    input = (u32)&gWarioData;
    held = *(u16*)value;
    input = ((struct WarioData*)input)->horizontalDirection;
    value = held;
    value &= input;
    if (value != 0) {
        return 0;
    }
    value = DPAD_LEFT | DPAD_RIGHT;
    value ^= input;
    value &= held;
    if (value != 0) {
        return 1;
    }
    value = DPAD_UP;
    value &= held;
    if (value == 0) {
        return 2;
    }
    return 0xFF;
}

u8 UpdateNormalWarioUnknown52(void)
{
    struct CarriedSprite* carried;
    register u32 value PIN("r0");

    carried = &gCurrentCarriedSprite;
    value = 0;
    carried->state = value;
    value = gWarioData.unk_1E;
    if (value > 15) {
        return 5;
    }
    return 0xFF;
}


u8 UpdateNormalWarioUnknown53(void)
{
    struct WarioData* wario;
    const struct WarioAnimationFrame* animation;
    register u32 value PIN("r0");
    u32 input;
    u32 zero;

    value = (u32)&gButtonsPressed;
    input = *(u16*)value;
    value = A_BUTTON;
    value &= input;
    if (value != 0) {
        return 3;
    }
    value = B_BUTTON;
    value &= input;
    if (value != 0) {
        return 22;
    }
    zero = 0xF3;
    zero &= input;
    if (zero != 0) {
        return 2;
    }

    wario = &gWarioData;
    animation = sWarioUnknown53Animation;
    value = wario->unk_1F;
    if (wario->unk_1E >= animation[value].time) {
        wario->unk_1E = zero;
        wario->unk_1F++;
        input = wario->unk_1F;
        if (animation[input].time == 0) {
            wario->unk_1F = 0;
        }
    }
    return 0xFF;
}


u8 UpdateNormalWarioDumbbells(void)
{
    struct WarioData* wario;
    const struct WarioAnimationFrame* animation;
    register u32 value PIN("r0");
    u32 input;

    {
        struct WarioData* base;

        base = &gWarioData;
        input = base->pose;
        wario = base;
    }
    if (input == 54) {
        value = (u32)&gButtonsPressed;
        input = *(u16*)value;
        value = A_BUTTON;
        value &= input;
        if (value != 0) {
            return 3;
        }
        value = B_BUTTON;
        value &= input;
        if (value != 0) {
            return 22;
        }
        value = 0xF3;
        value &= input;
        if (value != 0) {
            return 2;
        }
    }

    animation = sWarioDumbbellAnimation;
    value = wario->unk_1F;
    if (wario->unk_1E >= animation[value].time) {
        wario->unk_1E = 0;
        wario->unk_1F++;
        input = wario->unk_1F;
        if (animation[input].time == 0) {
            wario->unk_1F = 0;
        }
    }
    return 0xFF;
}


u8 UpdateNormalWarioBarbell(void)
{
    struct WarioData* wario;
    const struct WarioAnimationFrame* animation;
    u32 value;
    u32 frame;

    wario = &gWarioData;
    animation = sWarioBarbellAnimation;
    value = wario->unk_1F;
    if (wario->unk_1E >= animation[value].time) {
        wario->unk_1E = 0;
        wario->unk_1F++;
        frame = wario->unk_1F;
        if (animation[frame].time == 0) {
            return 57;
        }
    }
    return 0xFF;
}


u8 UpdateNormalWarioBarbellEnding(void)
{
    struct WarioData* wario;
    const struct WarioAnimationFrame* animation;
    u32 value;
    u32 frame;

    wario = &gWarioData;
    animation = sWarioBarbellEndingAnimation;
    value = wario->unk_1F;
    if (wario->unk_1E >= animation[value].time) {
        wario->unk_1E = 0;
        wario->unk_1F++;
        frame = wario->unk_1F;
        if (animation[frame].time == 0) {
            wario->unk_1F = 0;
        }
    }
    return 0xFF;
}


u8 UpdateNormalWarioUnknown58(void)
{
    struct WarioData* wario;
    const struct WarioAnimationFrame* animation;
    register u32 value PIN("r0");
    u32 input;

    {
        struct WarioData* base;

        base = &gWarioData;
        input = base->pose;
        wario = base;
    }

    if (input == 58) {
        input = wario->xPosition;
        value = 0x6DF;
        if (input > value) {
            return 49;
        }
        goto updateMovement;
    }

    input = *(volatile u16*)&wario->horizontalDirection;
    value = DPAD_RIGHT;
    value &= input;
    if (value != 0) {
        input = wario->xPosition;
        value = 0x4DF;
        if (input > value) {
            return 65;
        }
    }

    input = *(volatile u16*)&wario->horizontalDirection;
    value = DPAD_LEFT;
    value &= input;
    if (value != 0) {
        input = wario->xPosition;
        value = 156;
        value <<= 3;
        if (input <= value) {
            return 65;
        }
    }

updateMovement:
    input = *(volatile u16*)&wario->horizontalDirection;
    value = DPAD_RIGHT;
    value &= input;
    if (value != 0) {
        value = 64;
    } else {
        value = 0xFFC0;
    }
    wario->xVelocity = value;

    animation = sWarioWalkingAnimation;
    value = wario->unk_1F;
    if (wario->unk_1E >= animation[value].time) {
        value = 0;
        wario->unk_1E = value;
        wario->unk_1F++;
        input = wario->unk_1F;
        value = animation[input].time;
        if (value == 0) {
            wario->unk_1F = value;
            gWarioDustEffect1.unk0 = 5;
        } else {
            value = input;
            if ((value == 2) || (value == 9)) {
                m4aSongNumStart(1);
            }
        }
    }
    return 0xFF;
}


u8 UpdateNormalWarioJumpRope(void)
{
    struct WarioData* wario;
    const struct WarioAnimationFrame* animation;
    register u32 value PIN("r0");
    u32 input;
    u32 zero;

    value = (u32)&gButtonsPressed;
    input = *(u16*)value;
    value = A_BUTTON;
    value &= input;
    if (value != 0) {
        return 3;
    }
    value = B_BUTTON;
    value &= input;
    if (value != 0) {
        return 22;
    }
    zero = 0xF3;
    zero &= input;
    if (zero != 0) {
        return 2;
    }

    wario = &gWarioData;
    animation = sWarioJumpRopeAnimation;
    value = wario->unk_1F;
    if (wario->unk_1E >= animation[value].time) {
        wario->unk_1E = zero;
        wario->unk_1F++;
        input = wario->unk_1F;
        if (animation[input].time == 0) {
            wario->unk_1F = 0;
        }
    }
    return 0xFF;
}


u8 UpdateNormalWarioUnknown60(void)
{
    u32 timer;
    struct WarioData* wario;
    register u32 value PIN("r0");
    u32 input;
    register u32 currentTimer PIN("r2");
    struct WarioData* base;

    base = &gWarioData;
    currentTimer = base->unk_0A;
    timer = currentTimer;
    wario = base;
    if (timer > 64) {
        value = 96;
        wario->damageTimer = value;
        return 4;
    }

    value = (u32)&gButtonsPressed;
    value = *(u16*)value;
    input = DPAD_LEFT | DPAD_RIGHT;
    input &= value;
    if (input != 0) {
        value = currentTimer;
        value += 11;
        wario->unk_0A = value;
        wario->horizontalDirection = input;
    } else if (timer != 0) {
        value = currentTimer - 1;
        wario->unk_0A = value;
    }

    {
        const struct WarioAnimationFrame* animation;

        animation = sWarioUnknown60Animation;
        value = wario->unk_1F;
        if (wario->unk_1E >= animation[value].time) {
            wario->unk_1E = 0;
            wario->unk_1F++;
            input = wario->unk_1F;
            if (animation[input].time == 0) {
                wario->unk_1F = 0;
            }
        }
    }
    return 0xFF;
}

u8 UpdateNormalWarioBossDefeatedMidair(void)
{
    return 0xFF;
}

u8 UpdateNormalWarioBossDefeated(void)
{
    if (gWarioData.unk_1E > 39) {
        return 65;
    }
    return 0xFF;
}


u8 UpdateNormalWarioFlexing(void)
{
    u8* passage;
    struct WarioData* wario;
    register const struct WarioAnimationFrame* animation PIN("r2");
    u32 value;
    u32 frame;
    register u32 previousFrame PIN("r3");
    u8* passageBase;

    passageBase = &gCurrentPassage;
    frame = *passageBase;
    passage = passageBase;
    wario = &gWarioData;
    if (frame == 0 && wario->unk_1F == 0 && wario->unk_1E <= 49) {
        wario->unk_1E = 50;
    }

    animation = sWarioFlexingAnimation;
    frame = wario->unk_1F;
    value = animation[frame].time;
    if (wario->unk_1E >= value) {
        value = 0;
        wario->unk_1E = value;
        previousFrame = wario->unk_1F;
        frame = previousFrame + 1;
        wario->unk_1F = frame;
        value = *passage;
        if (value == 0) {
            value = (u8)frame;
            if (value == 9) {
                wario->unk_1F = previousFrame;
                goto animation_done_80126A8;
            }
        }
        {
            frame = wario->unk_1F;
            value = animation[frame].time;
            if (value == 0) {
                value = frame - 4;
                wario->unk_1F = value;
            }
        }
animation_done_80126A8:
        value = wario->unk_1F;
        if (value == 1) {
            m4aSongNumStart(0x1B1);
        }
    }
    return 0xFF;
}


u8 UpdateNormalWarioUnknown66(void)
{
    struct WarioData* wario;
    const struct WarioAnimationFrame* animation;
    register u32 value PIN("r0");
    u32 frame;

    wario = &gWarioData;
    animation = sWarioUnknown66Animation;
    value = wario->unk_1F;
    if (wario->unk_1E >= animation[value].time) {
        wario->unk_1E = 0;
        wario->unk_1F++;
        frame = wario->unk_1F;
        value = animation[frame].time;
        if (value == 0) {
            wario->unk_1F = value;
        }
    }
    return 0xFF;
}

u8 UpdateNormalWarioUnknown67(void)
{
    struct WarioData* wario;
    const struct WarioAnimationFrame* animation;
    register u32 value PIN("r0");
    u32 frame;

    wario = &gWarioData;
    animation = sWarioUnknown67Animation;
    value = wario->unk_1F;
    if (wario->unk_1E >= animation[value].time) {
        wario->unk_1E = 0;
        wario->unk_1F++;
        frame = wario->unk_1F;
        value = animation[frame].time;
        if (value == 0) {
            value = frame - 1;
            wario->unk_1F = value;
        }
    }
    return 0xFF;
}

u8 UpdateNormalWarioUnknown68(void)
{
    struct WarioData* wario;
    const struct WarioAnimationFrame* animation;
    register u32 value PIN("r0");
    u32 frame;

    wario = &gWarioData;
    animation = sWarioUnknown68Animation;
    value = wario->unk_1F;
    if (wario->unk_1E >= animation[value].time) {
        wario->unk_1E = 0;
        wario->unk_1F++;
        frame = wario->unk_1F;
        value = animation[frame].time;
        if (value == 0) {
            wario->unk_1F = value;
        }
    }
    return 0xFF;
}

u8 UpdateNormalWarioUnknown69(void)
{
    struct WarioData* wario;
    const struct WarioAnimationFrame* animation;
    register u32 value PIN("r0");
    u32 input;
    u32 zero;

    value = (u32)&gButtonsPressed;
    input = *(u16*)value;
    value = A_BUTTON;
    value &= input;
    if (value != 0) {
        return 3;
    }
    value = B_BUTTON;
    value &= input;
    if (value != 0) {
        return 22;
    }
    zero = 0xF3;
    zero &= input;
    if (zero != 0) {
        return 70;
    }

    wario = &gWarioData;
    animation = sWarioUnknown69Animation;
    value = wario->unk_1F;
    if (wario->unk_1E >= animation[value].time) {
        wario->unk_1E = zero;
        wario->unk_1F++;
        input = wario->unk_1F;
        value = animation[input].time;
        if (value == 0) {
            value = input - 6;
            wario->unk_1F = value;
        }
    }
    return 0xFF;
}

u8 UpdateNormalWarioUnknown70(void)
{
    struct WarioData* wario;
    const struct WarioAnimationFrame* animation;
    register u32 value PIN("r0");
    u32 frame;

    wario = &gWarioData;
    animation = sWarioUnknown70Animation;
    value = wario->unk_1F;
    if (wario->unk_1E >= animation[value].time) {
        wario->unk_1E = 0;
        wario->unk_1F++;
        frame = wario->unk_1F;
        value = animation[frame].time;
        if (value == 0) {
            return 2;
        }
    }
    return 0xFF;
}

u8 UpdateNormalWarioUnknown71(void)
{
    struct WarioData* wario;
    const struct WarioAnimationFrame* animation;
    register u32 value PIN("r0");
    u32 input;

    value = (u32)&gButtonsPressed;
    input = *(u16*)value;
    value = B_BUTTON;
    value &= input;
    if (value != 0 && gCurrentCarriedSprite.state != 0) {
        return 34;
    }

    wario = &gWarioData;
    animation = sWarioUnknown71Animation;
    value = wario->unk_1F;
    if (wario->unk_1E >= animation[value].time) {
        wario->unk_1E = 0;
        wario->unk_1F++;
        input = wario->unk_1F;
        value = animation[input].time;
        if (value == 0) {
            value = wario->unk_07;
            if (value != 0) {
                value = 1;
                wario->unk_08 = value;
                return 0xFE;
            }
            return 2;
        }
    }
    return 0xFF;
}


u8 UpdateNormalWarioUnknown72(void)
{
    register u32 value PIN("r0");
    struct WarioData* warioBase;
    struct WarioData* wario;
    register u32 input PIN("r3");
    u32 zero;

    value = (u32)&gButtonsPressed;
    input = *(u16*)value;
    value = A_BUTTON;
    value &= input;
    warioBase = &gWarioData;
    if (value != 0) {
        goto cancel_8012914;
    }
    wario = warioBase;
    value = wario->unk_07;
    if (value != 0) {
cancel_8012914:
        value = 1;
        warioBase->unk_08 = value;
        return 0xFE;
    }

    value = B_BUTTON;
    value &= input;
    zero = (u16)value;
    if (zero != 0) {
        return 23;
    }

    value = (u32)&gButtonsHeld;
    warioBase = (struct WarioData*)(u32)*(u16*)value;
    input = wario->horizontalDirection;
    value = (u32)warioBase;
    value &= input;
    if (value != 0) {
        value = 0x300;
        value &= (u32)warioBase;
        if (value == 0) {
            value = DPAD_RIGHT;
            value &= input;
            if (value != 0) {
                wario->xVelocity -= 6;
            } else {
                wario->xVelocity += 6;
            }
            return 0;
        }
    } else {
        return 11;
    }

    {
        const struct WarioAnimationFrame* animation;
        u32 frame;

        animation = sWarioWalkingAnimation;
        value = wario->unk_1F;
        if (wario->unk_1E >= animation[value].time) {
            wario->unk_1E = zero;
            wario->unk_1F++;
            frame = wario->unk_1F;
            value = animation[frame].time;
            if (value == 0) {
                wario->unk_1F = value;
            } else {
                value = frame;
                if (value == 2 || value == 9) {
                    m4aSongNumStart(1);
                }
            }
        }
    }
    return 0xFF;
}

u8 UpdateNormalWarioUnknown73(void)
{
    return 0xFF;
}

u8 UpdateNormalWarioUnknown74(void)
{
    struct WarioData* wario;
    u32 value;
    s32 velocity;

    wario = &gWarioData;
    value = wario->unk_1E;
    if (value > 59) {
        goto cancel_80129D8;
    }
    velocity = wario->yVelocity;
    value = -120;
    if (velocity < (s32)value) {
        value = (u32)&gButtonsHeld;
        velocity = *(u16*)value;
        value = DPAD_LEFT | DPAD_RIGHT;
        value &= velocity;
        if (value != 0) {
cancel_80129D8:
            value = 2;
            wario->unk_08 = value;
            return 0xFE;
        }
    }
    return 0xFF;
}

u8 UpdateNormalWarioUnknown75(void)
{
    struct WarioData* wario;
    const struct WarioAnimationFrame* animation;
    register u32 value PIN("r0");
    u32 frame;

    wario = &gWarioData;
    animation = sWarioUnknown75Animation;
    value = wario->unk_1F;
    if (wario->unk_1E >= animation[value].time) {
        wario->unk_1E = 0;
        wario->unk_1F++;
        frame = wario->unk_1F;
        value = animation[frame].time;
        if (value == 0) {
            return 2;
        }
    }
    return 0xFF;
}

u8 UpdateNormalWarioUnused76(void)
{
    struct WarioData* wario;
    const struct WarioAnimationFrame* animation;
    register u32 value PIN("r0");
    u32 frame;

    wario = &gWarioData;
    animation = sWarioUnusedPose76Animation;
    value = wario->unk_1F;
    if (wario->unk_1E >= animation[value].time) {
        wario->unk_1E = 0;
        wario->unk_1F++;
        frame = wario->unk_1F;
        value = animation[frame].time;
        if (value == 0) {
            return 2;
        }
    }
    return 0xFF;
}

u8 UpdateNormalWarioUnused77(void)
{
    struct WarioData* wario;
    const struct WarioAnimationFrame* animation;
    register u32 value PIN("r0");
    u32 input;

    value = (u32)&gButtonsPressed;
    input = *(u16*)value;
    value = A_BUTTON;
    value &= input;
    if (value != 0) {
        register u32 held PIN("r2");
        u32 mask;

        value = (u32)&gButtonsHeld;
        held = *(u16*)value;
        mask = 0x300;
        value = mask;
        value &= held;
        if (value == mask) {
            return 78;
        }
    }

    wario = &gWarioData;
    animation = sWarioUnusedPose77Animation;
    value = wario->unk_1F;
    if (wario->unk_1E >= animation[value].time) {
        wario->unk_1E = 0;
        wario->unk_1F++;
        input = wario->unk_1F;
        value = animation[input].time;
        if (value == 0) {
            value = input - 1;
            wario->unk_1F = value;
        }
    }
    return 0xFF;
}

u8 UpdateNormalWarioUnused78(void)
{
    struct WarioData* wario;
    register u32 value PIN("r0");
    u32 input;
    u32 zero;

    value = (u32)&gButtonsPressed;
    input = *(u16*)value;
    value = A_BUTTON;
    value &= input;
    if (value != 0) {
        struct WarioData* warioBase;

        warioBase = &gWarioData;
        value = 1;
        warioBase->unk_08 = value;
        return 0xFE;
    }
    value = B_BUTTON;
    value &= input;
    if (value != 0) {
        return 22;
    }

    value = (u32)&gButtonsHeld;
    wario = &gWarioData;
    input = *(u16*)value;
    value = wario->horizontalDirection;
    value &= input;
    if (value != 0) {
        return 0;
    }
    value = wario->unk_0A;
    if (value == 0) {
        return 2;
    }
    value--;
    wario->unk_0A = value;
    value = *(u16*)&wario->xVelocity;
    value += 6;
    zero = 0;
    wario->xVelocity = value;
    value = (s16)value;
    if ((s32)value > 64) {
        value = 64;
        wario->xVelocity = value;
    }

    {
        const struct WarioAnimationFrame* animation;
        u32 frame;

        animation = sWarioWalkingAnimation;
        value = wario->unk_1F;
        if (wario->unk_1E >= animation[value].time) {
            wario->unk_1E = zero;
            wario->unk_1F++;
            frame = wario->unk_1F;
            value = animation[frame].time;
            if (value == 0) {
                wario->unk_1F = zero;
            } else {
                value = frame;
                if (value == 2 || value == 9) {
                    m4aSongNumStart(1);
                }
            }
        }
    }
    return 0xFF;
}

void ApplyNormalWarioPoseTransition(u8 pose)
{
    PrepareWarioUpdate();
    gWarioMotionAfterimage = sEmptyWarioAfterimage;
    if (gCurrentWarioEffect.type != 4) {
        gCurrentWarioEffect = sStartingWarioEffect;
    }

    if (gSubGameMode == 4) {
        gSubGameMode = 2;
        if (gWarioData.pose == 17) {
            gWarioData.yVelocity = 32;
        } else if (gWarioData.pose == 18) {
            gWarioData.yVelocity = -32;
        } else {
            gWarioData.unk_1F = 2;
        }
        return;
    }

    gWarioData.unk_02 = 0;
    if (pose == 5 && (gWarioDataCopy.unk_1C & 1)) {
        pose = 12;
    }

    if (pose == 31) {
        if (((int (*)(u8, u16, u8))CheckWarioVerticalCollision)(0, (s16)0xFFC0, 1) != 0) {
            memcpy(&gWarioData, &gWarioDataCopy, sizeof(gWarioData));
            return;
        }
        if (gButtonsHeld & A_BUTTON) {
            gWarioDataCopy.unk_08 = 1;
            pose = 0xFE;
        }
    }

    if (gWarioDataCopy.pose == 35) {
        if (pose != 37 && pose != 39) {
            m4aSongNumStart(40);
        }
    } else if (gWarioDataCopy.pose == 39) {
        if (pose != 37 && pose != 35) {
            m4aSongNumStart(40);
        }
    }

    if (pose == 0xFE) {
        ResumeNormalWarioAfterCollision();
    } else if (pose == 0xFD) {
        ResolveNormalWarioPoseCollision();
    } else {
        SetNormalWarioPose(pose);
    }
}

void ResumeNormalWarioAfterCollision(void)
{
    struct WarioData* wario;
    struct WarioData* copy;
    register u32 value PIN("r0");
    u32 temp;
    register struct CarriedSprite* carried PIN("r2");
    struct WarioData* warioBase;
    struct WarioData* copyBase;

    warioBase = &gWarioData;
    copyBase = &gWarioDataCopy;
    value = *(u16*)&copyBase->xVelocity;
    warioBase->xVelocity = value;
    value = copyBase->pose;
    value -= 5;
    wario = warioBase;
    copy = copyBase;

    switch (value) {
        case 47:
            wario->unk_09++;
        case 0:
        case 1:
        case 2:
        case 4:
            value = 0;
            wario->xVelocity = value;
        case 3:
            value = 8;
            wario->pose = value;
            value = 128;
            wario->yVelocity = value;
            break;

        case 9:
            value = 15;
            wario->pose = value;
            value = 128;
            wario->yVelocity = value;
            break;

        case 17:
            value = 24;
            wario->pose = value;
            value = 176;
            wario->yVelocity = value;
            break;

        case 18:
            value = 25;
            wario->pose = value;
            value = 176;
            wario->yVelocity = value;
            value = copy->unk_0C;
            wario->unk_0C = value;
            {
                struct WarioEffect* effect;

                effect = &gCurrentWarioEffect;
                value = 3;
                effect->type = value;
            }
            break;

        case 38:
        case 39:
        case 40:
            value = 4;
            wario->pose = value;
            value = 128;
            wario->yVelocity = value;
            break;

        case 19:
            m4aSongNumStart(31);
        case 11:
        case 21:
        case 26:
        case 42:
            {
                struct WarioData* base;

                base = &gWarioData;
                value = 0;
                base->xVelocity = value;
                wario = base;
                copy = &gWarioDataCopy;
            }
        default:
            value = 4;
            wario->pose = value;
            carried = &gCurrentCarriedSprite;
            temp = carried->unk1;
            value = 15;
            value &= temp;
            if (value > 1) {
                value = 128;
            } else {
                value = 176;
            }
            wario->yVelocity = value;
            temp = carried->unk1;
            value = 15;
            value &= temp;
            if (value != 0) {
                value = 4;
                carried->state = value;
            }
            break;
    }

    value = copy->unk_08;
    if (value == 0) {
        wario->yVelocity = value;
    } else if (value == 2) {
        value = *(u16*)&copy->yVelocity;
        wario->yVelocity = value;
    } else {
        m4aSongNumStart(13);
    }
}

void ResolveNormalWarioPoseCollision(void)
{
    struct WarioData* copy;
    register u32 value PIN("r0");
    register u32 index PIN("r0");
    struct WarioData* wario1;

    {
        struct WarioDustEffect* dust;
        struct WarioData* base;

        dust = &gWarioDustEffect1;
        value = 9;
        dust->unk0 = value;
        base = &gWarioDataCopy;
        index = base->pose;
        index -= 4;
        copy = base;
    }

    switch (index) {
        case 18:
        case 20:
        {
            struct WarioData* wario;
            u32 leftTile;
            u32 mask;
            u32 input;

            value = (u32)&gButtonsHeld;
            input = *(u16*)value;
            value = copy->horizontalDirection;
            value &= input;
            if (value != 0) {
                struct WarioData* current;
                u32 zero;

                current = &gWarioData;
                zero = 0;
                current->pose = zero;
                zero = *(u16*)&copy->xVelocity;
                current->xVelocity = zero;
            } else {
                wario = &gWarioData;
                value = GetBackgroundCollisionAtPosition((u16)(wario->yPosition + 1),
                    (u16)(wario->xPosition - 30));
                leftTile = value;
                leftTile = (s32)leftTile >> 16;
                mask = 0xFF;
                leftTile &= mask;
                value = GetBackgroundCollisionAtPosition((u16)(wario->yPosition + 1),
                    (u16)(wario->xPosition + 30));
                value = (s32)value >> 16;
                value &= mask;
                if (leftTile == 11 || value == 11) {
                    value = 10;
                    wario->pose = value;
                    input = copy->xVelocity;
                    value = input << 1;
                    value += input;
                    value = (s32)value >> 2;
                    wario->xVelocity = value;
                } else {
                    value = 2;
                    wario->pose = value;
                }
            }
            gWarioData.unk_07 = copy->unk_07;
            m4aSongNumStart(31);
            break;
        }

        case 0:
        {
            u32 input;
            struct WarioData* current;

            value = (u32)&gButtonsHeld;
            input = *(u16*)value;
            value = copy->horizontalDirection;
            value &= input;
            if (value != 0) {
                current = &gWarioData;
                input = 0;
                current->pose = input;
                input = *(u16*)&copy->xVelocity;
                current->xVelocity = input;
                goto copy_unk07_8012E5C;
            }
            wario1 = &gWarioData;
            value = 2;
            goto set_pose_8012E5C;
        }

        case 4:
            value = copy->unk_09;
            if (value != 0) {
                wario1 = &gWarioData;
                value = 52;
            } else {
                wario1 = &gWarioData;
                value = 5;
            }
set_pose_8012E5C:
            wario1->pose = value;
copy_unk07_8012E5C:
            wario1 = &gWarioData;
            value = copy->unk_07;
            wario1->unk_07 = value;
            break;

        case 11:
        {
            struct WarioData* wario;

            wario = &gWarioData;
            value = 14;
            wario->pose = value;
            value = copy->unk_07;
            wario->unk_07 = value;
            value = *(u16*)&copy->xVelocity;
            wario->xVelocity = value;
            break;
        }

        case 19:
        case 21:
        {
            u32 input;
            u32 mask;
            struct WarioData* wario;

            value = (u32)&gButtonsHeld;
            input = *(u16*)value;
            value = copy->horizontalDirection;
            value &= input;
            if (value != 0) {
                mask = 0x300;
                mask &= input;
                if (mask != 0) {
                    wario = &gWarioData;
                    value = 23;
                    wario->pose = value;
                    value = copy->unk_0C;
                    wario->unk_0C = value;
                    {
                        register struct WarioEffect* effect PIN("r2");

                        effect = &gCurrentWarioEffect;
                        value = 3;
                        effect->type = value;
                    }
                } else {
                    struct WarioData* current;

                    current = &gWarioData;
                    current->pose = mask;
                    wario = current;
                }
                value = copy->unk_07;
                wario->unk_07 = value;
            } else {
                wario = &gWarioData;
                value = 11;
                wario->pose = value;
            }
            value = (u32)&gWarioData;
            input = *(u16*)&copy->xVelocity;
            ((struct WarioData*)value)->xVelocity = input;
            value = ((struct WarioData*)value)->pose;
            if (value != 23) {
                m4aSongNumStart(42);
            }
            break;
        }

        case 23:
            gWarioData.pose = 29;
            gWarioMotionAfterimage.unk0 = 3;
            ScreenShakeRequestY(48, 0);
            m4aSongNumStart(34);
            break;

        case 24:
            gWarioData.pose = 30;
            gWarioMotionAfterimage.unk0 = 3;
            ScreenShakeRequestY(64, 1);
            m4aSongNumStart(36);
            VoiceSetPlay(0);
            break;

        case 57:
        {
            struct WarioData* wario;

            wario = &gWarioData;
            value = 62;
            wario->pose = value;
            break;
        }

        case 15:
        case 16:
            gWarioData.damageTimer = 96;
        case 34:
            gCurrentCarriedSprite.state = 0;
            gCurrentCarriedSprite.unk1 = 0;
        default:
            gWarioData.pose = 2;
            break;
    }

    {
        struct WarioData* wario;
        struct CarriedSprite* carried;
        u32 temp;

        wario = &gWarioData;
        value = wario->pose;
        value -= 29;
        value = (u8)value;
        if (value > 1) {
            m4aSongNumStart(12);
        }

        value = wario->pose;
        if (value == 2) {
            value = (u32)sWarioCollisionPointTable;
            temp = *(u16*)(value + 4);
            value = 0;
            if (((int (*)(u8, u16, u8))CheckWarioVerticalCollision)(value, temp, 1) != 0) {
                value = 5;
                wario->pose = value;
            }
        }

        carried = &gCurrentCarriedSprite;
        temp = carried->unk1;
        value = 15;
        value &= temp;
        if (value != 0) {
            value = carried->state;
            if (value != 8) {
                value = 4;
                carried->state = value;
            }
        }
    }
}

void SetNormalWarioPose(u8 arg0)
{
    u32 pose;
    register u32 value PIN("r0");
    u32 index;
    struct WarioData* wario;

    pose = arg0;
    value = gWarioDataCopy.pose;
    value -= 22;
    switch (value) {
            case 0:
            case 2:
                if (pose == 26) {
                    m4aSongNumStart(32);
                } else {
                    m4aSongNumStart(31);
                }
                break;

            case 1:
            case 3:
                if (pose == 26) {
                    m4aSongNumStart(44);
                } else {
                    m4aSongNumStart(42);
                }
                break;

            case 6:
                m4aSongNumStart(36);
                break;

            case 32:
            case 37:
                m4aSongNumStart(0x1BD);
                VoiceSetPlay(9);
                break;
    }

    {
        struct WarioData* base;

        base = &gWarioData;
        base->pose = pose;
        index = pose - 1;
        wario = base;
    }

    switch (index) {
        case 0:
        case 8:
        case 38:
            value = 1;
            wario->unk_03 = value;
            m4aSongNumStart(8);
            break;

        case 1:
            value = (u32)sWarioCollisionPointTable;
            index = *(u16*)(value + 4);
            value = 0;
            if (((int (*)(u8, u16, u8))CheckWarioVerticalCollision)(value, index, 1) != 0) {
                struct WarioData* current;
                struct CarriedSprite* carried;
                u32 zero;

                current = &gWarioData;
                zero = 0;
                value = 5;
                current->pose = value;
                carried = &gCurrentCarriedSprite;
                carried->state = zero;
            } else {
                struct CarriedSprite* carried;

                carried = &gCurrentCarriedSprite;
                value = carried->state;
                if (value != 0) {
                    value = 4;
                    carried->state = value;
                }
            }
            break;

        case 33:
            value = gButtonsHeld & DPAD_UP;
            if (value != 0) {
                register struct CarriedSprite* carried PIN("r2");

                carried = &gCurrentCarriedSprite;
                index = carried->unk1;
                value = 0x80;
                value |= index;
                carried->unk1 = value;
            }
            break;

        case 4:
        case 51:
            gCurrentCarriedSprite.state = 0;
            if (wario->hitboxOffsetTop < -80) {
                m4aSongNumStart(7);
            }
            break;

        case 5:
            index = gWarioDataCopy.horizontalDirection;
            value = DPAD_RIGHT;
            value &= index;
            if (value != 0) {
                value = 96;
            } else {
                value = 0xFFA0;
            }
            wario->xVelocity = value;
            m4aSongNumStart(10);
            break;

        case 9:
        {
            struct WarioData* copy;
            u32 rawVelocity;

            if (wario->horizontalDirection & DPAD_RIGHT) {
                copy = &gWarioDataCopy;
                index = *(u16*)&copy->xVelocity;
                value = copy->xVelocity;
                if ((s32)value > 31) {
                    goto store_index_801318C;
                }
                value = 32;
                goto store_x_velocity_801318C;
            } else {
                copy = &gWarioDataCopy;
                rawVelocity = *(u16*)&copy->xVelocity;
                value = copy->xVelocity;
                index = -32;
                if ((s32)value > (s32)index) {
store_index_801318C:
                    wario->xVelocity = index;
                    break;
                }
                wario->xVelocity = rawVelocity;
            }
            break;
        }

        case 10:
            wario->xVelocity = gWarioDataCopy.xVelocity;
            gWarioDustEffect1.unk0 = 5;
            break;

        case 11:
        {
            struct WarioData* copy;

            copy = &gWarioDataCopy;
            value = *(u16*)&copy->unk_1C;
            index = 0x30;
            value ^= index;
            /* agbcc otherwise folds the required XOR/AND pair into a BIC. */
            ASM_BARRIER(: "+r"(value));
            value &= index;
            wario->horizontalDirection = value;
            gCurrentCarriedSprite.state = 0;
            gWarioDustEffect1.unk0 = 5;
            m4aSongNumStart(45);
            VoiceSetPlay(0);
            break;
        }

        case 15:
        {
            u32 copyValue;

            copyValue = (u32)&gWarioDataCopy;
            value = ((struct WarioData*)copyValue)->horizontalDirection & DPAD_RIGHT;
            if (value != 0) {
                value = 0xFFE0;
            } else {
                value = 32;
            }
            wario->xVelocity = value;
            copyValue = ((struct WarioData*)copyValue)->unk_1A;
            if (copyValue <= 1) {
                value = 96;
                wario->yVelocity = value;
                if (copyValue == 1) {
                    value = wario->yPosition - 2;
                    wario->yPosition = value;
                }
            }
            m4aSongNumStart(46);
            break;
        }

        case 16:
            gSubGameMode = 4;
            wario->yVelocity = 32;
            gWarioSubgameXPosition = gWarioDataCopy.xPosition;
            gWarioSubgameYPosition = gWarioDataCopy.yPosition - 128;
            goto subgame_tail_801318C;

        case 17:
            gSubGameMode = 4;
            wario->yVelocity = 0xFFE0;
            gWarioSubgameXPosition = gWarioDataCopy.xPosition;
            gWarioSubgameYPosition = gWarioDataCopy.yPosition + 1;
subgame_tail_801318C:
            wario->unk_02 = 1;
            m4aSongNumStart(20);
            break;

        case 18:
            if (gHeartMeter.unk_2 == 0 && gHeartMeter.current != 0) {
                gHeartMeter.current--;
                SpawnSecondarySprite(gWarioData.yPosition - 96,
                    gWarioData.xPosition, 48);
            }
            pose = (u32)&gWarioData;
            ((struct WarioData*)pose)->damageTimer = 1;
            value = (u32)sWarioCollisionPointTable;
            index = *(u16*)(value + 4);
            value = 0;
            if (((int (*)(u8, u16, u8))CheckWarioVerticalCollision)(value, index, 1) != 0) {
                value = 20;
                ((struct WarioData*)pose)->pose = value;
                value = 64;
            } else {
                value = 96;
            }
            ((struct WarioData*)pose)->yVelocity = value;
            if (gWarioDataCopy.horizontalDirection & DPAD_RIGHT) {
                gWarioData.xVelocity = 0xFFE0;
            } else {
                gWarioData.xVelocity = 32;
            }
            gCurrentCarriedSprite.state = 0;
            m4aSongNumStart(19);
            VoiceSetPlay(4);
            break;

        case 20:
            if (gWarioDataCopy.unk_1A != 2) {
                wario->yVelocity = 64;
            }
            wario->xVelocity = gWarioDataCopy.xVelocity;
            break;

        case 21:
            wario->xVelocity = gWarioDataCopy.xVelocity;
            m4aSongNumStart(30);
            break;

        case 22:
            wario->xVelocity = gWarioDataCopy.xVelocity;
            gCurrentWarioEffect.type = 3;
            m4aSongNumStart(41);
            break;

        case 25:
        {
            struct WarioData* copy;
            u32 zero;

            wario->unk_1F = 8;
            copy = &gWarioDataCopy;
            value = copy->horizontalDirection & DPAD_RIGHT;
            if (value != 0) {
                value = 0xFFE0;
            } else {
                value = 32;
            }
            wario->xVelocity = value;
            index = copy->unk_1A;
            if (index <= 1) {
                zero = copy->unk_08;
                if (zero != 0) {
                    value = 80;
                    wario->yVelocity = value;
                    if (index == 1) {
                        value = wario->yPosition - 2;
                        wario->yPosition = value;
                    }
                } else {
                    value = 48;
                    wario->yVelocity = value;
                    wario->xVelocity = zero;
                }
            } else {
                value = 20;
                wario->yVelocity = value;
            }
            break;
        }

        case 26:
            if (gCurrentCarriedSprite.state != 0) {
                gCurrentCarriedSprite.state = 8;
            }
        case 3:
        case 60:
        {
            struct WarioData* copy;

            copy = &gWarioDataCopy;
            value = *(u16*)&copy->yVelocity;
            wario->yVelocity = value;
            break;
        }

        case 27:
        {
            struct WarioData* copy;

            if (gCurrentCarriedSprite.state != 0) {
                gCurrentCarriedSprite.state = 8;
            }
            copy = &gWarioDataCopy;
            value = *(u16*)&copy->yVelocity;
            wario->yVelocity = value;
            m4aSongNumStart(35);
            break;
        }

        case 30:
            wario->xVelocity = gWarioDataCopy.xVelocity;
            wario->yVelocity = 96;
            if ((gCurrentCarriedSprite.unk1 & 15) != 0) {
                gCurrentCarriedSprite.state = 4;
            }
            break;

        case 31:
            gCurrentCarriedSprite.state = 1;
            gCurrentCarriedSprite.unk1 = 1;
            break;

        case 32:
            gCurrentCarriedSprite.state = 1;
            gCurrentCarriedSprite.unk1 = 2;
            break;

        case 34:
            gCurrentCarriedSprite.state = 7;
            m4aSongNumStart(39);
            break;

        case 37:
            gCurrentCarriedSprite.state = 12;
        case 35:
            m4aSongNumStart(37);
            break;

        case 36:
            m4aSongNumStart(38);
            break;

        case 40:
            index = gWarioDataCopy.xPosition;
            value = 0xFFC0;
            value &= index;
            value += 32;
            wario->xPosition = value;
            break;

        case 41:
            gWarioDustEffect1.unk0 = 11;
            m4aSongNumStart(6);
            break;

        case 46:
            wario->yVelocity = 96;
            break;

        case 48:
            gSubGameMode = 4;
            gCurrentCarriedSprite.state = 0;
            gWarioSubgameXPosition = gWarioDataCopy.xPosition;
            gWarioSubgameYPosition = gWarioDataCopy.yPosition;
            break;

        case 52:
            gCurrentWarioEffect.type = 5;
            break;

        case 54:
            wario->horizontalDirection = DPAD_RIGHT;
        case 53:
        case 58:
            gCurrentWarioEffect.type = 6;
            break;

        case 55:
            wario->horizontalDirection = DPAD_RIGHT;
            gCurrentWarioEffect.type = 7;
            break;

        case 56:
            gCurrentWarioEffect.type = 8;
            break;

        case 64:
        {
            struct WarioData* copy;

            copy = &gWarioDataCopy;
            index = copy->xPosition;
            if (index <= 0x4CF) {
                value = copy->horizontalDirection & DPAD_RIGHT;
                if (value != 0) {
                    goto pose_64_801318C;
                }
                goto pose_63_801318C;
            }
            value = 158 << 3;
            if (index <= value) {
                goto play_voice_801318C;
            }
            value = copy->horizontalDirection & DPAD_RIGHT;
            if (value == 0) {
                goto pose_64_801318C;
            }
pose_63_801318C:
            value = 63;
            wario->pose = value;
            value = 1;
            wario->unk_03 = value;
            goto end_case_64_801318C;

pose_64_801318C:
            value = 64;
            wario->pose = value;
            goto end_case_64_801318C;

play_voice_801318C:
            VoiceSetPlay(2);
end_case_64_801318C:
            break;
        }

        case 66:
            m4aSongNumStart(0x312);
            break;

        case 2:
        case 6:
        case 39:
        case 42:
        case 43:
        case 44:
        case 59:
        case 61:
        case 76:
            break;

        default:
            wario->xVelocity = gWarioDataCopy.xVelocity;
            break;

store_x_velocity_801318C:
            wario->xVelocity = value;
            break;
    }
}

void UpdateNormalWarioMovement(void)
{
    u16 movementOffset;
    u32 temp;
    struct WarioData *wario;

    {
        struct WarioCollisionData *collision;
        const u8 *properties;
        u32 offset;
        register const u8 *property PIN("r1");

        collision = &gWarioCollisionData;
        properties = (const u8 *)sWarioNormalPoseProperties;
        wario = &gWarioData;
        offset = wario->pose;
        offset <<= 3;
        property = properties + 1;
        offset += (u32)property;
        collision->unk_08 = *(const u8 *)offset;
        offset = wario->pose;
        offset <<= 3;
        property = properties + 2;
        offset += (u32)property;
        collision->unk_09 = *(const u8 *)offset;
        offset = wario->pose;
        offset <<= 3;
        properties += 3;
        offset += (u32)properties;
        collision->unk_0A = *(const u8 *)offset;
    }
    UpdateWarioHorizontalCollisionOffset();
    movementOffset = 0;
    temp = wario->pose - 4;
    switch (temp) {
        case 0:
        case 4:
        case 11:
        case 12:
        case 15:
        case 16:
        case 17:
        case 20:
        case 21:
        case 22:
        case 34:
        case 38:
        case 42:
        case 43:
        case 44:
        case 57:
            if (gWarioData.yVelocity <= 0x80) {
                movementOffset = gWarioData.yVelocity >> 3;
            } else {
                movementOffset = 0x10;
            }
            gWarioData.yVelocity -= 8;
            break;

        case 23:
        case 24:
        case 27:
            movementOffset = gWarioData.yVelocity >> 3;
            gWarioData.yVelocity -= 0x10;
            break;

        case 13:
        case 14:
            movementOffset = gWarioData.yVelocity >> 3;
            break;
    }

    gWarioData.yPosition -= movementOffset;
    if ((s32)gWarioData.yVelocity < -0x80) {
        gWarioData.yVelocity = -0x80;
    }

    if (gWarioData.unk_1A == 0) {
        movementOffset = (u16)((s32)(GetAdjustedWarioXVelocity() << 0x10) >> 0x13);
        if (gWarioData.pose == 0 || gWarioData.pose == 0x16) {
            if (gWarioData.horizontalDirection & 0x10) {
                if ((s32)(movementOffset << 0x10) < 0) {
                    movementOffset = 0;
                }
            } else if ((s32)(movementOffset << 0x10) > 0) {
                movementOffset = 0;
            }
        }
    } else {
        movementOffset = (u16)((s32)((u16)gWarioData.xVelocity << 0x10) >> 0x13);
    }

    temp = gWarioData.xPosition;
    temp += movementOffset;
    gWarioData.xPosition = temp;
}

void ResolveNormalWarioCollision(void)
{
    struct WarioData *wario;
    u8 result;

    {
        struct WarioCollisionData *collision;
        const u8 *properties;
        register const u8 *property PIN("r1");
        u32 offset;

        collision = &gWarioCollisionData;
        properties = (const u8 *)sWarioNormalPoseProperties;
        wario = &gWarioData;

        offset = wario->pose * 8;
        property = properties + 4;
        offset += (u32)property;
        collision->unk_0B = *(const u8 *)offset;
        offset = wario->pose * 8;
        property = properties + 5;
        offset += (u32)property;
        collision->unk_0C = *(const u8 *)offset;
        offset = wario->pose * 8;
        property = properties + 6;
        offset += (u32)property;
        collision->unk_0D = *(const u8 *)offset;
        offset = wario->pose * 8;
        properties += 7;
        offset += (u32)properties;
        collision->unk_11 = *(const u8 *)offset;

        if (wario->yVelocity <= 47) {
            collision->unk_0C = 0;
        }
    }

    result = 0xFF;
    switch (wario->pose) {
        case 17:
        case 18:
        case 41:
        case 45:
        case 49:
        case 50:
            break;

        case 40:
            result = ResolveWarioLadderCollision();
            break;

        case 43:
            result = ResolveWarioVerticalFenceCollision();
            break;

        case 44:
            result = ResolveWarioHorizontalFenceCollision();
            break;

        default:
            if (gWarioCollisionData.flags & 0x40) {
                result = ResolveWarioFloorCollision();
            } else if (gWarioCollisionData.flags & 0x80) {
                result = ResolveWarioCeilingCollision();
            } else if (gWarioCollisionData.unk_00 != 0) {
                if (gWarioCollisionData.unk_11 == 2) {
                    result = ResolveWarioCeilingCollision();
                } else {
                    result = ResolveWarioStandardCollision();
                    if (result == 0xFF) {
                        if ((u8)(gWarioData.pose - 6) <= 1) {
                            result = ResolveWarioTileEdgeCollision();
                        } else if (gWarioData.pose == 0) {
                            if (gWarioData.horizontalDirection & 0x10) {
                                if (gWarioData.xVelocity <= 40) {
                                    result = ResolveWarioTileEdgeCollision();
                                }
                            } else if (gWarioData.xVelocity >= -40) {
                                result = ResolveWarioTileEdgeCollision();
                            }
                        }
                    }
                }
            } else if (gWarioCollisionData.unk_11 == 0) {
                result = ResolveWarioLandingCollision();
            }
            break;
    }

    switch (gWarioData.pose) {
        case 22:
        case 23:
        case 24:
        case 25:
            if (result == 0xFC) {
                result = 26;
                if (gWarioCollisionData.unk_0E == 0) {
                    gWarioData.unk_08 = 1;
                }
            }
            break;

        case 12:
            if (gWarioCollisionData.unk_12 != 0) {
                result = 2;
            }
            break;

        case 13:
        case 14:
        case 15:
            if (result == 0xFC) {
                result = 16;
            }
            break;

        case 27:
        case 28:
            if (result == 0xFD) {
                u32 temp;
                s32 collisionResult;

                collisionResult = CheckWarioPointCollision(gWarioCollisionData.unk_08, (u16)(gWarioData.yPosition + 1), (u16 *)&temp);
                if (collisionResult != 0 && collisionResult <= gWarioCollisionData.unk_10 && gWarioData.pose == 28) {
                    result = 0xFF;
                }
            }
            break;
    }

    if (gWarioCollisionData.flags & 0x80) {
        s32 collisionResult;

        collisionResult = GetBackgroundCollisionAtPosition(gWarioData.yPosition, gWarioData.xPosition);
        if ((collisionResult & 0xFF) == 1) {
            gWarioData.reaction = 1;
            if (gWarioData.pose == 28) {
                SetWarioWaterPose(7);
            } else {
                if ((u8)(gWarioData.pose - 19) <= 1) {
                    gWarioData.damageTimer = 0x60;
                }
                SetWarioWaterPose(0);
            }
            return;
        }
    }

    if (gWarioCollisionData.unk_11 != 0xFF) {
        if (result != 0xFF) {
            ApplyNormalWarioPoseTransition(result);
        }
        if (result == 0xFE) {
            gWarioData.yPosition++;
        }
    }
}

u8 CheckWarioTileCollision(u32 xPosition, u32 yPosition, u16 *newYPosition, u16 *newXPosition, u16 *inWater)
{
    u32 x;
    u16 y;
    u32 collisionResult;
    u16 adjustedY;
    u32 adjustedX;
    u8 result;

    {
        u32 xRaw;
        u32 yRaw;

        xRaw = xPosition;
        yRaw = yPosition;
        x = (u16)xRaw;
        y = (u16)yRaw;
    }
    collisionResult = GetWarioBlockCollisionAtPosition(y, x);
    if (collisionResult & 0x01000000) {
        *inWater = 1;
    } else {
        *inWater = 0;
    }

    result = 0;
    switch ((collisionResult & 0xFF) - 2) {
        case 1:
        {
            u32 mask;
            u32 lowMask;
            register u32 base PIN("r1");
            register u32 offset PIN("r0");

            mask = 0xFFC0;
            base = y;
            base &= mask;
            lowMask = 0x3F;
            offset = x;
            offset &= lowMask;
            offset -= 0x3F;
            base -= offset;
            adjustedY = base;
            base = x;
            base &= mask;
            offset = y;
            offset &= lowMask;
            offset -= 0x3F;
            base -= offset;
            adjustedX = (u16)base;
            result = 17;
            break;
        }

        case 4:
        {
            u32 mask;
            u32 lowMask;
            register u32 base PIN("r1");
            register u32 offset PIN("r0");

            mask = 0xFFC0;
            base = y;
            base &= mask;
            lowMask = 0x3F;
            offset = x;
            offset &= lowMask;
            offset >>= 1;
            offset -= 0x3F;
            base -= offset;
            adjustedY = base;
            base = x;
            base &= mask;
            offset = y;
            offset &= lowMask;
            offset <<= 1;
            offset -= 0x7E;
            base -= offset;
            adjustedX = (u16)base;
            result = 18;
            break;
        }

        case 5:
        {
            u32 mask;
            u32 lowMask;
            register u32 base PIN("r1");
            register u32 offset PIN("r0");

            mask = 0xFFC0;
            base = y;
            base &= mask;
            lowMask = 0x3F;
            offset = x;
            offset &= lowMask;
            offset >>= 1;
            offset -= 0x1F;
            base -= offset;
            adjustedY = base;
            base = x;
            base &= mask;
            offset = y;
            offset &= lowMask;
            offset <<= 1;
            offset -= 0x3E;
            base -= offset;
            adjustedX = (u16)base;
            result = 18;
            break;
        }

        case 0:
        {
            u32 mask;
            u32 lowMask;
            register u32 temp PIN("r0");

            mask = 0xFFC0;
            adjustedY = y;
            adjustedY &= mask;
            lowMask = 0x3F;
            temp = x;
            temp &= lowMask;
            adjustedY |= temp;
            adjustedX = x;
            adjustedX &= mask;
            temp = y;
            temp &= lowMask;
            adjustedX |= temp;
            result = 33;
            break;
        }

        {
            u32 mask;
            u32 lowMask;
            u32 base;
            register u32 offset PIN("r0");

        case 3:
            mask = 0xFFC0;
            adjustedY = y & mask;
            lowMask = 0x3F;
            offset = x & lowMask;
            offset >>= 1;
            offset += 0x1F;
            adjustedY |= offset;
            base = x & mask;
            offset = y & lowMask;
            offset <<= 1;
            lowMask = 0xFFC1;
            offset += lowMask;
            goto combineCase2And3;

        case 2:
            mask = 0xFFC0;
            adjustedY = y & mask;
            lowMask = 0x3F;
            offset = x & lowMask;
            offset >>= 1;
            adjustedY |= offset;
            base = x & mask;
            offset = y & lowMask;
            offset <<= 1;

        combineCase2And3:
            base += offset;
            base <<= 16;
            adjustedX = base >> 16;
            result = 34;
            break;
        }

        case 10:
            *inWater = 16;
        default:
        {
            u32 mask;

            mask = 0xFFC0;
            adjustedY = y;
            adjustedY &= mask;
            adjustedX = x;
            adjustedX &= mask;
            result = 0;
            break;
        }
    }

    *newXPosition = adjustedY;
    *newYPosition = adjustedX;
    return result;
}
u32 CheckWarioHorizontalCollision(u32 side, u16 *newXPosition, u32 verticalOffset)
{
    volatile struct AGB_ALIGN { GBA_PTR(u16 *) output; u32 offset; } locals;
    register u32 index PIN("r8");
    u32 result;
    u32 xPosition, yPosition, collision;
    register u32 indexDouble PIN("r10");
    register u32 shiftedOffset PIN("r9");
    GP_SET(locals.output, newXPosition);
    index = (u8)side;
    locals.offset = (u16)verticalOffset;
    result = 0;
    {
    register struct WarioCollisionData *data PIN("r4");
    data = &gWarioCollisionData;
    data->unk_0E = 0;
    {
        register struct WarioData *wario PIN("r3");
        register const u16 *points PIN("r2");
        register u32 offset PIN("r0");
        register u32 row PIN("r1");
        register u32 warioY PIN("r3");
        wario = &gWarioData;
        points = sWarioCollisionPointTable;
        offset = data->unk_04 << 1;
        row = index << 1;
        row += index;
        row <<= 1;
        offset += row;
        offset += (u32)points;
        offset = *(const u16 *)offset;
        offset += wario->xPosition;
        xPosition = (u16)offset;
        points += 2;
        row += (u32)points;
        offset = *(const u16 *)row;
        warioY = wario->yPosition;
        offset += warioY;
        yPosition = (u16)offset;
    }
    collision = GetWarioBlockCollisionAtPosition(yPosition, xPosition);
    if (collision & 0x01000000) {
        switch ((s32)(collision & 0xFF)) {
        case 2: case 3: case 4: case 5: case 6: case 7: break;
        default:
            if (data->unk_0B && ProcessHorizontalTileInteraction(yPosition, xPosition))
                data->unk_0E += 1;
            result += 1;
            break;
        }
    }
    }
    {
        register const s16 *points PIN("r1");
        register u32 row PIN("r0");
        u32 doubled;
        register u32 shifted PIN("r3");
        register s32 height PIN("r12");
        {
            register u32 value PIN("r12");
            value = locals.offset;
            shifted = value << 16;
            height = (s32)shifted >> 16;
        }
        points = (const s16 *)sWarioCollisionPointTable;
        doubled = index << 1;
        row = doubled + index;
        row <<= 1;
        points += 2;
        row += (u32)points;
        row = *(const s16 *)row + 64;
        indexDouble = doubled;
        shiftedOffset = shifted;
        if (height > (s32)row) {
            yPosition = (u16)(yPosition + 64);
            collision = GetWarioBlockCollisionAtPosition(yPosition, xPosition);
    if (collision & 0x01000000) {
        switch ((s32)(collision & 0xFF)) {
        case 2: case 3: case 4: case 5: case 6: case 7: break;
        default:
            if (gWarioCollisionData.unk_0B && ProcessHorizontalTileInteraction(yPosition, xPosition))
                gWarioCollisionData.unk_0E += 2;
            result += 2;
            break;
        }
    }
        }
    }
    {
        const s16 *points;
        u32 row;
        points = (const s16 *)sWarioCollisionPointTable;
        row = indexDouble + index;
        row <<= 1;
        points += 2;
        row += (u32)points;
        if (((s32)shiftedOffset >> 16) > *(const s16 *)row) {
            {
                register u32 value PIN("r0");
                register u32 offset PIN("r5");
                value = gWarioData.yPosition;
                offset = locals.offset;
                value += offset;
                yPosition = (u16)value;
            }
            collision = GetWarioBlockCollisionAtPosition(yPosition, xPosition);
    if (collision & 0x01000000) {
        switch ((s32)(collision & 0xFF)) {
        case 2: case 3: case 4: case 5: case 6: case 7: break;
        default:
            if (gWarioCollisionData.unk_0B && ProcessHorizontalTileInteraction(yPosition, xPosition))
                gWarioCollisionData.unk_0E += 4;
            result += 4;
            break;
        }
    }
        }
    }
    if (result) {
        register u32 value PIN("r1");
        register const u16 *points PIN("r3");
        register struct WarioCollisionData *data PIN("r2");
        register u32 row PIN("r0");
        register u8 sideOffset PIN("r4");
        register u16 *output PIN("r5");
        register u32 adjustment PIN("r2");
        value = 0xFFC0;
        value &= xPosition;
        points = sWarioCollisionPointTable;
        data = &gWarioCollisionData;
        row = indexDouble;
        row += index;
        sideOffset = data->unk_04;
        row += sideOffset;
        row <<= 1;
        row += (u32)points;
        row = *(const u16 *)row;
        value -= row;
        adjustment = data->unk_06;
        value += adjustment;
        output = GP(locals.output);
        *output = value;
    }
    return result;
}

int CheckWarioVerticalCollision(u32 index, u32 offset, u32 mode)
{
    volatile struct AGB_ALIGN {
        u32 index;
        u32 offset;
    } locals;
    u32 previousMask;
    u32 yPosition;
    register struct WarioData *previousWario PIN("r8");
    register const u16 *points PIN("r4");
    register u32 pointOffset PIN("r5");
    u32 xPosition;
    register u32 result PIN("r6");
    u32 collision;

    locals.index = (u8)index;
    locals.offset = (u16)offset;
    mode <<= 24;
    gWarioCollisionData.unk_0F = 0;
    previousMask = 0;
    if (mode == 0) {
        register struct WarioData *warioBase PIN("r2");

        {
            u32 previousY;
            u32 localOffset;

            previousY = gPreviousYPosition;
            localOffset = locals.offset;
            previousY = (localOffset) - (-(previousY));
            yPosition = (u16)previousY;
        }
        warioBase = &gWarioData;
        previousWario = warioBase;
        points = sWarioCollisionSideTable;
        {
            register u32 localIndex PIN("r0");

            localIndex = locals.index;
            pointOffset = localIndex << 3;
        }
        {
            u32 pointValue;
            u32 warioX;

            pointValue = pointOffset;
            pointValue += (u32)points;
            pointValue = *(const u16 *)pointValue;
            warioX = warioBase->xPosition;
            pointValue += warioX;
            xPosition = (u16)pointValue;
        }
        collision = GetWarioBlockCollisionAtPosition(yPosition, xPosition);
        result = 0x01000000;
        collision &= result;
        collision = -collision;
        collision >>= 31;
        previousMask = collision;

        points += 3;
        pointOffset += (u32)points;
        {
            u32 pointValue;
            register struct WarioData *warioBase2 PIN("r2");
            register u32 warioX PIN("r2");

            pointValue = *(const u16 *)pointOffset;
            warioBase2 = previousWario;
            warioX = warioBase2->xPosition;
            pointValue += warioX;
            xPosition = (u16)pointValue;
        }
        collision = GetWarioBlockCollisionAtPosition(yPosition, xPosition);
        collision &= result;
        if (collision != 0) {
            u32 eight;

            eight = 8;
            previousMask += eight;
        }
    }

    result = 0;
    {
        struct WarioData *wario;
        register u32 offsetValue PIN("r2");
        u32 one;
        u32 maskValue;
        register u32 localIndex PIN("r2");

        wario = &gWarioData;
        {
            u32 currentY;

            currentY = wario->yPosition;
            offsetValue = locals.offset;
            currentY = (offsetValue) - (-(currentY));
            yPosition = (u16)currentY;
        }
        one = 1;
        maskValue = previousMask;
        one &= maskValue;
        localIndex = locals.index;
        pointOffset = localIndex << 3;
        if (one == 0) {
            u32 pointValue;
            u32 warioX;
            register u32 collisionMask PIN("r1");

            pointValue = (u32)sWarioCollisionSideTable;
            pointValue = pointOffset + pointValue;
            pointValue = *(const u16 *)pointValue;
            warioX = wario->xPosition;
            pointValue += warioX;
            xPosition = (u16)pointValue;
            collision = GetWarioBlockCollisionAtPosition(yPosition, xPosition);
            collisionMask = 0x01000000;
            collisionMask &= collision;
            if (collisionMask != 0) {
                points = (const u16 *)&gWarioCollisionData;
                if (gWarioCollisionData.unk_0C != 0) {
                    register u32 nibble PIN("r1");

                    collision = ProcessSideTileInteraction(yPosition, xPosition);
                    nibble = 0xF;
                    nibble &= collision;
                    if (nibble != 0)
                        gWarioCollisionData.unk_0F += 1;
                }
                result += 1;
            }
        }
    }

    {
        struct WarioData *wario;
        const u16 *pointAddress;
        register u32 collisionMask PIN("r1");

        wario = &gWarioData;
        pointAddress = sWarioCollisionSideTable;
        pointAddress = (const u16 *)((const u8 *)pointAddress + 2);
        pointAddress = (const u16 *)(pointOffset + (u32)pointAddress);
        {
            u32 pointValue;
            u32 warioX;

            pointValue = *pointAddress;
            warioX = wario->xPosition;
            pointValue += warioX;
            xPosition = (u16)pointValue;
        }
        collision = GetWarioBlockCollisionAtPosition(yPosition, xPosition);
        collisionMask = 0x01000000;
        collisionMask &= collision;
        if (collisionMask != 0) {
            points = (const u16 *)&gWarioCollisionData;
            if (gWarioCollisionData.unk_0C != 0) {
                register u32 nibble PIN("r1");

                collision = ProcessSideTileInteraction(yPosition, xPosition);
                nibble = 0xF;
                nibble &= collision;
                if (nibble != 0)
                    gWarioCollisionData.unk_0F += 2;
            }
            result += 2;
        }
    }

    {
        struct WarioData *wario;
        const u16 *pointAddress;
        register u32 collisionMask PIN("r1");

        wario = &gWarioData;
        pointAddress = sWarioCollisionSideTable;
        pointAddress = (const u16 *)((const u8 *)pointAddress + 4);
        pointAddress = (const u16 *)(pointOffset + (u32)pointAddress);
        {
            u32 pointValue;
            u32 warioX;

            pointValue = *pointAddress;
            warioX = wario->xPosition;
            pointValue += warioX;
            xPosition = (u16)pointValue;
        }
        collision = GetWarioBlockCollisionAtPosition(yPosition, xPosition);
        collisionMask = 0x01000000;
        collisionMask &= collision;
        if (collisionMask != 0) {
            points = (const u16 *)&gWarioCollisionData;
            if (gWarioCollisionData.unk_0C != 0) {
                register u32 nibble PIN("r1");

                collision = ProcessSideTileInteraction(yPosition, xPosition);
                nibble = 0xF;
                nibble &= collision;
                if (nibble != 0)
                    gWarioCollisionData.unk_0F += 4;
            }
            result += 4;
        }
    }

    {
        u32 eight;
        u32 masked;

        eight = 8;
        masked = previousMask;
        masked &= eight;
        if (masked == 0) {
            struct WarioData *wario;
            const u16 *pointAddress;
            register u32 collisionMask PIN("r1");

            wario = &gWarioData;
            pointAddress = sWarioCollisionSideTable;
            pointAddress = (const u16 *)((const u8 *)pointAddress + 6);
            pointAddress = (const u16 *)(pointOffset + (u32)pointAddress);
            {
                u32 pointValue;
                u32 warioX;

                pointValue = *pointAddress;
                warioX = wario->xPosition;
                pointValue += warioX;
                xPosition = (u16)pointValue;
            }
            collision = GetWarioBlockCollisionAtPosition(yPosition, xPosition);
            collisionMask = 0x01000000;
            collisionMask &= collision;
            if (collisionMask != 0) {
                points = (const u16 *)&gWarioCollisionData;
                if (gWarioCollisionData.unk_0C != 0) {
                    register u32 nibble PIN("r1");

                    collision = ProcessSideTileInteraction(yPosition, xPosition);
                    nibble = 0xF;
                    nibble &= collision;
                    if (nibble != 0)
                        gWarioCollisionData.unk_0F += 8;
                }
                result += 8;
            }
        }
    }

    return result;
}

u32 CheckWarioPointCollision(u32 index, u32 yPosition, u16 *secondaryResult)
{
    volatile struct AGB_ALIGN {
        GBA_PTR(u16 *) secondaryResult;
    } locals;
    u32 normalizedIndex;
    u32 normalizedY;
    u32 xPosition;
    u32 result;
    u32 secondary;
    u32 indexTimesTwo;
    u32 collision;

    GP_SET(locals.secondaryResult, secondaryResult);
    normalizedIndex = (u8)index;
    normalizedY = (u16)yPosition;
    gWarioCollisionData.unk_10 = 0;
    result = 0;
    secondary = result;

    {
        struct WarioData *wario;
        const u16 *points;
        u32 pointValue;
        register u32 warioX PIN("r2");

        wario = &gWarioData;
        points = sWarioCollisionPointTable;
        pointValue = normalizedIndex << 1;
        pointValue += normalizedIndex;
        pointValue <<= 1;
        pointValue += (u32)points;
        pointValue = *(const u16 *)pointValue;
        warioX = wario->xPosition;
        pointValue += warioX;
        xPosition = (u16)pointValue;
    }
    collision = GetWarioBlockCollisionAtPosition(normalizedY, xPosition);
    if ((collision & 0x01000000) != 0) {
        if (gWarioCollisionData.unk_0D != 0 && ProcessPointTileInteraction(normalizedY, xPosition) != 0)
            gWarioCollisionData.unk_10 += 1;
        result += 1;
    } else if ((collision & 0xFF) == 12) {
        result = 1;
        secondary = result;
    }

    {
        struct WarioData *wario;
        const u16 *points;
        u32 indexDouble;
        u32 pointValue;
        register u32 warioX PIN("r2");

        wario = &gWarioData;
        points = sWarioCollisionPointTable;
        indexDouble = normalizedIndex << 1;
        pointValue = indexDouble + normalizedIndex;
        pointValue <<= 1;
        points++;
        pointValue += (u32)points;
        pointValue = *(const u16 *)pointValue;
        warioX = wario->xPosition;
        pointValue += warioX;
        xPosition = (u16)pointValue;
        collision = GetWarioBlockCollisionAtPosition(normalizedY, xPosition);
        {
            u32 collisionMask;

            collisionMask = 0x01000000;
            collisionMask &= collision;
            indexTimesTwo = indexDouble;
            if (collisionMask != 0)
                goto secondSolid;
        }
        goto secondNotSolid;
    }
secondSolid:
    {
        if (gWarioCollisionData.unk_0D != 0 && ProcessPointTileInteraction(normalizedY, xPosition) != 0)
            gWarioCollisionData.unk_10 += 2;
        result += 2;
    }
    goto secondDone;
secondNotSolid:
    if ((collision & 0xFF) == 12) {
        result += 2;
        secondary = (u16)(secondary + 2);
    }
secondDone:

    {
        const s16 *points;
        u32 pointOffset;
        u32 zero;

        points = (const s16 *)sWarioCollisionPointTable;
        pointOffset = indexTimesTwo + normalizedIndex;
        pointOffset <<= 1;
        points++;
        pointOffset += (u32)points;
        zero = 0;
        if (*(const s16 *)pointOffset > 31) {
            xPosition = gWarioData.xPosition;
            collision = GetWarioBlockCollisionAtPosition(normalizedY, xPosition);
            if ((collision & 0x01000000) != 0) {
                if (gWarioCollisionData.unk_0D != 0 && ProcessPointTileInteraction(normalizedY, xPosition) != 0)
                    gWarioCollisionData.unk_10 += 4;
                result += 4;
            } else if ((collision & 0xFF) == 12) {
                result += 4;
                secondary = (u16)(secondary + 4);
            }
        }
    }

    {
        register u32 outputValue PIN("r1");
        u16 *output;

        outputValue = secondary;
        output = GP(locals.secondaryResult);
        *output = outputValue;
    }
    return result;
}

u8 ResolveWarioStandardCollision(void)
{
    u16 adjX;
    u16 adjY;
    u16 sec;
    u32 flags = gWarioCollisionData.unk_00;
    u32 side = gWarioCollisionData.unk_08;
    u32 side2 = side;
    u32 left = gWarioCollisionData.unk_04;
    u32 right = gWarioCollisionData.unk_05;
    u32 result;
    u16 x;
    u16 y;
    u32 cur = (u16)gWarioData.unk_1C;

    if (cur == 0) {
        result = CheckWarioHorizontalCollision(side, &adjX, -62);
        if (gWarioData.reaction == 0) {
            if (gWarioData.pose == 0 || gWarioData.pose == 2) {
                flags &= gWarioData.horizontalDirection;
                if (flags != 0 && result == 1) {
                    return 5;
                }
            }
        }
        if (result != 0) {
            gWarioData.xPosition = adjX;
            if (gWarioCollisionData.unk_0B == 0) {
                gWarioData.xVelocity = 0;
                gWarioCollisionData.unk_12++;
            } else if (gWarioCollisionData.unk_0B == 1) {
                return 0xFC;
            } else {
                if ((s32)result > (s32)gWarioCollisionData.unk_0E) {
                    return 0xFC;
                }
                side2 = gWarioCollisionData.unk_0A;
            }
        } else {
            side2 = gWarioCollisionData.unk_0A;
        }

        {
            const u16 *points = sWarioCollisionPointTable;
            u32 leftOffset = left * 2;
            u32 sideDouble = side * 2;
            u32 offset = leftOffset + (sideDouble + side) * 2;
            x = gWarioData.xPosition + *(const u16 *)((u32)points + offset);
        }
        y = gWarioData.yPosition;
        result = GBA_CALL_AS(CheckWarioTileCollision, u32 (*)(u32, u32, u16 *, u16 *, u16 *))(x, y, &adjX, &adjY, &sec);
        if (result != 0 && sec != 0) {
            gWarioData.unk_1C = result;
            gWarioData.yPosition = adjY;
            gWarioData.unk_1A = 0;
            return 0xFF;
        }

        {
            struct WarioData *wario = &gWarioData;
            const u16 *points = sWarioCollisionPointTable;
            u32 offset = left * 2 + side2 * 6;
            x = wario->xPosition + *(const u16 *)((u32)points + offset);
        }
        y = y + 1;
        CheckWarioTileCollision(x, y, &adjX, &adjY, &sec);
        if (sec != 0) {
            goto clear;
        }

        x = gWarioData.xPosition + WARIO_POINT(side, right);
        result = GBA_CALL_AS(CheckWarioTileCollision, u32 (*)(u32, u32, u16 *, u16 *, u16 *))(x, y, &adjX, &adjY, &sec);
        if (result != 0) {
            gWarioData.unk_1C = result;
            gWarioData.yPosition = adjY;
            gWarioData.unk_1A = 0;
            return 0xFF;
        }

        x = gWarioData.xPosition + WARIO_POINT(side2, right);
        CheckWarioTileCollision(x, y, &adjX, &adjY, &sec);
        if (sec != 0) {
            goto clear;
        }
        if (gWarioData.unk_1A == 1) {
            return 0xFF;
        }
        CheckWarioTileCollision(gWarioData.xPosition, y, &adjX, &adjY, &sec);
        if (sec != 0) {
            goto clear;
        }
        return 0xFE;
    clear:
        gWarioData.unk_1A = 0;
        return 0xFF;
    }

    if ((cur & flags) != 0) {
        result = CheckWarioHorizontalCollision(side, &adjX, -62);
        if (result != 0) {
            gWarioData.xPosition = adjX;
            if (gWarioCollisionData.unk_0B == 0) {
                gWarioData.xVelocity = 0;
                gWarioCollisionData.unk_12++;
            } else if (gWarioCollisionData.unk_0B == 1 || (s32)result > (s32)gWarioCollisionData.unk_0E) {
                return 0xFC;
            }
        }
        {
        struct WarioData *wario = &gWarioData;
        const u16 *points = sWarioCollisionPointTable;
        register u16 *newY PIN("r5");
        register u16 *water PIN("r4");
        {
            register u32 offset PIN("r1") = (side * 3 + left) * 2;
            x = wario->xPosition + *(const u16 *)((u32)points + offset);
        }
        y = wario->yPosition;
        newY = &adjY;
        water = &sec;
        result = GBA_CALL_AS(CheckWarioTileCollision, u32 (*)(u32, u32, u16 *, u16 *, u16 *))(x, y, &adjX, newY, water);
        wario->unk_1C = result;
        if (result == 0) {
            if (*water == 0) {
                return 0xFF;
            }
            y = y - 64;
            result = GBA_CALL_AS(CheckWarioTileCollision, u32 (*)(u32, u32, u16 *, u16 *, u16 *))(x, y, &adjX, newY, water);
            wario->unk_1C = result;
        }
        if (result != 0) {
            wario->yPosition = *newY;
            return 0xFF;
        }
        wario->yPosition = *newY + 63;
        return 0xFF;
        }
    }

    if (gWarioData.unk_1A == 1) {
        gWarioData.unk_1C = 0;
        return 0xFF;
    }

    {
        const u16 *points = sWarioCollisionPointTable;
        u32 offset = right * 2 + side * 6;
        x = gWarioData.xPosition + *(const u16 *)((u32)points + offset);
    }
    y = gWarioData.yPosition;
    result = GBA_CALL_AS(CheckWarioTileCollision, u32 (*)(u32, u32, u16 *, u16 *, u16 *))(x, y, &adjX, &adjY, &sec);
    if (result != 0) {
        gWarioData.yPosition = adjY;
        return 0xFF;
    }
    if (sec != 0) {
        return 0xFF;
    }
    y = y + 64;
    result = GBA_CALL_AS(CheckWarioTileCollision, u32 (*)(u32, u32, u16 *, u16 *, u16 *))(x, y, &adjX, &adjY, &sec);
    gWarioData.unk_1C = result;
    if (result != 0) {
        gWarioData.yPosition = adjY;
        return 0xFF;
    }
    if (sec == 0) {
        return 0xFF;
    }
    if (gWarioData.horizontalDirection & 0x10) {
        gWarioData.xPosition = adjX - WARIO_POINT(side, 0);
    } else {
        gWarioData.xPosition = adjX - WARIO_POINT(side, 1) + 63;
    }
    gWarioData.yPosition = adjY - 1;
    return 0xFF;
}

u8 ResolveWarioLandingCollision(void)
{
    volatile struct AGB_ALIGN {
        u16 newX;
        u16 newY;
        u16 inWater;
        u16 padding;
    } locals;
    u32 savedY;
    u32 r4Value;
    register u32 r5Value PIN("r5");
    register u32 r6Value PIN("r6");
    u32 r7Value;
    u32 yPosition;
    register struct WarioData *wario PIN("r9");
    const u16 *points;
    u32 result;

    r5Value = gWarioCollisionData.unk_08;
    r4Value = gWarioCollisionData.unk_09;
    {
        const u8 *data;
        u32 address;

        data = (const u8 *)sWarioCollisionPointTable;
        address = r5Value << 1;
        address += r5Value;
        address <<= 1;
        data += 4;
        address += (u32)data;
        result = *(const u16 *)address;
    }
    result = CheckWarioVerticalCollision(r4Value, result, 1);
    if (result == 1 || result == 3) {
        register struct WarioData *currentWario PIN("r2");
        const u8 *data;
        register u32 address PIN("r0");
        register u32 currentX PIN("r3");

        currentWario = &gWarioData;
        data = (const u8 *)sWarioCollisionSideTable;
        address = r4Value << 3;
        address += (u32)data;
        result = *(const u16 *)address;
        currentX = currentWario->xPosition;
        address = result + currentX;
        r4Value = (u16)address;
        address = 0xFFC0;
        r4Value &= address;
        result = r4Value - result;
        result += 0x40;
        goto storeAdjustedX;
    }
    if (result == 8 || result == 12) {
        register struct WarioData *currentWario PIN("r2");
        const u8 *data;
        register u32 address PIN("r0");
        register u32 currentX PIN("r3");

        currentWario = &gWarioData;
        data = (const u8 *)sWarioCollisionSideTable;
        result = r4Value << 3;
        data += 6;
        result += (u32)data;
        result = *(const u16 *)result;
        currentX = currentWario->xPosition;
        address = result + currentX;
        r4Value = (u16)address;
        address = 0xFFC0;
        r4Value &= address;
        result = r4Value - result;
        result -= 1;
storeAdjustedX:
        currentWario->xPosition = result;
    }

    wario = &gWarioData;
    points = sWarioCollisionPointTable;
    {
        u32 address;
        register u32 currentX PIN("r2");

        address = r5Value << 1;
        address += r5Value;
        r7Value = address << 1;
        address = r7Value + (u32)points;
        address = *(const u16 *)address;
        currentX = (u32)wario;
        currentX = ((struct WarioData *)currentX)->xPosition;
        address += currentX;
        r4Value = (u16)address;
    }
    {
        register struct WarioData *currentWario PIN("r3");
        u32 currentY;

        currentWario = wario;
        currentY = currentWario->yPosition;
        currentY += 1;
        yPosition = (u16)currentY;
    }
    savedY = yPosition;
    r6Value = (u32)&locals.newY;
    r5Value = (u32)&locals.inWater;
    result = GBA_CALL_AS(CheckWarioTileCollision, u32 (*)(u32, u32, u16 *, u16 *, u16 *))(
        r4Value, yPosition, (u16 *)&locals.newX, (u16 *)r6Value, (u16 *)r5Value);
    if (*(u16 *)r5Value == 0)
        goto secondPoint;
    if (result == 0)
        goto retryFirstPoint;
    {
        register struct WarioData *storeWario PIN("r2");

        storeWario = wario;
        storeWario->unk_1C = result;
        storeWario->yPosition = *(u16 *)r6Value;
    }
    goto returnFF;

retryFirstPoint:
    r7Value = *(u16 *)r6Value;
    result = GBA_CALL_AS(CheckWarioTileCollision, u32 (*)(u32, u32, u16 *, u16 *, u16 *))(
        (u16)(r4Value + 1), (u16)(yPosition + 1), (u16 *)&locals.newX,
        (u16 *)r6Value, (u16 *)r5Value);
    {
        register struct WarioData *storeWario PIN("r3");

        storeWario = wario;
        storeWario->unk_1C = result;
        if (result != 0) {
            storeWario->yPosition = r7Value;
        } else {
            struct WarioData *zeroStoreWario;
            register u32 adjustedY PIN("r0");

            adjustedY = r7Value - 1;
            zeroStoreWario = wario;
            ASM_BARRIER(: : "r"(adjustedY), "r"(zeroStoreWario) : "memory");
            zeroStoreWario->yPosition = adjustedY;
        }
    }
    goto returnFF;

secondPoint:
    {
        u32 address;
        register u32 currentX PIN("r2");

        address = (u32)points;
        address += 2;
        address = r7Value + address;
        r7Value = *(const u16 *)address;
        currentX = (u32)wario;
        currentX = ((struct WarioData *)currentX)->xPosition;
        address = r7Value + currentX;
        r4Value = (u16)address;
    }
    result = GBA_CALL_AS(CheckWarioTileCollision, u32 (*)(u32, u32, u16 *, u16 *, u16 *))(
        r4Value, yPosition, (u16 *)&locals.newX, (u16 *)r6Value, (u16 *)r5Value);
    if (*(u16 *)r5Value == 0)
        goto centerPoint;
    if (result == 0)
        goto retrySecondPoint;
    {
        register struct WarioData *storeWario PIN("r3");

        storeWario = wario;
        storeWario->unk_1C = result;
        storeWario->yPosition = *(u16 *)r6Value;
    }
    goto returnFF;

retrySecondPoint:
    r7Value = *(u16 *)r6Value;
    result = GBA_CALL_AS(CheckWarioTileCollision, u32 (*)(u32, u32, u16 *, u16 *, u16 *))(
        (u16)(r4Value - 1), (u16)(yPosition + 1), (u16 *)&locals.newX,
        (u16 *)r6Value, (u16 *)r5Value);
    {
        struct WarioData *storeWario;

        storeWario = wario;
        storeWario->unk_1C = result;
        if (result != 0) {
            storeWario->yPosition = r7Value;
        } else {
            struct WarioData *zeroStoreWario;
            register u32 adjustedY PIN("r0");

            adjustedY = r7Value - 1;
            zeroStoreWario = wario;
            zeroStoreWario->yPosition = adjustedY;
        }
    }
    goto returnFF;

centerPoint:
    if ((s16)r7Value > 31) {
        register struct WarioData *centerWario PIN("r2");

        centerWario = &gWarioData;
        r4Value = centerWario->xPosition;
        result = GBA_CALL_AS(CheckWarioTileCollision, u32 (*)(u32, u32, u16 *, u16 *, u16 *))(
            r4Value, savedY, (u16 *)&locals.newX, (u16 *)r6Value, (u16 *)r5Value);
        if (*(u16 *)r5Value != 0) {
            register struct WarioData *storeWario PIN("r3");
            u32 adjustedY;

            storeWario = &gWarioData;
            storeWario->unk_1C = result;
            adjustedY = *(u16 *)r6Value;
            adjustedY -= 1;
            storeWario->yPosition = adjustedY;
            goto returnFF;
        }
    }
    goto checkPose;
returnFF:
    return 0xFF;
checkPose:
    if (gWarioData.unk_1A == 1)
        goto returnFF;
    return 0xFE;
}

u8 ResolveWarioCeilingCollision(void)
{
    u16 adjX;
    u16 adjY;
    u16 sec;
    u32 side = gWarioCollisionData.unk_08;
    u32 left = gWarioCollisionData.unk_04;
    u32 right = gWarioCollisionData.unk_05;
    s16 offset = gWarioCollisionData.unk_06;
    u32 result;
    u32 x;
    u32 y;
    u32 savedX;
    u32 currentY;
    u32 previousY;
    u16 pointValue;

    if (gWarioCollisionData.unk_00 != 0) {
        result = CheckWarioHorizontalCollision(side, &adjX, -22);
        if (result != 0) {
            gWarioData.xPosition = adjX;
            if (gWarioCollisionData.unk_0B == 0) {
                gWarioData.xVelocity = 0;
                gWarioCollisionData.unk_12++;
                goto main;
            }
            if (gWarioCollisionData.unk_0B == 1 || (s32)result > gWarioCollisionData.unk_0E) {
                u32 initialX;
                u32 initialY;
                initialX = (u16)(adjX + sWarioCollisionPointTable[side * 3 + left]);
                gWarioData.yPosition = (gWarioData.yPosition + gPreviousYPosition) >> 1;
                initialY = (u16)(gWarioData.yPosition + 1);
                CheckWarioTileCollision(initialX, initialY, &adjX, &adjY, &sec);
                if (sec == 0) goto returnFC;
                if (!(sec & 0x10)) goto adjustFC;
                currentY = gWarioData.yPosition;
                previousY = gPreviousYPosition;
                goto compareFC;
            }
        }
    }

main:
    if (gWarioData.unk_1A == 1) {
        return 0xFF;
    }

    pointValue = sWarioCollisionPointTable[side * 3 + left];
    x = (u16)(pointValue + gWarioData.xPosition);
    y = gWarioData.yPosition;
    savedX = x;
    result = GBA_CALL_AS(CheckWarioTileCollision, u32 (*)(u32, u32, u16 *, u16 *, u16 *))(x, y, &adjX, &adjY, &sec);
    if (sec != 0) {
        if (result != 0) {
            gWarioData.unk_1C = result;
            gWarioData.yPosition = adjY;
            return 0xFD;
        }
        if (gPreviousYPosition < adjY) {
            y = gPreviousYPosition;
            result = GBA_CALL_AS(CheckWarioTileCollision, u32 (*)(u32, u32, u16 *, u16 *, u16 *))(x, y, &adjX, &adjY, &sec);
            gWarioData.unk_1C = result;
            if (result != 0) {
                gWarioData.yPosition = adjY;
            } else {
                gWarioData.yPosition = adjY + 63;
            }
            return 0xFD;
        }
        if (sec != 0x10) {
            gWarioData.xPosition = offset + (adjX - pointValue);
            x = gWarioData.xPosition;
            result = GBA_CALL_AS(CheckWarioTileCollision, u32 (*)(u32, u32, u16 *, u16 *, u16 *))(x, y, &adjX, &adjY, &sec);
            gWarioData.unk_1C = result;
            if (result != 0) {
                gWarioData.yPosition = adjY;
                return 0xFD;
            }
            if (sec != 0) {
                gWarioData.yPosition = adjY - 1;
                return 0xFD;
            }
            if (gWarioCollisionData.unk_0B == 0) {
                gWarioData.xVelocity = result;
                return 0xFF;
            }
            result = ProcessHorizontalTileInteraction(y, savedX);
            if (result != 0 && gWarioCollisionData.unk_0B != 1) {
                return 0xFF;
            }
            CheckWarioTileCollision(gWarioData.xPosition, (u16)(gWarioData.yPosition + 1), &adjX, &adjY, &sec);
            if (sec == 0) goto returnFC;
            if (!(sec & 0x10)) goto adjustFC;
            currentY = gWarioData.yPosition;
            previousY = gPreviousYPosition;
            compareFC:
            if ((currentY & 0xFFC0) == (previousY & 0xFFC0)) goto returnFC;
adjustFC:
            gWarioData.yPosition = adjY - 2;
returnFC:
            return 0xFC;
        }
    }

    x = (u16)(*(const u16 *)((u32)sWarioCollisionPointTable + (right << 1) + ((side * 3) << 1)) + gWarioData.xPosition);
    y = gWarioData.yPosition;
    result = GBA_CALL_AS(CheckWarioTileCollision, u32 (*)(u32, u32, u16 *, u16 *, u16 *))(x, y, &adjX, &adjY, &sec);
    if (sec != 0) {
        if (result != 0) {
            gWarioData.unk_1C = result;
            gWarioData.yPosition = adjY;
            return 0xFD;
        }
        if (gPreviousYPosition >= adjY) {
            return 0xFF;
        }
        y = gPreviousYPosition;
        result = GBA_CALL_AS(CheckWarioTileCollision, u32 (*)(u32, u32, u16 *, u16 *, u16 *))(x, y, &adjX, &adjY, &sec);
        gWarioData.unk_1C = result;
        if (result != 0) {
            gWarioData.yPosition = adjY;
        } else {
            gWarioData.yPosition = adjY + 63;
        }
        return 0xFD;
    }
    if (*(const s16 *)((u32)(sWarioCollisionPointTable + 1) + ((side * 3) << 1)) > 31) {
        x = gWarioData.xPosition;
        result = GBA_CALL_AS(CheckWarioTileCollision, u32 (*)(u32, u32, u16 *, u16 *, u16 *))(x, y, &adjX, &adjY, &sec);
        if (sec != 0 && gPreviousYPosition < adjY) {
            gWarioData.unk_1C = result;
            gWarioData.yPosition = adjY - 1;
            return 0xFD;
        }
    }
    return 0xFF;
}

u8 ResolveWarioFloorCollision(void)
{
    struct AGB_ALIGN {
        u16 adjustedX;
        u16 adjustedY;
        u16 secondary;
    } outputs;
    u16 *adjustedXOutput;
    u32 index;
    u32 secondaryIndex;
    u32 pointIndex;
    s32 result;
    s32 tile;

    {
        struct WarioCollisionData *collisionData;
        u32 loadedIndex;
        struct WarioData *wario;

        collisionData = &gWarioCollisionData;
        loadedIndex = collisionData->unk_08;
        index = loadedIndex;
        secondaryIndex = collisionData->unk_09;
        pointIndex = collisionData->unk_04;
        wario = &gWarioData;

        if (*(u16 *)&wario->reaction == 0x1400) {
            if (((u32 (*)(u32, u32, u32))CheckWarioVerticalCollision)(0, sWarioCollisionPointTable[2], 0) != 0) {
                gWarioCollisionDamageFlag = 1;
            } else {
                wario->pose = WPOSE_NORMAL_TAKING_DAMAGE;
            }
        }
    }

    if (gWarioCollisionDamageFlag == 0) {
        const u16 *points;
        u32 pointOffset;
        u32 pointValue;
        struct WarioCollisionData *collisionData;

        points = sWarioCollisionPointTable;
        {
            u32 offsetTemp;

            offsetTemp = index << 1;
            offsetTemp += index;
            pointOffset = offsetTemp << 1;
        }
        {
            const u16 *pointPtr;

            pointPtr = points + 2;
            pointPtr = (const u16 *)(pointOffset + (u32)pointPtr);
            pointValue = *pointPtr;
        }
        result = ((s32 (*)(u32, u32, u32))CheckWarioVerticalCollision)(secondaryIndex, pointValue, 0);
        collisionData = &gWarioCollisionData;

        if (collisionData->unk_0F != 0) {
            struct WarioData *wario;
            register u32 aligned PIN("r4");

            wario = &gWarioData;
            {
                u32 raw;

                raw = wario->yPosition + pointValue;
                raw <<= 16;
                aligned = raw >> 16;
                aligned &= 0xFFC0;
                raw = aligned - pointValue;
                raw += 0x40;
                wario->yPosition = raw;
            }
            if (collisionData->unk_0C <= 1 || result > (s32)collisionData->unk_0F) {
                collisionData->unk_13++;
            }
        } else if (result != 0) {
            if (collisionData->unk_0C <= 1) {
                if (result == 1) {
                    struct WarioData *wario;
                    u32 aligned;
                    u32 sideOffset;

                    wario = &gWarioData;
                    {
                        const u16 *sidePtr;
                        u32 raw;

                        sidePtr = (const u16 *)(pointOffset + (u32)points);
                        sideOffset = *sidePtr;
                        aligned = wario->xPosition;
                        raw = sideOffset + aligned;
                        aligned = (u16)raw;
                        aligned &= 0xFFC0;
                        sideOffset = aligned - sideOffset;
                        sideOffset += 0x40;
                        wario->xPosition = sideOffset;
                    }
                } else if (result == 8) {
                    struct WarioData *wario;
                    u32 aligned;
                    u32 sideOffset;

                    wario = &gWarioData;
                    {
                        const u16 *sidePtr;
                        u32 raw;

                        sidePtr = points + 1;
                        sidePtr = (const u16 *)(pointOffset + (u32)sidePtr);
                        sideOffset = *sidePtr;
                        aligned = wario->xPosition;
                        raw = sideOffset + aligned;
                        aligned = (u16)raw;
                        aligned &= 0xFFC0;
                        sideOffset = aligned - sideOffset;
                        sideOffset--;
                        wario->xPosition = sideOffset;
                    }
                } else if ((result & 6) != 0) {
                    struct WarioData *wario;
                    register u32 aligned PIN("r4");
                    u32 raw;

                    wario = &gWarioData;
                    raw = wario->yPosition + pointValue;
                    raw <<= 16;
                    aligned = raw >> 16;
                    aligned &= 0xFFC0;
                    raw = aligned - pointValue;
                    raw += 0x40;
                    wario->yPosition = raw;
                    collisionData->unk_13++;
                }
            } else {
                struct WarioData *wario;
                register u32 aligned PIN("r4");
                u32 raw;

                wario = &gWarioData;
                raw = wario->yPosition + pointValue;
                raw <<= 16;
                aligned = raw >> 16;
                aligned &= 0xFFC0;
                raw = aligned - pointValue;
                raw += 0x40;
                wario->yPosition = raw;
                collisionData->unk_13++;
            }
        }
    }

    {
        struct WarioData *wario;

        wario = &gWarioData;
        if (*(u16 *)&wario->reaction == 0x0400 && (gButtonsHeld & 0x40) != 0 && gWarioCollisionData.unk_13 != 0) {
            tile = GetBackgroundCollisionAtPosition((u16)(wario->yPosition - 0x80), wario->xPosition) >> 16;
            if (tile == 9) {
                wario->xPosition = (wario->xPosition & 0xFFC0) + 0x40;
                return WPOSE_NORMAL_ENTERING_PIPE_UP;
            }
            if (tile == 10) {
                wario->xPosition &= 0xFFC0;
                return WPOSE_NORMAL_ENTERING_PIPE_UP;
            }
        }
    }

    {
        struct WarioCollisionData *collisionData;
        struct WarioCollisionData *loadedCollision;

        loadedCollision = &gWarioCollisionData;
        collisionData = loadedCollision;
        if (loadedCollision->unk_00 != 0) {
            struct WarioData *activeWario;
            register u32 xPosition PIN("r3");
            register u32 yPosition PIN("r4");
            register u32 pointValue PIN("r8");
            register u16 *adjustedYPtr PIN("r6");
            register u16 *secondaryPtr PIN("r5");

            activeWario = &gWarioData;
            pointValue = sWarioCollisionPointTable[index * 3 + pointIndex];
            {
                register u32 rawX PIN("r0");

                rawX = activeWario->xPosition + pointValue;
                xPosition = (u16)rawX;
            }
            yPosition = activeWario->yPosition;
            adjustedXOutput = &outputs.adjustedX;
            adjustedYPtr = &outputs.adjustedY;
            secondaryPtr = &outputs.secondary;
            result = GBA_CALL_AS(CheckWarioTileCollision, s32 (*)(u32, u32, u16 *, u16 *, u16 *))(
                xPosition, yPosition, adjustedXOutput, adjustedYPtr, secondaryPtr);

            if (*secondaryPtr != 0) {
                if (result != 0) {
                    activeWario->unk_1C = result;
                    goto secondaryResolved;
                }
                {
                    u32 adjustedValue;
                    register u32 currentX PIN("r3");

                    adjustedValue = outputs.adjustedX;
                    currentX = pointValue;
                    ASM_BARRIER(: "+r"(currentX));
                    pointValue = (u16)(adjustedValue - currentX + collisionData->unk_06);
                    currentX = activeWario->xPosition;
                    result = GBA_CALL_AS(CheckWarioTileCollision, s32 (*)(u32, u32, u16 *, u16 *, u16 *))(
                        currentX, yPosition, &outputs.adjustedX, adjustedYPtr, secondaryPtr);
                }
                if (result == 0)
                    goto secondaryMissed;
                activeWario->unk_1C = result;
                activeWario->xPosition = pointValue;
secondaryResolved:
                activeWario->yPosition = *adjustedYPtr;
                return 0xFD;
            }
secondaryMissed:
            result = ((s32 (*)(u32, u16 *, u32))CheckWarioHorizontalCollision)(index, &outputs.adjustedX, 0);
            if (result != 0) {
                gWarioData.xPosition = outputs.adjustedX;
                if (gWarioCollisionData.unk_0B == 0) {
                    gWarioData.xVelocity = 0;
                    gWarioCollisionData.unk_12++;
                } else if (gWarioCollisionData.unk_0B == 1 || result > (s32)gWarioCollisionData.unk_0E) {
                    return 0xFC;
                }
            }
        }
    }

    {
        struct WarioCollisionData *collisionData;

        collisionData = &gWarioCollisionData;
        if (collisionData->unk_13 != 0 && collisionData->unk_0C <= 1) {
            if (*(u16 *)&gWarioData.reaction == 0x0800 && gWarioData.yVelocity > 110) {
                gWarioData.xVelocity = 0;
                gWarioData.yVelocity = -gWarioData.yVelocity;
            } else {
                gWarioData.yVelocity = 0;
            }
        }
    }
    return 0xFF;
}

u8 ResolveWarioTileEdgeCollision(void)
{
    struct WarioData *wario;
    u32 mask;
    u16 *previousX;
    u16 newY;
    u16 newX;
    u16 inWater;
    u16 currentLow;
    u16 previousLow;

    wario = &gWarioData;
    {
        u32 current;
        register u32 maskedCurrent PIN("r1");
        register u32 previous PIN("r2");

        current = wario->xPosition;
        mask = 0xFFC0;
        previousX = &gPreviousXPosition;
        previous = *previousX;
        maskedCurrent = mask;
        maskedCurrent &= current;
        current = mask;
        current &= previous;
        if (maskedCurrent != current) {
            return 0xFF;
        }
    }
    if (*(u16 *)&wario->unk_1C != 0 || wario->unk_1A != 0) {
        return 0xFF;
    }
    {
        u32 xPosition;
        register u32 yRaw PIN("r0");
        u32 yPosition;

        xPosition = *(volatile u16 *)&wario->xPosition;
        yRaw = wario->yPosition;
        yRaw++;
        yRaw <<= 16;
        yPosition = yRaw >> 16;
        if (GBA_CALL_AS(CheckWarioTileCollision, u32 (*)(u32, u32, u16 *, u16 *, u16 *))(xPosition, yPosition, &newY, &newX, &inWater) != 0) {
            return 0xFF;
        }
    }
    if (inWater != 0) {
        return 0xFF;
    }

    currentLow = wario->xPosition & 0x3F;
    previousLow = *previousX & 0x3F;
    if (*(u16 *)&gWarioCollisionData & 0x10) {
        if (currentLow > 29 && previousLow <= 30) {
            wario->xPosition = (wario->xPosition & mask) + 30;
            return 0xFE;
        }
    } else if (*(u16 *)&gWarioCollisionData & 0x20) {
        if (currentLow <= 33 && previousLow > 32) {
            wario->xPosition = (wario->xPosition & mask) + 33;
            return 0xFE;
        }
    }
    return 0xFF;
}

u8 ResolveWarioLadderCollision(void)
{
      volatile struct AGB_ALIGN 
      {
          u16 newX;
          u16 newY;
          u16 water;
      } locals;
      u32 probeY;
      struct WarioData *wario;
      u32 index;
      u32 collision;
      u32 yPosition;
      u32 xPosition;
      register u32 rawIndex PIN("r2");
      s32 tile;
    {
      volatile u8 *collisionData;
      u16 rawCollision;
      collisionData = (volatile u8 *) (&gWarioCollisionData);
      rawIndex = collisionData[8];
      index = rawIndex;
      wario = &gWarioData;
      xPosition = wario->xPosition;
      rawCollision = *((volatile u16 *) (collisionData + 2));
      {
          u32 mask;
          mask = 0x40;
          mask &= rawCollision;
          collision = (u16)mask;
      }
    }
    if (collision != 0)
    {
      {
          u32 tableOffset;
          const u8 *tableData;

          tableData = (const u8 *)sWarioCollisionPointTable;
          tableOffset = rawIndex << 1;
          tableOffset += rawIndex;
          tableOffset <<= 1;
          tableData += 4;
          tableOffset += (u32)tableData;
          index = *(const u16 *)tableOffset;
      }
      {
          u32 sum;

          sum = wario->yPosition;
          sum += index;
          yPosition = (u16)sum;
      }
      ;
      GBA_CALL_AS(CheckWarioTileCollision, u32 (*)(u32, u32, u16 *, u16 *, u16 *))(xPosition, yPosition, (u16 *) (&locals.newX), (u16 *) (&locals.newY), (u16 *) (&locals.water));
      probeY = yPosition;
      if (locals.water == 1)
      {
        wario->yPosition = (locals.newY - index) + 0x40;
        return 0xFF;
      }
      {
          u32 tileY;
          u32 tileX;

          tileX = wario->xPosition;
          tileY = probeY;
          ASM_BARRIER(: : "r"(tileX), "r"(tileY));
          tile = GetBackgroundCollisionAtPosition(tileY, tileX) >> 16;
      }
      if (((u32) (tile - 2)) <= 1)
      {
        return 0xFF;
      }
      tile = GetBackgroundCollisionAtPosition(wario->yPosition, wario->xPosition) >> 16;
      if (((u32) (tile - 2)) <= 1)
      {
        return 0xFF;
      }
      wario->yPosition = (gPreviousYPosition & 0xFFC0) - 1;
      return 2;
    }
    yPosition = wario->yPosition;
    ;
    GBA_CALL_AS(CheckWarioTileCollision, u32 (*)(u32, u32, u16 *, u16 *, u16 *))(xPosition, yPosition, (u16 *) (&locals.newX), (u16 *) (&locals.newY), (u16 *) (&locals.water));
    if (locals.water == 1)
    {
      wario->yPosition = (wario->yPosition & 0xFFC0) - 1;
      return 2;
    }
    *(u16 *)&locals.water = collision;
    yPosition = wario->yPosition;
    {
        u32 tileY;
        u32 tileX;

        tileX = wario->xPosition;
        tileY = yPosition;
        ASM_BARRIER(: : "r"(tileX), "r"(tileY));
        tile = GetBackgroundCollisionAtPosition(tileY, tileX) >> 16;
    }
    if (((u32) (tile - 2)) > 1)
    {
      *(u16 *)&locals.water = 1;
    }
    {
        u32 tableIndex;
        u32 tableValue;
        const u8 *tableData;

        tableData = (const u8 *)sWarioCollisionPointTable;
        tableIndex = index;
        tableValue = tableIndex << 1;
        tableValue += index;
        tableValue <<= 1;
        tableData += 4;
        tableValue += (u32)tableData;
        tableValue = *(const u16 *)tableValue;
        tableValue += wario->yPosition;
        yPosition = (u16)tableValue;
    }
    {
        u32 tileY;
        u32 tileX;

        tileX = wario->xPosition;
        tileY = yPosition;
        ASM_BARRIER(: : "r"(tileX), "r"(tileY));
        tile = GetBackgroundCollisionAtPosition(tileY, tileX) >> 16;
    }
    if (((locals.water != 0) && (tile != 2)) && (tile != 3))
    {
      return 0xFE;
    }
    return 0xFF;
}

u8 ResolveWarioVerticalFenceCollision(void)
{
    struct WarioCollisionData *collisionData;
    u32 index;
    u32 yPosition;
    s32 tile;
    u16 *tableEntry;
    u32 tableValue;
    register struct WarioData *wario PIN("r10");
    struct AGB_ALIGN {
        u16 collision;
        u16 padding;
        volatile u32 secondaryIndex;
    } locals;

    collisionData = &gWarioCollisionData;
    index = collisionData->unk_08;
    locals.secondaryIndex = collisionData->unk_09;
    {
        register struct WarioData *base PIN("r1");
        base = &gWarioData;
        ASM_BARRIER(: "+r"(base));
        wario = base;
    }
    {
        u8 *data;
        u32 offset;

        data = (u8 *)sWarioCollisionPointTable;
        offset = index << 1;
        offset += index;
        offset <<= 1;
        data += 4;
        offset += (u32)data;
        tableEntry = (u16 *)offset;
        tableValue = *tableEntry;
    }
    {
        struct WarioData *base;
        u32 value;
        u32 xPosition;

        base = wario;
        value = base->yPosition;
        value += tableValue;
        yPosition = (u16)value;
        xPosition = base->xPosition;
        value = yPosition;
        tile = ((s32 (*)(u32, u32))GetBackgroundCollisionAtPosition)(value, xPosition) >> 16;
    }

    if (collisionData->flags & 0x40) {
        u32 adjusted;
        u32 offset;

        if (tile != 4) {
            yPosition &= 0xFFC0;
            offset = tableValue;
            adjusted = yPosition - offset;
            goto updateYPosition;
        } else {
            u32 tableOffset;
            u32 value;
            u32 argument;

            tableOffset = *tableEntry;
            value = locals.secondaryIndex;
            argument = tableOffset;
            if (((u32 (*)(u32, u32, u32))CheckWarioVerticalCollision)(value, argument, 0) != 0) {
                yPosition &= 0xFFC0;
                adjusted = yPosition - tableOffset;
updateYPosition:
                adjusted += 0x40;
                {
                    register struct WarioData *base PIN("r1");
                    base = wario;
                    base->yPosition = adjusted;
                }
            }
        }
        return 0xFF;
    }

    {
        register struct WarioData *base PIN("r1");
        u32 value;

        base = wario;
        value = base->yPosition;
        value++;
        yPosition = (u16)value;
    }
    if (((u32 (*)(u32, u32, u16 *))CheckWarioPointCollision)(index, yPosition, &locals.collision) != 0) {
        return 2;
    }
    if (tile == 4) {
        return 0xFF;
    }
    return 0xFE;
}

u8 ResolveWarioHorizontalFenceCollision(void)
{
    u32 index;
    struct WarioData *wario;
    u32 savedIndex;
    u16 newX;
    u16 collision;

    index = gWarioCollisionData.unk_08;
    savedIndex = index;
    if (((u32 (*)(u32, u16 *, u32))CheckWarioHorizontalCollision)(index, &newX, 0) != 0) {
        struct WarioData *dest;
        u16 *source;

        dest = &gWarioData;
        source = &newX;
        dest->xPosition = *source;
        return 0xFF;
    }

    wario = &gWarioData;
    {
        u32 yPosition;
        u16 *out;

        yPosition = wario->yPosition;
        yPosition++;
        yPosition <<= 16;
        yPosition >>= 16;
        out = &collision;
        if (((u32 (*)(u32, u32, u16 *))CheckWarioPointCollision)(index, yPosition, out) != 0) {
            return 2;
        }
    }

    {
        u32 offset;
        u8 *data;
        s32 tile;

        data = (u8 *)sWarioCollisionPointTable;
        offset = savedIndex << 1;
        offset += savedIndex;
        offset <<= 1;
        data += 4;
        offset += (u32)data;
        offset = *(u16 *)offset;
        data = (u8 *)(u32)wario->yPosition;
        offset += (u32)data;
        offset = (u16)offset;
        tile = GetBackgroundCollisionAtPosition(offset, wario->xPosition);
        tile >>= 16;
        if (tile != 4) {
            return 0xFE;
        }
    }
    return 0xFF;
}


void DrawNormalWario(u8 direction)
{
    u32 directionValue;
    struct WarioData *earlyWario;
    struct WarioData *loadedWario;
    struct WarioData *wario;
    const struct WarioAnimationFrame *animation;
    const u8 *graphics;
    register const u16 *palette PIN("r1");
    u16 *historyCounter;
    struct WarioAfterimageData *history;
    u32 counterValue;
    u32 historyIndex;
    u32 historyDestination;
    u32 afterimageOne;
    u32 earlyPose;
    u8 damageTimer;
    u32 tempR0;
    u32 tempR1;
    u32 poseValue;

    directionValue = direction;
    earlyWario = &gWarioData;
    earlyPose = (u8)(earlyWario->pose - 19);
    wario = earlyWario;
    if (earlyPose > 1) {
        damageTimer = wario->damageTimer;
        if (damageTimer != 0) {
            damageTimer--;
            wario->damageTimer = damageTimer;
            gWarioDamagePaletteActive = 1;
            goto damageFlagDone;
        }
    }
    gWarioDamagePaletteActive = 0;
damageFlagDone:

    switch (wario->pose) {
        case 23:
        case 25:
        case 28:
        case 72:
        case 73:
        {
            struct WarioAfterimage *afterimage;

            afterimage = &gWarioDashAfterimage;
            afterimageOne = 1;
            afterimage->unk0 = afterimageOne;
            if (gWarioData.pose == 28)
                afterimage->unk0 = 2;
            afterimage->unk1 = 16;

            historyCounter = (u16 *)&gWarioAfterimageHistoryIndex;
            counterValue = *historyCounter;
            historyIndex = 31;
            history = &gWarioAfterimageHistory;
            historyIndex &= counterValue;
            historyIndex <<= 1;
            historyDestination = historyIndex;
            historyDestination += (u32)history;
            *(u16 *)historyDestination = gPreviousXPosition;
            history = (struct WarioAfterimageData *)((u8 *)history + 0x40);
            historyIndex += (u32)history;
            *(u16 *)historyIndex = gPreviousYPosition;
            counterValue++;
            *historyCounter = counterValue;
            if ((s16)counterValue > 31)
                gWarioAfterimageHistoryWrapped = afterimageOne;
            break;
        }

        default:
            UpdateWarioPositionHistory();
            break;
    }

    loadedWario = &gWarioData;
    poseValue = loadedWario->pose;
    wario = loadedWario;
    switch (poseValue) {
        case 4:
            if (gCurrentCarriedSprite.state == 0) {
                if (wario->yVelocity >= 0) {
                    tempR1 = (u32)sWarioAirborneAnimationTable;
                    goto selectDirectionFromR1;
                }
                tempR0 = (u32)sWarioAirborneAnimationTable;
                tempR1 = directionValue << 2;
                tempR0 += 8;
                goto selectBaseR0;
            }
            if (gCurrentCarriedSprite.unk1 & 0x80) {
                if (wario->yVelocity >= 0) {
                    tempR0 = (u32)sWarioAirborneAnimationTable;
                    tempR1 = directionValue << 2;
                    tempR0 += 32;
                    goto selectBaseR0;
                }
                tempR0 = (u32)sWarioAirborneAnimationTable;
                tempR1 = directionValue << 2;
                tempR0 += 40;
                goto selectBaseR0;
            }
            if (wario->yVelocity >= 0) {
                tempR0 = (u32)sWarioAirborneAnimationTable;
                tempR1 = directionValue << 2;
                tempR0 += 16;
                goto selectBaseR0;
            }
            tempR0 = (u32)sWarioAirborneAnimationTable;
            tempR1 = directionValue << 2;
            tempR0 += 24;
            goto selectBaseR0;

        case 0:
        case 1:
        case 2:
        case 3:
        case 10:
        case 27:
        case 29:
        case 30:
        case 31:
        case 51:
            if (gCurrentCarriedSprite.state != 0) {
                if (poseValue == 0) {
                    tempR1 = gCurrentCarriedSprite.unk1;
                    tempR0 = 15;
                    tempR0 &= tempR1;
                    poseValue = 1;
                    if (tempR0 <= 1)
                        poseValue = 0;
                } else if (poseValue == 1) {
                    poseValue = 2;
                } else if (poseValue == 2) {
                    poseValue = 3;
                } else if (poseValue == 3) {
                    poseValue = 4;
                } else if (poseValue == 10) {
                    poseValue = 3;
                } else if (poseValue == 27) {
                    poseValue = 5;
                } else if (poseValue == 29) {
                    poseValue = 6;
                } else if (poseValue == 28) {
                    poseValue = 5;
                } else if (poseValue == 30) {
                    poseValue = 6;
                } else if (poseValue == 31) {
                    poseValue = 7;
                } else if (poseValue == 51) {
                    poseValue = 8;
                }
                tempR0 = (u32)sWarioCarryingAnimationTable;
                goto selectPoseDirectionFromR0;
            }
            if (gWarioPauseTimer == 0 && poseValue == 0 && (gButtonsHeld & 0x300)) {
                tempR1 = (u32)sWarioIdleInputAnimationTable;
                goto selectDirectionFromR1;
            }
            goto defaultAnimation;

selectDirectionFromR1:
            tempR0 = directionValue << 2;
            tempR0 += tempR1;
            animation = GP(*(GBA_PTR(const struct WarioAnimationFrame *) *)tempR0);
            goto animationSelected;

        case 34:
        case 35:
        case 36:
        case 37:
        case 38:
            if (gCurrentCarriedSprite.unk1 & 0x80) {
                tempR0 = (u8)(poseValue - 34);
                poseValue = (u32)sWarioUpwardThrowAnimationTable;
                tempR1 = directionValue << 2;
                tempR0 <<= 3;
                tempR1 += tempR0;
                tempR1 += poseValue;
                goto loadAnimationR1;
            }
            goto defaultAnimation;

        default:
defaultAnimation:
            tempR0 = (u32)sWarioPoseAnimationTable;
selectPoseDirectionFromR0:
            tempR1 = directionValue << 2;
            poseValue <<= 3;
            tempR1 += poseValue;
selectBaseR0:
            tempR1 += tempR0;
loadAnimationR1:
            animation = GP(*(GBA_PTR(const struct WarioAnimationFrame *) *)tempR1);
            goto animationSelected;

    }

animationSelected:
    animation += wario->unk_1F;
    graphics = (const u8 *)GP(animation->objData);
    wario->objData1Size = *graphics << 5;
    graphics++;
    wario->objData2Size = *graphics << 5;
    graphics++;
    GP_SET(wario->pObjData1, (u8 *)graphics);
    GP_SET(wario->pObjData2, (u8 *)graphics + wario->objData1Size);
    GP_SET(wario->pOamData, (const u16 *)GP(animation->oamData));

    if (wario->damageTimer != 0 && gWarioDamagePaletteActive == 0 && (gMainTimer & 7) <= 3) {
        gWarioPaletteSize = 32;
        palette = sWarioDamageFlashPalette;
        CopyWarioPalette(palette, 0, 16);
        return;
    }

    switch (wario->pose) {
        case 22:
        case 24:
        case 26:
            gWarioPaletteSize = 96;
            palette = sWarioShoulderBashPalette;
            CopyWarioPalette(palette, 0, 48);
            break;

        case 23:
        case 25:
        {
            struct WarioData *paletteWario;
            u32 paletteOffset;

            tempR1 = (u32)&gWarioPaletteSize;
            tempR0 = 96;
            *(u16 *)tempR1 = tempR0;
            palette = (const u16 *)((u32)sWarioDashAttackPaletteFrames - 0x20);
            CopyWarioPalette(palette, 0, 16);
            paletteWario = &gWarioData;
            paletteOffset = paletteWario->unk_0C;
            paletteOffset >>= 1;
            paletteOffset <<= 20;
            paletteOffset >>= 15;
            palette = (const u16 *)((u32)sWarioDashAttackPaletteFrames + paletteOffset);
            CopyWarioPalette(palette, 32, 16);
            paletteOffset = paletteWario->unk_0C;
            if (paletteOffset <= 58) {
                paletteOffset++;
                paletteWario->unk_0C = paletteOffset;
            } else {
                paletteWario->unk_0C = 0;
            }
            break;
        }

        case 27:
        case 28:
        case 29:
        case 30:
            gWarioPaletteSize = 96;
            palette = GP(sWarioGroundPoundPaletteFrames[wario->unk_1F]);
            CopyWarioPalette(palette, 0, 16);
            palette = sWarioGroundPoundSecondaryPalette;
            CopyWarioPalette(palette, 16, 16);
            palette = sWarioGroundPoundPalette;
            CopyWarioPalette(palette, 32, 16);
            break;

        case 35:
        {
            register struct WarioData *paletteWario PIN("r1");

            gWarioPaletteSize = 32;
            paletteWario = wario;
            if (paletteWario->unk_1E > 5)
                paletteWario->unk_1E = 0;
            tempR0 = wario->unk_1E;
            tempR0 >>= 1;
            tempR0 <<= 5;
            tempR1 = (u32)sWarioChargedThrowPaletteFrames;
            tempR1 = tempR0 + tempR1;
            palette = (const u16 *)tempR1;
            CopyWarioPalette(palette, 0, 16);
            break;
        }

        case 54:
        case 55:
        case 59:
            gWarioPaletteSize = 64;
            palette = sWarioDefaultObjPalette;
            CopyWarioPalette(palette, 0, 16);
            palette = sWarioDumbbellSecondaryPalette;
            CopyWarioPalette(palette, 16, 16);
            break;

        case 49:
        case 50:
        case 56:
        case 57:
            gWarioPaletteSize = 64;
            palette = sWarioDefaultObjPalette;
            CopyWarioPalette(palette, 0, 16);
            palette = sWarioDoorAndBarbellSecondaryPalette;
            CopyWarioPalette(palette, 16, 16);
            break;

        case 65:
        case 66:
        case 67:
        case 68:
            gWarioPaletteSize = 64;
            palette = sWarioFlexingPalette;
            CopyWarioPalette(palette, 0, 32);
            break;

        case 76:
            gWarioPaletteSize = 64;
            palette = sWarioDefaultObjPalette;
            CopyWarioPalette(palette, 0, 16);
            tempR0 = (u32)&gMainTimer;
            tempR1 = *(u8 *)tempR0;
            tempR0 = 4;
            tempR0 &= tempR1;
            palette = sWarioUnusedPose76Palette;
            if (tempR0 != 0)
                palette = sWarioUnusedPose76AlternatePalette;
            CopyWarioPalette(palette, 16, 16);
            break;

        default:
            gWarioPaletteSize = 32;
            palette = sWarioDefaultObjPalette;
            CopyWarioPalette(palette, 0, 16);
            break;
    }
}

void UpdateWarioMusicEffects(void)
{
    s8 *statePtr;
    register struct MusicPlayerInfo *player0 PIN("r8");
    s32 state;

    if (gSwitchPressed != 0)
        return;

    statePtr = &gWarioMusicState;
    state = *statePtr;
    if (state > 4) {
        u32 value;
        register u32 tempo PIN("r5");
        register struct MusicPlayerInfo *resetPlayer1 PIN("r6");
        struct MusicPlayerInfo *loadedPlayer;

        value = (u32)gMPlayTable;
        loadedPlayer = GP(((const struct MusicPlayer *)value)[0].info);
        player0 = loadedPlayer;
        tempo = 0x100;
        {
            struct MusicPlayerInfo *arg0;
            u32 arg1;
            arg0 = player0;
            arg1 = tempo;
            ((void (*)(struct MusicPlayerInfo *, u32))m4aMPlayTempoControl)(arg0, arg1);
        }
        resetPlayer1 = GP(((const struct MusicPlayer *)value)[1].info);
        {
            struct MusicPlayerInfo *arg0;
            u32 arg1;
            arg0 = resetPlayer1;
            arg1 = tempo;
            ((void (*)(struct MusicPlayerInfo *, u32))m4aMPlayTempoControl)(arg0, arg1);
        }
        value = 0xFFFF;
        {
            struct MusicPlayerInfo *arg0;
            u32 arg1;
            u32 arg2;
            arg0 = player0;
            arg1 = value;
            arg2 = 0;
            ((void (*)(struct MusicPlayerInfo *, u32, u32))m4aMPlayPitchControl)(arg0, arg1, arg2);
        }
        {
            struct MusicPlayerInfo *arg0;
            u32 arg1;
            u32 arg2;
            arg0 = resetPlayer1;
            arg1 = value;
            arg2 = 0;
            ((void (*)(struct MusicPlayerInfo *, u32, u32))m4aMPlayPitchControl)(arg0, arg1, arg2);
        }
        {
            struct MusicPlayerInfo *arg0;
            u32 arg1;
            u32 arg2;
            arg0 = player0;
            arg1 = value;
            arg2 = 0;
            ((void (*)(struct MusicPlayerInfo *, u32, u32))m4aMPlayModDepthSet)(arg0, arg1, arg2);
        }
        {
            struct MusicPlayerInfo *arg0;
            u32 arg1;
            u32 arg2;
            arg0 = resetPlayer1;
            arg1 = value;
            arg2 = 0;
            ((void (*)(struct MusicPlayerInfo *, u32, u32))m4aMPlayModDepthSet)(arg0, arg1, arg2);
        }
        {
            struct MusicPlayerInfo *arg0;
            u32 arg1;
            u32 arg2;
            arg0 = player0;
            arg1 = value;
            arg2 = 20;
            ((void (*)(struct MusicPlayerInfo *, u32, u32))m4aMPlayLFOSpeedSet)(arg0, arg1, arg2);
        }
        {
            struct MusicPlayerInfo *arg0;
            u32 arg1;
            u32 arg2;
            arg0 = resetPlayer1;
            arg1 = value;
            arg2 = 20;
            ((void (*)(struct MusicPlayerInfo *, u32, u32))m4aMPlayLFOSpeedSet)(arg0, arg1, arg2);
        }
        *statePtr = 0;
    }

    switch (gWarioData.pose) {
        case WPOSE_NORMAL_CROUCHING:
        case WPOSE_NORMAL_CRAWLING:
        case WPOSE_NORMAL_TURNING_CROUCHED:
        case WPOSE_NORMAL_CROUCH_JUMP:
        case WPOSE_NORMAL_UNKNOWN_52:
            if (gWarioMusicState == 1)
                return;
            m4aMPlayTempoControl(GP(gMPlayTable[0].info), 230);
            m4aMPlayTempoControl(GP(gMPlayTable[1].info), 230);
            gWarioMusicState = 1;
            return;

        case WPOSE_NORMAL_ROLLING:
        case WPOSE_NORMAL_ROLLING_JUMP:
            if (gWarioMusicState == 2)
                return;
            m4aMPlayTempoControl(GP(gMPlayTable[0].info), 400);
            m4aMPlayTempoControl(GP(gMPlayTable[1].info), 400);
            gWarioMusicState = 2;
            return;

        case WPOSE_NORMAL_GROUND_POUND_LANDING:
        {
            struct MusicPlayerInfo *modePlayer0;
            struct MusicPlayerInfo *modePlayer1;

            if (gWarioMusicState == 3)
                return;
            modePlayer0 = GP(gMPlayTable[0].info);
            m4aMPlayModDepthSet(modePlayer0, 0xFFFF, 50);
            modePlayer1 = GP(gMPlayTable[1].info);
            m4aMPlayModDepthSet(modePlayer1, 0xFFFF, 50);
            m4aMPlayLFOSpeedSet(modePlayer0, 50, 100);
            m4aMPlayLFOSpeedSet(modePlayer1, 50, 100);
            gWarioMusicState = 3;
            return;
        }

        default:
        {
            const struct MusicPlayer *players;
            register struct MusicPlayerInfo *loadedPlayer PIN("r1");
            register struct MusicPlayerInfo *defaultPlayer1 PIN("r6");
            u32 defaultTempo;

            if (gWarioMusicState == 0)
                return;
            players = gMPlayTable;
#ifdef PORTABLE
            loadedPlayer = GP(players[0].info);
#else
            loadedPlayer = *(struct MusicPlayerInfo * volatile *)&players[0].info;
#endif
            player0 = loadedPlayer;
            defaultTempo = 0x100;
            {
                struct MusicPlayerInfo *arg0;
                u32 arg1;
                arg0 = player0;
                arg1 = defaultTempo;
                m4aMPlayTempoControl(arg0, arg1);
            }
            defaultPlayer1 = GP(players[1].info);
            {
                struct MusicPlayerInfo *arg0;
                u32 arg1;
                arg0 = defaultPlayer1;
                arg1 = defaultTempo;
                m4aMPlayTempoControl(arg0, arg1);
            }
            m4aMPlayModDepthSet(player0, 0xFFFF, 0);
            m4aMPlayModDepthSet(defaultPlayer1, 0xFFFF, 0);
            m4aMPlayLFOSpeedSet(player0, 0xFFFF, 20);
            m4aMPlayLFOSpeedSet(defaultPlayer1, 0xFFFF, 20);
            gWarioMusicState = 0;
            return;
        }
    }
}

void UpdateWarioHitbox(void)
{
    u8 *poseData;
    struct WarioData *wario;
    u8 *hitboxData;
    register u32 offset PIN("r1");
    u32 value;

    poseData = (u8 *)sWarioNormalPoseProperties;
    wario = &gWarioData;
    value = wario->pose;
    value <<= 3;
    value += (u32)poseData;
    offset = *(u8 *)value;
    hitboxData = (u8 *)sWarioHitboxes;
    offset <<= 3;
    value = offset + (u32)hitboxData;
    wario->hitboxOffsetLeft = *(u16 *)value;
    value = (u32)(hitboxData + 2);
    value = offset + value;
    wario->hitboxOffsetTop = *(u16 *)value;
    value = (u32)(hitboxData + 4);
    value = offset + value;
    wario->hitboxOffsetRight = *(u16 *)value;
    hitboxData += 6;
    offset += (u32)hitboxData;
    wario->hitboxOffsetBottom = *(u16 *)offset;

    value = wario->pose;
    value <<= 3;
    poseData += 7;
    value += (u32)poseData;
    value = *(u8 *)value;
    offset = value;
    if (value == 2) {
        wario->unk_1A = value;
    } else if (wario->unk_1A != 1) {
        wario->unk_1A = offset;
    }
}

u32 CheckWarioReactionCollision(u8 reaction)
{
    u16 value;
    u32 result;

    switch (reaction) {
        case 8:
            value = sWarioCollisionPointTable[26];
            goto check;
        case 9:
        case 11:
            value = sWarioCollisionPointTable[8];
            goto check;
        case 10:
            value = sWarioCollisionPointTable[29];
            goto check;
        default:
            goto defaultCheck;
    }
check:
    result = CheckWarioVerticalCollision(0, value, 1);
    goto done;
defaultCheck:
    result = CheckWarioVerticalCollision(0, sWarioCollisionPointTable[5], 1);
done:
    return result;
}

void UpdateWarioTileMovement(void)
{
    s32 tile;
    s32 index;

    tile = GetBackgroundCollisionAtPosition(gWarioData.yPosition - 16, gWarioData.xPosition) >> 16;
    tile &= 0xFF;
    index = tile;
    index -= 12;
    if ((u32)index <= 7) {
        gWarioTileHorizontalSpeed = sWarioTileHorizontalSpeedTable[index];
        gWarioTileVerticalSpeed = sWarioTileVerticalSpeedTable[index];
    } else {
        gWarioTileHorizontalSpeed = 0;
        gWarioTileVerticalSpeed = 0;
    }
}

u8 CheckWarioTileMovement(void)
{
    UpdateWarioTileMovement();
    if (gWarioData.verticalDirection & 0x40) {
        if (gWarioTileVerticalSpeed == -96) {
            return 10;
        }
    } else if (gWarioData.verticalDirection & 0x80) {
        if (gWarioTileVerticalSpeed == 96) {
            return 10;
        }
    }

    if (gWarioData.horizontalDirection & 0x10) {
        if (gWarioTileHorizontalSpeed == -96) {
            return 9;
        }
    } else if (gWarioTileHorizontalSpeed == 96) {
        return 9;
    }
    return 0xFF;
}
