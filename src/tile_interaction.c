#include "tile_interaction.h"
#include "background_registers.h"
#include "bg_clip.h"
#include "compiler.h"
#include "global_data.h"
#include "main.h"
#include "score.h"
#include "screen_shake.h"
#include "sound.h"
#include "sprite.h"
#include "wario.h"
#ifdef PORTABLE
#include "port/rumble_snapshot.h"
#endif

void SpawnTileDebris(struct TileInteractionContext* ctx);
void SpawnTileDebrisSprite(struct TileInteractionContext* ctx);
s32 FindTwoStepTileReplacement(struct TileInteractionContext* ctx);
s32 FindTileReplacement(struct TileInteractionContext* ctx);
s32 ActivateSwitchTile(struct TileInteractionContext* ctx, u8 mode);
s32 ProcessSpecialBossTile(struct TileInteractionContext* ctx);
void ReplaceTileEffectPair(u8 index);
void GetTileEffectPosition(u8 index, u16* y, u16* x);
void UpdateWarioEnvironmentalTiles(void);
s32 ProcessWarioReactionTile(s32 attribute, s32 pose);
void ClearTileAndSpawnPrimaryEffect(u32 y, u32 x);
void ClearTileAndSpawnAlternateEffect(u32 y, u32 x);
s32 RequestWarioPoseForTileAttributes(const u32* attributes, u32 pose);
s32 SetBg1Tile(u32 tile, u32 y, u32 x);
void SetBg1TileAndGraphics(u32 tile, u32 y, u32 x);
void SetBg2TileAndGraphics(u16 tile, u16 y, u16 x);

#ifndef PORTABLE
const u16 sSingleTileBreakSoundIds[] = {
    0xFC,  0x104, 0xFE,  0x106, 0x100, 0x108, 0x102, 0x10A, 0x10C, 0x10D, 0x10C,
    0x10D, 0x10C, 0x10D, 0x10C, 0x10D, 0xFC,  0x104, 0xFC,  0x104, 0xFC,  0x104,
};

const u16 sAdjacentTileBreakSoundIds[] = {
    0xFD,  0x105, 0xFF,  0x107, 0x101, 0x109, 0x103, 0x10B, 0x10C, 0x10D, 0x10C,
    0x10D, 0x10C, 0x10D, 0x10C, 0x10D, 0xFD,  0x105, 0xFD,  0x105, 0xFD,  0x105,
};

const u8 sTileEffectPositions[] = {
    0x11, 0x16, 0x13, 0x16, 0x15, 0x16, 0x11, 0x16, 0x13, 0x16, 0x15, 0x16, 0x11, 0x16, 0x13, 0x16,
    0x15, 0x16, 0x11, 0x16, 0x13, 0x16, 0x15, 0x16, 0x10, 0x14, 0x10, 0x16, 0x10, 0x18, 0x12, 0x14,
    0x12, 0x16, 0x12, 0x18, 0x14, 0x14, 0x14, 0x16, 0x14, 0x18, 0x16, 0x14, 0x16, 0x16, 0x16, 0x18,
};

const u16 sTileInteractionSoundIds[] = {0x140, 0x152, 0x141, 0x153, 0x142, 0x154};
#else
extern const u16 sSingleTileBreakSoundIds[];
extern const u16 sAdjacentTileBreakSoundIds[];
extern const u8 sTileEffectPositions[];
extern const u16 sTileInteractionSoundIds[];
#endif

s32 ProcessTileInteractionAttribute(struct TileInteractionContext* ctx);
s32 ProcessFragileTileAttribute(struct TileInteractionContext* ctx);
s32 ProcessDirectionalTileAttribute(struct TileInteractionContext* ctx);
s32 DispatchTileInteractionHandler(struct TileInteractionContext* ctx, u8 type);

u32 ProcessHorizontalTileInteraction(u32 y, u32 x)
{
    /* Required for byte-exact agbcc allocation or scheduling; direct C alternatives were tested. */
    struct TileInteractionContext ctx;
    u16 originalX;
    s32 result;

    y = (u16)y;
    x = (u16)x;
    originalX = x;
    result = 0;
    ctx.yTile = y >> 6;
    ctx.xTile = x >> 6;
    {
        struct BackgroundInfo* background;
        u32 width;
        u32 address;
        u32 xTile;
        const u16* attributes;
        struct TileInteractionContext* state;

        background = &gBackgroundInfo;
        width = background->bg1Width;
        address = ctx.yTile;
        {
            volatile u16* xPtr;
            xPtr = &ctx.xTile;
            address *= width;
            xTile = *xPtr;
        }
        COMPILER_BARRIER(xTile);
        address += xTile;
        xTile = (u32)GP(background->pBg1Data);
        address <<= 1;
        address += xTile;
        address = *(u16*)address;
        state = &ctx;
        attributes = GP(gBackgroundTileTables.attributes);
        address <<= 1;
        address += (u32)attributes;
        state->attribute = *(u16*)address;
    }
    ctx.debrisType = 0;
    ctx.debrisOffset = 0;
    ctx.soundIndex = 0;
    ctx.collisionMode = gWarioCollisionData.unk_0B;
    ctx.direction = 1;

    if (gWarioData.reaction != 0) {
        if (gWarioData.reaction != 6) {
            goto done;
        }
        ctx.direction = 5;
        if (gWarioData.xPosition > originalX) {
            ctx.direction = 6;
        }
        result = ProcessDirectionalTileAttribute(&ctx);
        goto done;
    }

    if (gWarioData.xPosition > originalX) {
        ctx.direction = 2;
    }

    switch (gWarioData.pose) {
        case 14:
        case 15:
            goto process;
        case 22:
        case 24:
            goto reverseDirection;
        case 23:
        case 25:
            goto addDirection;
        default:
            goto checkResult;
    }
addDirection:
    ctx.direction += 4;
    goto process;
reverseDirection:
    ctx.direction ^= 3;
    result = 1;
checkResult:
    if (result == 0) {
        goto done;
    }
process:
    result = ProcessTileInteractionAttribute(&ctx);
    if (result == 1) {
        goto done;
    }
    if (result == 2) {
        result = 0;
        goto done;
    }
    result = ActivateSwitchTile(&ctx, 0);
done:
    return result;
}

u32 ProcessPointTileInteraction(u32 y, u32 x)
{
    /* Required for byte-exact agbcc allocation or scheduling; direct C alternatives were tested. */
    struct TileInteractionContext ctx;
    u32 zero;
    register s32 result PIN("r3");

    y = (u16)y;
    x = (u16)x;
    {
        struct TileInteractionContext* state;
        u32 yTile;
        state = &ctx;
        yTile = y >> 6;
        zero = 0;
        state->yTile = yTile;
    }
    ctx.xTile = x >> 6;
    {
        struct BackgroundInfo* background;
        u32 width;
        register u32 address PIN("r0");
        u32 xTile;

        background = &gBackgroundInfo;
        width = background->bg1Width;
        address = ctx.yTile;
        {
            struct TileInteractionContext* state;
            state = &ctx;
            address *= width;
            xTile = state->xTile;
        }
        address += xTile;
        xTile = (u32)GP(background->pBg1Data);
        address <<= 1;
        address += xTile;
        {
            register u32 tile PIN("r3");
            struct TileInteractionContext* state;
            struct BackgroundTileTables* tables;
            const u16* attributes;

            tile = *(u16*)address;
            state = &ctx;
            tables = &gBackgroundTileTables;
            attributes = GP(tables->attributes);
            address = tile << 1;
            address += (u32)attributes;
            state->attribute = *(u16*)address;
        }
    }
    ctx.debrisType = zero;
    ctx.debrisOffset = zero;
    ctx.soundIndex = zero;
    ctx.collisionMode = gWarioCollisionData.unk_0D;
    ctx.direction = 4;
#ifdef PORTABLE
    if (gWarioData.reaction == REACTION_NORMAL && gWarioData.yVelocity > 0 &&
        ((ctx.attribute >= 11 && ctx.attribute <= 42) || ctx.attribute == 52 || ctx.attribute == 53 ||
         (ctx.attribute >= 89 && ctx.attribute <= 93)))
        PortRumbleImpact(PORT_RUMBLE_HEAD);
#endif
    result = ProcessTileInteractionAttribute(&ctx);
    if (result == 1) {
        return 1;
    }
    if (result == 2) {
        return 0;
    }
    if (ActivateSwitchTile(&ctx, 2)) {
        return 1;
    }
    return 0;
}

u32 ProcessSideTileInteraction(u32 y, u32 x)
{
    /* Required for byte-exact agbcc allocation or scheduling; direct C alternatives were tested. */
    struct TileInteractionContext ctx;
    u32 zero;
    register s32 result PIN("r3");

    y = (u16)y;
    x = (u16)x;
    {
        struct TileInteractionContext* state;
        u32 yTile;
        state = &ctx;
        yTile = y >> 6;
        zero = 0;
        state->yTile = yTile;
    }
    ctx.xTile = x >> 6;
    {
        struct BackgroundInfo* background;
        u32 width;
        register u32 address PIN("r0");
        u32 xTile;

        background = &gBackgroundInfo;
        width = background->bg1Width;
        address = ctx.yTile;
        {
            struct TileInteractionContext* state;
            state = &ctx;
            address *= width;
            xTile = state->xTile;
        }
        address += xTile;
        xTile = (u32)GP(background->pBg1Data);
        address <<= 1;
        address += xTile;
        {
            register u32 tile PIN("r3");
            struct TileInteractionContext* state;
            struct BackgroundTileTables* tables;
            const u16* attributes;

            tile = *(u16*)address;
            state = &ctx;
            tables = &gBackgroundTileTables;
            attributes = GP(tables->attributes);
            address = tile << 1;
            address += (u32)attributes;
            state->attribute = *(u16*)address;
        }
    }
    ctx.debrisType = zero;
    ctx.debrisOffset = zero;
    ctx.soundIndex = zero;
    ctx.collisionMode = gWarioCollisionData.unk_0C;
    ctx.direction = 3;
    if (gWarioData.reaction == 7) {
        ctx.direction = 4;
    }
    result = ProcessTileInteractionAttribute(&ctx);
    if (result == 1) {
        return 1;
    }
    if (result == 2) {
        return 0x80;
    }
    if (ActivateSwitchTile(&ctx, 1)) {
        return 1;
    }
    return 0;
}

s32 ProcessTileInteractionAttribute(struct TileInteractionContext* ctx)
{
    /* Required for byte-exact agbcc allocation or scheduling; direct C alternatives were tested. */
    struct TileInteractionContext* state;
    u8 result;
    register u32 value PIN("r0");
    u32 argument;
    u32 collisionMode;

    state = ctx;
    result = 0;
    argument = state->attribute;
    value = argument;
    value -= 11;
    value = (u16)value;
    if (value <= 19) {
        value = argument;
        if (value <= 15) {
            state->debrisOffset = result;
            state->soundIndex = result;
            argument = 11;
        } else if (value <= 20) {
            value = 3;
            state->debrisOffset = value;
            value = 1;
            state->soundIndex = value;
            argument = 16;
        } else if (value <= 25) {
            value = 1;
            state->debrisOffset = value;
            value = 2;
            state->soundIndex = value;
            argument = 21;
        } else if (value <= 30) {
            value = 2;
            state->debrisOffset = value;
            value = 3;
            state->soundIndex = value;
            argument = 26;
        } else {
            goto returnZero;
        }
        value = *(volatile u16*)&state->attribute;
        argument = value - argument;
        goto callReturnOne;
    }

    value = argument;
    value -= 31;
    value = (u16)value;
    if (value <= 11) {
        collisionMode = state->collisionMode;
        if (collisionMode == 2) {
            argument = state->direction;
            if (argument > 4) {
                value = 3;
                value ^= argument;
                state->direction = value;
            }
            value = state->attribute;
            if (value <= 33) {
                state->debrisOffset = result;
                value = 4;
                state->soundIndex = value;
                argument = 31;
            } else if (value <= 36) {
                value = 3;
                state->debrisOffset = value;
                value = 5;
                state->soundIndex = value;
                argument = 34;
            } else if (value <= 39) {
                value = 1;
                state->debrisOffset = value;
                value = 6;
                state->soundIndex = value;
                argument = 37;
            } else if (value <= 42) {
                state->debrisOffset = collisionMode;
                value = 7;
                state->soundIndex = value;
                argument = 40;
            } else {
                goto returnZero;
            }
            value = *(volatile u16*)&state->attribute;
            value -= argument;
            argument = value + 2;
            argument <<= 24;
            argument >>= 24;
            value = DispatchTileInteractionHandler(state, argument);
            if (value != 0) {
                result = 2;
            }
            goto done;
        }
    }

    argument = state->attribute;
    value = argument;
    value -= 52;
    value = (u16)value;
    if (value <= 1) {
        value = 0;
        state->debrisOffset = value;
        value = 3;
        state->soundIndex = value;
        value = *(volatile u16*)&state->attribute;
        argument = value;
        argument -= 46;
        goto callReturnOne;
    }
    value = argument;
    value -= 89;
    value = (u16)value;
    if (value <= 4) {
        value = 0;
        state->debrisOffset = value;
        state->soundIndex = value;
        value = *(volatile u16*)&state->attribute;
        argument = value;
        argument -= 89;
    callReturnOne:
        argument = (u8)argument;
        value = DispatchTileInteractionHandler(state, argument);
        if (value != 0) {
            result = 1;
        }
        goto done;
    }
    goto returnZero;
done: {
    u32 returnValue;
    returnValue = result;
    return returnValue;
}
returnZero:
    return 0;
}

s32 ProcessFragileTileAttribute(struct TileInteractionContext* ctx)
{
    /* Required for byte-exact agbcc allocation or scheduling; direct C alternatives were tested. */
    struct TileInteractionContext* state;
    u32 value;
    register u32 zero PIN("r1");

    state = ctx;
    zero = 0;
    value = state->attribute;
    value -= 43;
    value = (u16)value;
    if (value <= 4) {
        goto valid;
    }
    value = 0;
    goto done;
valid:
    state->debrisOffset = zero;
    value = 8;
    state->soundIndex = value;
    zero = state->attribute;
    zero -= 43;
    zero = (u8)zero;
    value = DispatchTileInteractionHandler(state, zero);
    zero = -value;
    zero |= value;
    zero >>= 31;
    COMPILER_BARRIER(zero);
    value = zero;
done:
    return value;
}

s32 ProcessDirectionalTileAttribute(struct TileInteractionContext* ctx)
{
    /* Required for byte-exact agbcc allocation or scheduling; direct C alternatives were tested. */
    struct TileInteractionContext* state;
    s32 result;
    u32 collisionMode;
    u32 attribute;
    register u32 value PIN("r0");
    u32 type;

    state = ctx;
    result = 0;
    collisionMode = state->collisionMode;
    if (collisionMode > 1) {
        goto main;
    }
returnZero:
    value = 0;
    goto directReturn;
main:
    attribute = state->attribute;
    value = attribute;
    value -= 48;
    value = (u16)value;
    if (value > 3) {
        goto done;
    }
    state->debrisOffset = result;
    value = attribute << 16;
    COMPILER_BARRIER(value);
    value >>= 16;
    COMPILER_BARRIER(attribute);
    if (value == 48) {
        value = 9;
        goto type2Store;
    }
    if (value == 49) {
        value = 9;
        goto type5Store;
    }
    if (value != 50) {
        goto checkType5;
    }
    if (collisionMode != 3) {
        goto checkType5;
    }
    value = 10;
type2Store:
    state->soundIndex = value;
    type = 2;
    goto call;
checkType5:
    value = attribute << 16;
    value >>= 16;
    if (value != 51) {
        goto returnZero;
    }
    value = state->collisionMode;
    if (value != 3) {
        goto returnZero;
    }
    value = 10;
type5Store:
    state->soundIndex = value;
    type = 5;
call:
    value = DispatchTileInteractionHandler(state, type);
    if (value != 0) {
        result = 1;
    }
done:
    value = result;
directReturn:
    return value;
}

s32 DispatchTileInteractionHandler(struct TileInteractionContext* ctx, u8 type)
{
    switch (type) {
        case 0:
        case 2:
        case 6:
            break;
        case 1:
            ctx->debrisType = 1;
            break;
        case 3:
            ctx->debrisType = 2;
            break;
        case 4:
            FindTwoStepTileReplacement(ctx);
            break;
        case 5:
        case 7:
            FindTileReplacement(ctx);
            break;
        case 8:
        default:
            return 0;
    }
    gTileInteractionHandlers[type](ctx);
    return 1;
}

void BreakSingleTile(struct TileInteractionContext* ctx)
{
    /* Required for byte-exact agbcc allocation or scheduling; direct C alternatives were tested. */
    if (SetBg1Tile(0, ctx->yTile, ctx->xTile)) {
        SpawnTileDebris(ctx);
    }
    if (ctx->debrisType) {
        SpawnTileDebrisSprite(ctx);
    }
    {
        const u16* sounds;
        register u32 soundOffset PIN("r0");
        u32 reaction;

        sounds = sSingleTileBreakSoundIds;
        soundOffset = ctx->soundIndex;
        reaction = gWarioData.reaction;
        soundOffset <<= 2;
        if (reaction == 1) {
            soundOffset += 2;
        }
        soundOffset += (u32)sounds;
        m4aSongNumStart(*(const u16*)soundOffset);
    }
}

void BreakAdjacentTiles(struct TileInteractionContext* ctx)
{
    /* Required for byte-exact agbcc allocation or scheduling; direct C alternatives were tested. */
    if (SetBg1Tile(0, ctx->yTile, ctx->xTile)) {
        SpawnTileDebris(ctx);
    }
    if (ctx->debrisType) {
        SpawnTileDebrisSprite(ctx);
    }
    ctx->xTile++;
    ctx->yTile++;
    if (SetBg1Tile(0, ctx->yTile, ctx->xTile)) {
        SpawnTileDebris(ctx);
    }
    ctx->yTile--;
    if (SetBg1Tile(0, ctx->yTile, ctx->xTile)) {
        SpawnTileDebris(ctx);
    }
    ctx->xTile--;
    ctx->yTile++;
    if (SetBg1Tile(0, ctx->yTile, ctx->xTile)) {
        SpawnTileDebris(ctx);
    }
    {
        const u16* sounds;
        register u32 soundOffset PIN("r0");
        u32 reaction;

        sounds = sAdjacentTileBreakSoundIds;
        soundOffset = ctx->soundIndex;
        reaction = gWarioData.reaction;
        soundOffset <<= 2;
        if (reaction == 1) {
            soundOffset += 2;
        }
        soundOffset += (u32)sounds;
        m4aSongNumStart(*(const u16*)soundOffset);
    }
}

void BreakTrackedAdjacentTiles(struct TileInteractionContext* ctx)
{
    /* Required for byte-exact agbcc allocation or scheduling; direct C alternatives were tested. */
    register struct TileInteractionContext* state PIN("r5");
    u32 replacement;

    state = ctx;
    {
        struct BackgroundInfo* background;
        u32 width;
        register u32 value PIN("r0");
        register u32 product PIN("r2");
        u32 xTile;
        u8* count;
        u32 base;

        background = &gBackgroundInfo;
        width = background->bg1Width;
        value = state->yTile;
        COMPILER_BARRIER(value);
        product = value;
        product *= width;
        COMPILER_BARRIER(product);
        value = product;
        COMPILER_BARRIER(value);
        xTile = state->xTile;
        value += xTile;
        replacement = (u16)value;
        count = &gRoomTransitionTileDataCount;
        value = *count;
        value <<= 1;
        base = 0x02039F80;
        value += base;
        *(u16*)value = replacement;
        base = (u32)GP(background->pBg1Data);
        value = replacement << 1;
        value += base;
        replacement = *(u16*)value;
        value = *count;
        value <<= 1;
        base = 0x02039F82;
        value += base;
        *(u16*)value = replacement;
        value = *count;
        value += 2;
        *count = value;
    }

    {
        u32 value;
        value = replacement + 1;
        replacement = (u16)value;
    }
    {
        u16 callY;
        u16 callX;

        callY = state->yTile;
        callX = state->xTile;
        if (SetBg1Tile(replacement, callY, callX)) {
            SpawnTileDebris(state);
        }
    }
    state->xTile++;
    {
        u32 value;
        value = replacement + 2;
        replacement = (u16)value;
    }
    {
        u16 callY;
        u16 callX;

        callY = state->yTile;
        callX = state->xTile;
        if (SetBg1Tile(replacement, callY, callX)) {
            SpawnTileDebris(state);
        }
    }
    state->yTile++;
    {
        u32 value;
        value = replacement + 4;
        replacement = (u16)value;
    }
    {
        u16 callY;
        u16 callX;

        callY = state->yTile;
        callX = state->xTile;
        if (SetBg1Tile(replacement, callY, callX)) {
            SpawnTileDebris(state);
        }
    }
    state->xTile--;
    {
        u32 value;
        value = replacement - 2;
        replacement = (u16)value;
    }
    {
        u16 callY;
        u16 callX;

        callY = state->yTile;
        callX = state->xTile;
        if (SetBg1Tile(replacement, callY, callX)) {
            SpawnTileDebris(state);
        }
    }
    {
        const u16* sounds;
        register u32 soundOffset PIN("r0");
        u32 reaction;

        sounds = sAdjacentTileBreakSoundIds;
        soundOffset = state->soundIndex;
        reaction = gWarioData.reaction;
        soundOffset <<= 2;
        if (reaction == 1) {
            soundOffset += 2;
        }
        soundOffset += (u32)sounds;
        m4aSongNumStart(*(const u16*)soundOffset);
    }
}

void SpawnTileDebris(struct TileInteractionContext* ctx)
{
#ifdef PORTABLE
    PortRumbleTileBroken();
#endif
    u16 yTile;
    u16 y;
    u16 xTile;
    u16 x;
    u16 parity;
    u8 one;

    yTile = ctx->yTile;
    y = yTile << 6;
    xTile = ctx->xTile;
    x = xTile << 6;
    one = 1;
    parity = xTile ^ yTile;
    parity &= one;
    SpawnTileDebrisParticle(y, x, ctx->debrisOffset, ctx->direction, parity);
}

void SpawnTileDebrisSprite(struct TileInteractionContext* ctx)
{
    u32 shifted;
    u32 y;
    u32 x;
    u32 spawnY;
    u32 spawnX;

    shifted = ctx->yTile;
    shifted <<= 22;
    y = shifted >> 16;
    shifted = ctx->xTile;
    shifted <<= 22;
    x = shifted >> 16;
    if (ctx->debrisType == 2) {
        spawnY = y;
        spawnY += 64;
        spawnY <<= 16;
        spawnY >>= 16;
        spawnX = x;
        spawnX += 64;
        spawnX = (u16)spawnX;
        SpriteSpawnAsChild(0x82, 0, 0, spawnY, spawnX);
    } else {
        spawnY = 32;
        spawnY |= y;
        spawnX = 32;
        spawnX |= x;
        SpriteSpawnAsChild(0x81, 0, 0, spawnY, spawnX);
    }
}

s32 FindTwoStepTileReplacement(struct TileInteractionContext* ctx)
{
    /* Required for byte-exact agbcc allocation or scheduling; direct C alternatives were tested. */
    struct TileInteractionContext* state;
    u32 targetA;
    u32 targetB;
    u32 temp;
    register u32 second PIN("r1");
    s32 i;
    struct BackgroundTileTables* tables;
    register struct BackgroundInfo* background PIN("r4");

    state = ctx;
    second = state->attribute;
    temp = second - 2;
    targetA = (u16)temp;
    second -= 1;
    targetB = (u16)second;
    i = 0;
    tables = &gBackgroundTileTables;
    background = &gBackgroundInfo;
loop:
    if (i == 0) {
        goto decrementX;
    }
    if (i == 1) {
        state->xTile++;
        state->yTile--;
        goto sample;
    }
    if (i != 2) {
        goto sample;
    }
decrementX:
    state->xTile--;
sample: {
    register u32 width PIN("r1");
    u32 address;
    width = background->bg1Width;
    address = state->yTile;
    address *= width;
    width = state->xTile;
    address += width;
    width = (u32)GP(background->pBg1Data);
    address <<= 1;
    address += width;
    address = *(u16*)address;
    width = (u32)GP(tables->attributes);
    address <<= 1;
    address += width;
    address = *(u16*)address;
    width = 0;
    state->attribute = address;
    address <<= 16;
    address >>= 16;
    if (address != targetA) {
        goto checkB;
    }
    state->debrisType = width;
    temp = 1;
    goto done;
checkB:
    if (address != targetB) {
        goto next;
    }
    temp = 2;
    state->debrisType = temp;
    temp = 1;
    goto done;
}
next:
    i++;
    if (i <= 2) {
        goto loop;
    }
    temp = 0;
done:
    return temp;
}

s32 FindTileReplacement(struct TileInteractionContext* ctx)
{
    /* Required for byte-exact agbcc allocation or scheduling; direct C alternatives were tested. */
    struct TileInteractionContext* state;
    u32 target;
    u32 temp;
    s32 i;
    struct BackgroundTileTables* tables;
    register struct BackgroundInfo* background PIN("r4");

    state = ctx;
    temp = state->attribute;
    temp -= 1;
    target = (u16)temp;
    i = 0;
    tables = &gBackgroundTileTables;
    background = &gBackgroundInfo;
loop:
    if (i == 0) {
        goto decrementX;
    }
    if (i == 1) {
        state->xTile++;
        state->yTile--;
        goto sample;
    }
    if (i != 2) {
        goto sample;
    }
decrementX:
    state->xTile--;
sample: {
    register u32 width PIN("r1");
    u32 address;
    width = background->bg1Width;
    address = state->yTile;
    address *= width;
    width = state->xTile;
    address += width;
    width = (u32)GP(background->pBg1Data);
    address <<= 1;
    address += width;
    address = *(u16*)address;
    width = (u32)GP(tables->attributes);
    address <<= 1;
    address += width;
    address = *(u16*)address;
    width = 0;
    state->attribute = address;
    address <<= 16;
    address >>= 16;
    if (address != target) {
        goto next;
    }
    state->debrisType = width;
    temp = 1;
    goto done;
}
next:
    i++;
    if (i <= 2) {
        goto loop;
    }
    temp = 0;
done:
    return temp;
}

s32 ActivateSwitchTile(struct TileInteractionContext* ctx, u8 mode)
{
    /* Required for byte-exact agbcc allocation or scheduling; direct C alternatives were tested. */
    u32 normalizedMode;
    register s32 result PIN("r4");
    u32 switchId;
    u32 attribute;
    register u32 value PIN("r0");

    normalizedMode = mode;
    result = 0;
    switchId = 0;
    attribute = ctx->attribute;
    value = attribute;
    value -= 64;
    value = (u16)value;
    if (value > 3) {
        value = 0;
        goto directReturn;
    }
    value = attribute;
    if (value == 64) {
        switchId = 1;
        goto processSwitch;
    }
    if (value == 65) {
        switchId = 2;
        goto processSwitch;
    }
    if (value == 66) {
        switchId = 3;
        goto processSwitch;
    }
    if (value == 67) {
        switchId = 4;
    }
    if (switchId == 0) {
        goto done;
    }
processSwitch: {
    u8* switchState;
    u16 state;
    switchState = &gSwitchStates[switchId];
    state = *switchState;
    if (state > 1) {
        goto done;
    }
    value = 2;
    value |= state;
    *switchState = value;
#ifdef PORTABLE
    PortRumbleImpact(PORT_RUMBLE_SWITCH);
#endif
    state = 1;
    value &= state;
    state = 215;
    if (value != 0) {
        state = 216;
    }
    m4aSongNumStart(state);
}
    ScreenShakeRequestY(20, 0);
    if (gWarioCollisionData.unk_0C == 0) {
        goto done;
    }
    if (normalizedMode != 1) {
        goto done;
    }
    attribute = gWarioData.reaction;
    value = 7;
    attribute ^= value;
    value = -attribute;
    value |= attribute;
    result = value >> 31;
done:
    value = result;
directReturn:
    return value;
}

s32 ProcessSpecialBossTile(struct TileInteractionContext* ctx)
{
    /* Required for byte-exact agbcc allocation or scheduling; direct C alternatives were tested. */
    register struct TileInteractionContext* state PIN("r4");
    register u32 value PIN("r0");
    u32 result;

    state = ctx;
    result = 0;
    value = gUnk_300001B;
    if (value != 3) {
        value = 0;
        goto directReturn;
    }
    value = state->attribute;
    if (value != 10) {
        goto done;
    }
    {
        register struct BackgroundInfo* background PIN("r2");
        register u32 product PIN("r3");

        background = &gBackgroundInfo;
        result = background->bg1Width;
        value = state->yTile;
        COMPILER_BARRIER(value);
        product = value;
        product *= result;
        COMPILER_BARRIER(product);
        value = product;
        COMPILER_BARRIER(value);
        result = state->xTile;
        value += result;
        value <<= 16;
        result = (u32)GP(background->pBg1Data);
        value >>= 15;
        value += result;
        value = *(u16*)value;
        value += 1;
        value = (u16)value;
        {
            u32 yArg;
            register u32 xArg PIN("r2");

            yArg = state->yTile;
            xArg = state->xTile;
            SetBg1TileAndGraphics(value, yArg, xArg);
        }
    }
    value = state->yTile;
    value <<= 6;
    value += 32;
    result = state->xTile;
    result <<= 6;
    result += 32;
    SpawnSecondarySprite(value, result, 68);
    result = 1;
done:
    value = result;
directReturn:
    return value;
}

s32 ProcessTileInteractionAtPosition(u32 y, u32 x, u32 tile)
{
    /* Required for byte-exact agbcc allocation or scheduling; direct C alternatives were tested. */
    struct TileInteractionContext ctx;
    u32 yInput;
    u32 xInput;
    u32 tileInput;
    u32 yReg;
    u32 xReg;
    s32 result;
    s32 i;
    u16* entry;
    s32 mode;

    yInput = y;
    xInput = x;
    tileInput = tile;
    yReg = (u16)yInput;
    xReg = (u16)xInput;
    tileInput = (u16)tileInput;
    ctx.xTile = xReg;
    ctx.yTile = yReg;
    {
        struct TileInteractionContext* state;
        const u16* attributes;
        state = &ctx;
        attributes = GP(gBackgroundTileTables.attributes);
        tileInput <<= 1;
        tileInput += (u32)attributes;
        state->attribute = *(const u16*)tileInput;
        state->direction = 4;
        ctx.debrisType = 0;
        ctx.debrisOffset = 0;
        state->collisionMode = 1;
    }
    result = 0;
    mode = (u8)gSpriteTileInteractionMode;

    switch (mode) {
        case 1:
            result = ProcessTileInteractionAttribute(&ctx);
            if (result == 1) {
                break;
            }
            if (result == 2) {
                result = 0;
                break;
            }
            ActivateSwitchTile(&ctx, 0);
            break;
        case 2:
            entry = (u16*)0x02039F00;
            i = 0;
            {
                u32 stored;
                u32 high;
                u32 packed;

                stored = *entry;
                high = xReg << 8;
                packed = high;
                packed |= yReg;
                xReg = high;
                if (stored == packed) {
                    *entry = result;
                    goto eraseTile;
                }
            }
            do {
                u32 stored;
                u32 packed;

                entry++;
                i++;
                if (i > 63) {
                    break;
                }
                stored = *entry;
                packed = xReg;
                COMPILER_BARRIER2(stored, packed);
                packed |= yReg;
                if (stored == packed) {
                    register u32 zero PIN("r0");
                    zero = 0;
                    *entry = zero;
                    result = 1;
                    break;
                }
            } while (1);
            if (result) {
            eraseTile:
                SetBg1TileAndGraphics(0, ctx.yTile, ctx.xTile);
            }
            result = 1;
            break;
        case 3:
            entry = (u16*)0x02039F00;
            i = 0;
            {
                u32 stored;
                u32 high;
                u32 packed;

                stored = *entry;
                high = xReg << 8;
                packed = high;
                packed |= yReg;
                xReg = high;
                if (stored == packed) {
                    result = 1;
                } else {
                    do {
                        u32 nextStored;
                        register u32 nextPacked PIN("r0");

                        entry++;
                        i++;
                        if (i > 63) {
                            break;
                        }
                        nextStored = *entry;
                        nextPacked = xReg;
                        nextPacked |= yReg;
                        if (nextStored == nextPacked) {
                            result = 1;
                            break;
                        }
                    } while (1);
                }
            }
            if (result == 0) {
                entry = (u16*)0x02039F00;
                i = 0;
                {
                    struct TileInteractionContext* state;
                    state = &ctx;
                    goto emptyCheck;
                    do {
                        entry++;
                        i++;
                    emptyCheck:
                        if (i > 63) {
                            break;
                        }
                        if (*entry == 0) {
                            xReg |= yReg;
                            *entry = xReg;
                            SetBg1TileAndGraphics(1, state->yTile, state->xTile);
                            result = 1;
                            break;
                        }
                    } while (1);
                }
            }
            break;
        case 4:
            ProcessSpecialBossTile(&ctx);
            break;
    }
    return result;
}

void StartTileEffect(u32 type)
{
    u32 normalized;
    u32 compare;
    struct TileEffectState* effect;
    u32 zero;
    u32 stored;
    register u32 gameMode PIN("r0");

    normalized = (u8)type;
    compare = normalized;
    effect = &gTileEffectState;
    zero = 0;
    effect->timer = zero;
    effect->state = zero;
    gameMode = gUnk_300001B;
    COMPILER_BARRIER(gameMode);
    stored = 0;
    if (gameMode > 1) {
        stored = normalized;
    }
    effect->type = stored;
    if (compare <= 11) {
        effect->state = 1;
    }
}

void UpdateTileEffect(void)
{
    struct TileEffectState* effect;
    u16 x;
    u16 y;
    s32 state;
    u32 timer;

    effect = &gTileEffectState;
    state = effect->state;
    if (state == 1) {
        goto state1;
    }
    if (state <= 1) {
        goto end;
    }
    if (state == 2) {
        goto state2;
    }
    if (state == 3) {
        goto state3;
    }
    goto end;

state1:
    y = 0;
    x = 0;
    GetTileEffectPosition(effect->type, &x, &y);
    SpawnHighPriorityPrimarySprite(14, effect->type, 0, y, x);
    effect->timer = 0;
    goto advance;

state2:
    timer = effect->timer;
    if (timer <= 74) {
        timer++;
        effect->timer = timer;
        goto end;
    }
    effect->timer = 0;
advance:
    effect->state++;
    goto end;

state3:
    ReplaceTileEffectPair(effect->type);
    effect->type |= 0x80;
    effect->timer = 0;
    effect->state = 0;
end:
    return;
}

void ReplaceTileEffectPair(u8 index)
{
    /* Required for byte-exact agbcc allocation or scheduling; direct C alternatives were tested. */
    u32 tableIndex;
    u32 offset;
    const u8* table;
    u16 x;
    u16 y;
    struct BackgroundInfo* background;
    u32 leftX;
    s32 i;
    u32 product;
    register u32 work PIN("r2");
    register u16 yArg PIN("r1");
    register u32 scratch PIN("r3");
    const u16* map;

    tableIndex = gBossTreasureType;
    tableIndex -= 1;
    offset = tableIndex * 3;
    tableIndex = offset + index;
    table = sTileEffectPositions;
    offset = tableIndex << 1;
    x = table[offset];
    table++;
    y = table[offset];
    background = &gBackgroundInfo;
    leftX = (x - 1) << 16;
    i = 1;
    do {
        product = background->bg2Width;
        product *= y;
        work = product + x;
        map = (const u16*)GP(background->pBg2Data);
        product = work << 1;
        product += (u32)map;
        work = *(const u16*)product;
        product = work + 2;
        product <<= 16;
        product >>= 16;
        SetBg2TileAndGraphics(product, y, x);

        product = background->bg2Width;
        product *= y;
        product += x;
        work = product - 1;
        map = (const u16*)GP(background->pBg2Data);
        product = work << 1;
        product += (u32)map;
        work = *(const u16*)product;
        product = work + 2;
        product <<= 16;
        product >>= 16;
        yArg = y;
        scratch = leftX;
        COMPILER_BARRIER_INPUT(scratch);
        SetBg2TileAndGraphics(product, yArg, (u16)(scratch >> 16));

        i--;
        y = (u16)(y - 1);
    } while (i >= 0);
}

void GetTileEffectPosition(u8 index, u16* y, u16* x)
{
    u32 group;
    u32 offset;
    const u8* table;

    group = gBossTreasureType - 1;
    offset = (group << 1) + group;
    offset += index;
    table = sTileEffectPositions;
    offset <<= 1;
    *y = (table[offset] << 6) + 32;
    table++;
    *x = table[offset] << 6;
}

void RefreshCollectedTileEffects(void)
{
    s32 index;
    u8* collection;
    s32 count;
    u32 address;

    if (gUnk_300001B != 3) {
        goto done;
    }

    gBossTreasureType = 5;
    index = 0;
    address = (u32)gCurrentCollection;
    address += 40;
    collection = (u8*)address;
    count = 3;
    do {
        if ((s32)(*collection << 31) == 0) {
            ReplaceTileEffectPair((u8)index);
        }
        index++;
        if ((s32)(*collection << 30) >= 0) {
            ReplaceTileEffectPair((u8)index);
        }
        index++;
        if ((s32)(*collection << 29) >= 0) {
            ReplaceTileEffectPair((u8)index);
        }
        index++;
        collection += 24;
        count--;
    } while (count >= 0);

done:
    return;
}

void UpdateWarioTileInteractions(void)
{
    /* Required for byte-exact agbcc allocation or scheduling; direct C alternatives were tested. */
    volatile s32 stack[5];
    s32 right;
    s32 bottom;
    s32 dx;
    register s32 edge PIN("r4");
    s32 x;
    s32 y;
    struct WarioData* wario;
    TileCoordinateHandler* entry;

    if (gSubGameMode == 8) {
        return;
    }
    {
        u32 work;
        register u32 timer PIN("r1");
        work = (u32)&gMainTimer;
        timer = *(u8*)work;
        work = 1;
        work &= timer;
        if (work != 0) {
            UpdateWarioEnvironmentalTiles();
            return;
        }
    }

    {
        register u32 zero PIN("r2");
        u32 ptr;
        u32 reaction;
        zero = 0;
        stack[3] = zero;
        ptr = (u32)&gWarioData;
        reaction = *(u8*)ptr;
        wario = (struct WarioData*)ptr;
        if (reaction == 2) {
            edge = 1;
            stack[3] = edge;
        }
    }
repeat: {
    register s32 mode PIN("r6");
    mode = stack[3];
    if (mode == 1) {
        s32 zero;
        register s32 three PIN("r1");
        register s32 negThree PIN("r2");
        zero = 0;
        stack[0] = zero;
        three = 3;
        bottom = three;
        negThree = 3;
        negThree = -negThree;
        stack[1] = negThree;
        right = three;
    } else {
        edge = 0;
        stack[0] = edge;
        bottom = edge;
        y = 0;
        stack[1] = y;
        right = y;
    }
}

    {
        s32 xPosition;
        s32 yPosition;
        register s32 work PIN("r0");
        register s32 index;
        register struct BackgroundInfo* background PIN("r5");
        register s32 extra PIN("r6");

        xPosition = wario->xPosition;
        {
            register u32 hitboxIndex PIN("r1");

            hitboxIndex = 54;
            {
                register u32 r4Live PIN("r4");

                /* Keep r4 live across the C read to preserve the target index register. */
                ASM_BARRIER(: "=r"(r4Live));
                work = *(const s16*)((const u8*)wario + hitboxIndex);
                ASM_BARRIER(: : "r"(r4Live));
            }
        }
        work = xPosition + work;
        work += right;
        work >>= 6;
        right = work;
        background = &gBackgroundInfo;
        work = background->bg1Width;
        if (right > work) {
            right = work;
        }

        work = wario->hitboxOffsetLeft;
        work = xPosition + work;
        extra = stack[1];
        edge = work + extra;
        if (edge < 0) {
            edge = 0;
        }
        edge >>= 6;
        stack[1] = edge;

        yPosition = wario->yPosition;
        index = 52;
        work = *(s16*)((u8*)wario + index);
        work = yPosition + work;
        extra = stack[0];
        edge = work + extra;
        if (edge < 0) {
            edge = 0;
        }
        edge >>= 6;
        stack[0] = edge;

        index = 56;
        work = *(s16*)((u8*)wario + index);
        work = yPosition + work;
        work += bottom;
        work >>= 6;
        bottom = work;
        work = background->bg1Height;
        if (bottom > work) {
            bottom = work;
        }

        work = gPreviousXPosition;
        xPosition -= work;
        COMPILER_BARRIER(xPosition);
        dx = xPosition;
        work = gPreviousYPosition;
        yPosition -= work;
        stack[2] = yPosition;
    }

    edge = 0;
    do {
        y = stack[0];
        if (dx != 0) {
            x = stack[1];
            if (dx >= 0) {
                x = right;
            }
            edge++;
        } else {
            x = stack[1];
            if (edge != 0) {
                x = right;
            }
        }
        if (y <= bottom) {
            register TileCoordinateHandler* table PIN("r1");
            register u32 mode PIN("r2");
            register u32 offset PIN("r0");
            table = gTileCoordinateHandlers;
            mode = stack[3];
            offset = mode << 2;
#ifdef PORTABLE
            // The port builds this table natively, of 8-byte pointers in the executable.
            entry = &table[mode];
#else
            entry = (TileCoordinateHandler*)(offset + (u32)table);
#endif
            x <<= 16;
            do {
                u32 arg0;
                u32 arg1;
                TileCoordinateHandler call;
                arg0 = (u16)y;
                call = *entry;
                arg1 = (u16)(x >> 16);
                call(arg0, arg1);
                y++;
            } while (y <= bottom);
        }
        edge++;
    } while (edge <= 1);

    edge = 0;
    do {
        x = stack[1];
        y = dx;
        COMPILER_BARRIER(y);
        if (y != 0) {
            y = stack[0];
            if (stack[2] >= 0) {
                y = bottom;
            }
            edge++;
        } else {
            y = stack[0];
            if (edge != 0) {
                y = bottom;
            }
        }
        {
            s32 nextEdge;
            nextEdge = edge + 1;
            if (x <= right) {
                register TileCoordinateHandler* table PIN("r1");
                register u32 mode PIN("r2");
                register u32 offset PIN("r0");
                table = gTileCoordinateHandlers;
                mode = stack[3];
                offset = mode << 2;
#ifdef PORTABLE
                // The port builds this table natively, of 8-byte pointers in the executable.
                entry = &table[mode];
#else
                entry = (TileCoordinateHandler*)(offset + (u32)table);
#endif
                {
                    register u32 yShift PIN("r4");
                    yShift = y;
                    yShift <<= 16;
                    do {
                        u32 arg0;
                        u32 arg1;
                        TileCoordinateHandler call;
                        arg1 = (u16)x;
                        call = *entry;
                        arg0 = yShift >> 16;
                        stack[4] = nextEdge;
                        call(arg0, arg1);
                        x++;
                        nextEdge = stack[4];
                    } while (x <= right);
                }
            }
            edge = nextEdge;
        }
    } while (edge <= 1);

    edge = stack[3];
    if (edge != 0) {
        y = 0;
        stack[3] = y;
        wario = &gWarioData;
        goto repeat;
    }
}

void ProcessTileAtPosition(u32 y, u32 x)
{
    /* Required for byte-exact agbcc allocation or scheduling; direct C alternatives were tested. */
    u32 yReg;
    u32 xReg;
    struct BackgroundInfo* background;
    struct BackgroundInfo* initialBackground;
    register struct BackgroundTileTables* tables PIN("r3");
    u32 attribute;
    u32 savedAttribute;
    u8* switches;
    struct AGB_ALIGN {
        volatile u32 bg0;
        volatile u32 attribute;
        volatile u32 bg2;
    } slots;
    u32 tile;
    u32 work;
    u32 amount;

    yReg = (u16)y;
    xReg = (u16)x;
    initialBackground = &gBackgroundInfo;
    work = initialBackground->bg1Width;
    work *= yReg;
    work += xReg;
    tile = GP(initialBackground->pBg1Data)[work];
    tables = &gBackgroundTileTables;
    attribute = GP(tables->attributes)[tile];
    slots.attribute = attribute;
    background = initialBackground;

    if (attribute == 0x70) {
        ClearTileAndSpawnPrimaryEffect(yReg, xReg);
        return;
    }
    if (attribute == 0x72) {
        ClearTileAndSpawnAlternateEffect(yReg, xReg);
        return;
    }

    switches = gSwitchStates;
    if ((switches[1] & 1) != 0) {
        if (attribute == 0x48) {
            ClearTileAndSpawnPrimaryEffect(yReg, xReg);
            return;
        }
        if (attribute == 0x49) {
            ClearTileAndSpawnAlternateEffect(yReg, xReg);
            return;
        }
    }

    work = switches[4] & 1;
    savedAttribute = slots.attribute;
    if (work != 0) {
        if (savedAttribute == 0x4A) {
            ClearTileAndSpawnPrimaryEffect(yReg, xReg);
            return;
        }
        if (savedAttribute == 0x4B) {
            ClearTileAndSpawnAlternateEffect(yReg, xReg);
            return;
        }
    }

    work = savedAttribute - 0x74;
    if (work <= 2) {
        if (gTimerState > 3) {
            return;
        }
        if (gWarioPauseTimer != 0) {
            return;
        }
        if (gHeartMeter.current == 0 && gUnk_3000022 == 0) {
            return;
        }

        SetBg1Tile(0, yReg, xReg);
        work = slots.attribute;
        amount = 3;
        if (work != 0x75) {
            amount = 1;
            if (work == 0x76) {
                amount = 5;
            }
        }

        {
            const u16* soundTable;
            u32 soundIndex;
            register u32 warioState PIN("r1");

            soundTable = sTileInteractionSoundIds;
            soundIndex = slots.attribute;
            soundIndex -= 0x74;
            warioState = (u32)&gWarioData;
            warioState = *(u8*)warioState;
            soundIndex <<= 2;
            if (warioState == 1) {
                soundIndex += 2;
            }
            soundIndex += (u32)soundTable;
            m4aSongNumStart(*(const u16*)soundIndex);
        }

        work = gHeartMeter.filling;
        work += amount;
        gHeartMeter.filling = work;
        work = gHeartMeter.current + gHeartMeter.filling;
        if ((s32)work > 8) {
            gHeartMeter.filling = 8 - gHeartMeter.current;
        }
        gHeartMeter.unk_2 = gHeartMeter.filling << 3;
        gHeartMeter.unk_3 = 0;
        return;
    }

    if (gCurrentRoomHeader.bg0Param == 16) {
        work = background->bg0Width;
        work *= yReg;
        work += xReg;
        {
            u16* map;
            register u32 mapTile PIN("r4");
            register const u16* attributes PIN("r1");
            register u32 address PIN("r0");
            map = GP(background->pBg0Data);
            mapTile = map[work];
            attributes = GP(tables->attributes);
            address = mapTile << 1;
            address += (u32)attributes;
            work = *(u16*)address;
        }
    } else {
        work = 0;
    }
    slots.bg0 = work;

    work = background->bg2Width;
    work *= yReg;
    work += xReg;
    {
        u16* map;
        register u32 mapTile PIN("r4");
        register const u16* attributes PIN("r1");
        register u32 address PIN("r0");
        map = GP(background->pBg2Data);
        mapTile = map[work];
        attributes = GP(tables->attributes);
        address = mapTile << 1;
        address += (u32)attributes;
        work = *(u16*)address;
    }
    slots.bg2 = work;

    switch (gWarioData.reaction) {
        case 0:
            if (gWarioData.damageTimer != 0) {
                return;
            }
            if (ProcessWarioReactionTile(savedAttribute, 19) != 0) {
                return;
            }
            if (slots.attribute == 0x69) {
                gWarioData.reaction = 2;
                GF(sWarioPoseRequestFuncTable[2])(0);
            }
            break;
        case 1:
            if (gWarioData.damageTimer == 0) {
                ProcessWarioReactionTile(savedAttribute, 6);
            }
            break;
        case 8:
            if (gWarioData.damageTimer == 0) {
                ProcessWarioReactionTile(savedAttribute, 19);
            }
            break;
    }
}

s32 ProcessWarioReactionTile(s32 attribute, s32 pose)
{
    /* Required for byte-exact agbcc allocation or scheduling; direct C alternatives were tested. */
    s32 attributeReg;
    s32 poseReg;
    s32 hit;
    register s32 offset0 PIN("r0");

    attributeReg = attribute;
    poseReg = pose;
    hit = 0;
    if (attributeReg == 1) {
        hit = 1;
    } else {
        offset0 = attributeReg - 0x90;
        if ((u32)offset0 <= 7) {
            s32 expected;
            s32 offset;
            register u32 value PIN("r0");
            u32 switchValue;

            expected = 1;
            offset = offset0;
            if (offset > 3) {
                value = 3;
                offset &= value;
                expected = 0;
            }
            offset++;
            value = (u32)gSwitchStates;
            value = offset + value;
            switchValue = *(u8*)value;
            value = 1;
            value &= switchValue;
            if (value != expected) {
                hit = 1;
            }
        } else {
            offset0 = attributeReg - 0x7E;
            if ((u32)offset0 <= 3) {
                struct WarioData* wario;
                u32 position;

                if (attributeReg == 0x81) {
                    wario = &gWarioData;
                    position = (u16)wario->hitboxOffsetTop;
                    position += wario->yPosition;
                    position &= 0x3F;
                    if (position <= 7) {
                        hit = 1;
                    }
                } else if (attributeReg == 0x80) {
                    wario = &gWarioData;
                    position = (u16)wario->hitboxOffsetBottom;
                    position += wario->yPosition;
                    position &= 0x3F;
                    if (position > 0x37) {
                        hit = 1;
                    }
                } else if (attributeReg == 0x7E) {
                    wario = &gWarioData;
                    position = (u16)wario->hitboxOffsetLeft;
                    position += wario->xPosition;
                    position &= 0x3F;
                    if (position <= 7) {
                        hit = 1;
                    }
                } else if (attributeReg == 0x7F) {
                    wario = &gWarioData;
                    position = (u16)wario->hitboxOffsetRight;
                    position += wario->xPosition;
                    position &= 0x3F;
                    if (position > 0x37) {
                        hit = 1;
                    }
                }
            } else if (gUnk_300001B == 3 && attributeReg == 0x9C && !(gSwitchStates[2] & 1)) {
                struct WarioData* wario;
                register u32 position PIN("r1");

                wario = &gWarioData;
                position = (u16)wario->hitboxOffsetBottom;
                position += wario->yPosition;
                position &= 0x3F;
                if (position > 0x2F) {
                    hit = 1;
                }
            }
        }
    }

    if (hit) {
        if (gWarioData.reaction == 8) {
            gWarioData.reaction = 0;
        }
        GF(sWarioPoseRequestFuncTable[gWarioData.reaction])((u8)poseReg);
        ChangeStageScoreAndDropCoins(-10);
    }
    return hit;
}

void ProcessAlternateTileAtPosition(u32 y, u32 x)
{
    struct TileInteractionContext ctx;
    u32 yTile;
    u32 xTile;
    u32 attribute;
    u32 zero;
    struct TileInteractionContext* collisionState;
    const u16* attributes;
    struct TileInteractionContext* attributeState;

    yTile = (u16)y;
    xTile = (u16)x;
    attribute = ((u16*)GP(gBackgroundInfo.pBg1Data))[yTile * gBackgroundInfo.bg1Width + xTile];
    attributes = GP(gBackgroundTileTables.attributes);
    attribute = attributes[attribute];
    if (gWarioData.reaction == 2 && (u8)(gWarioData.pose - 7) <= 1) {
        struct TileInteractionContext* state;
        struct TileInteractionContext* setupState;

        state = &ctx;
        zero = 0;
        state->yTile = yTile;
        attributeState = state;
        attributeState->xTile = xTile;
        attributeState->attribute = attribute;
        setupState = &ctx;
        setupState->direction = 3;
        state = &ctx;
        state->debrisType = zero;
        state->debrisOffset = zero;
        state->soundIndex = zero;
        collisionState = setupState;
        collisionState->collisionMode = 1;
        ProcessFragileTileAttribute(&ctx);
    }
}

void ClearTileAndSpawnPrimaryEffect(u32 y, u32 x)
{
    u32 yReg;
    u32 xReg;
    yReg = y;
    xReg = x;
    yReg = (u16)yReg;
    xReg = (u16)xReg;
    SetBg1Tile(0, yReg, xReg);
    yReg <<= 6;
    xReg <<= 6;
    SpawnOnePointTileEffect(yReg, xReg);
}

void ClearTileAndSpawnAlternateEffect(u32 y, u32 x)
{
    u32 yReg;
    u32 xReg;
    yReg = y;
    xReg = x;
    yReg = (u16)yReg;
    xReg = (u16)xReg;
    SetBg1Tile(0, yReg, xReg);
    yReg <<= 6;
    xReg <<= 6;
    SpawnTenPointTileEffect(yReg, xReg);
}

void UpdateWarioEnvironmentalTiles(void)
{
    /* Required for byte-exact agbcc allocation or scheduling; direct C alternatives were tested. */
    struct AGB_ALIGN Frame {
        u32 bg0Attributes[2];
        u32 bg1Attributes[2];
        u32 bg2Attributes[2];
        s32 xTiles[2];
        u32 bg1Row;
        u32 bg1Map;
        u32 bg2Row;
        u32 bg2Map;
        GBA_PTR(u32*) bg2AttributesPtr;
        u32 stackPadding;
    } frame;
    s32 yTile;
    s32 i;
    register s32* xTiles PIN("r10");
    struct BackgroundInfo* background;
    u32* bg1Attributes;
    const u16* attributes;
    u32 roomParam;
    register u32* outputCursor;

    frame.xTiles[0] = (gWarioData.xPosition + (gWarioData.hitboxOffsetRight >> 1)) >> 6;
    {
        s32 secondX;
        s32* xTemp;
        secondX = (gWarioData.xPosition + (gWarioData.hitboxOffsetLeft >> 1)) >> 6;
        xTemp = frame.xTiles;
        xTemp[1] = secondX;
        yTile = (gWarioData.yPosition + (gWarioData.hitboxOffsetTop >> 1)) >> 6;
        i = 0;
        xTiles = xTemp;
    }
    background = &gBackgroundInfo;
    bg1Attributes = frame.bg1Attributes;
    {
        register u32* bg2Ptr PIN("r1");
        bg2Ptr = (u32*)&frame;
        ASM_BARRIER_VOLATILE(: "+r"(bg2Ptr) : : "memory");
        bg2Ptr += 4;
        GP_SET(frame.bg2AttributesPtr, bg2Ptr);
    }
    {
        register u32 row PIN("r2");
        row = yTile;
        row *= background->bg1Width;
        frame.bg1Row = row;
    }
    attributes = GP(gBackgroundTileTables.attributes);
    {
        register u32 row PIN("r1");
        row = yTile;
        row *= background->bg2Width;
        frame.bg2Row = row;
    }
    {
        register u32 map PIN("r2");
        map = (u32)GP(background->pBg2Data);
        frame.bg2Map = map;
    }
    outputCursor = frame.bg0Attributes;
    frame.bg1Map = (u32)GP(background->pBg1Data);
    roomParam = gCurrentRoomHeader.bg0Param;

    do {
        u32 offset;
        s32* xEntry;
        register u32 value PIN("r0");

        offset = i << 2;
        xEntry = (s32*)(offset + (u32)xTiles);
        value = *xEntry;
        value = frame.bg1Row + value;
        value <<= 1;
        {
            register u32 bg1Map = frame.bg1Map;
            value += bg1Map;
        }

        value = *(volatile u16*)value;
        {
            register u32 bgDest PIN("r1");
            bgDest = (u32)bg1Attributes;
            ASM_BARRIER_VOLATILE(: "+r"(value), "+r"(bgDest) : "r"(offset), "r"(attributes) : "memory");
            bgDest += offset;
            value <<= 1;
            value += (u32)attributes;
            value = *(volatile u16*)value;
            *(volatile u32*)bgDest = value;
        }

        value = *xEntry;
        value = frame.bg2Row + value;
        value <<= 1;
        {
            register u32 bg2Map = frame.bg2Map;
            value += bg2Map;
        }
        value = *(volatile u16*)value;
        {
            register u32* out PIN("r2");
            register u32* base PIN("r1") = GP(frame.bg2AttributesPtr);
            out = base + i;
            value <<= 1;
            value += (u32)attributes;
            value = *(volatile u16*)value;
            *out = value;
        }
        if (roomParam == 16) {
            value = yTile * background->bg0Width;
            value += *xEntry;
            COMPILER_BARRIER_MEMORY(value);
#ifdef PORTABLE
            // Transition-edge coordinates can yield a negative tile index. The GBA
            // wraps the byte address at 32 bits; native pointer indexing does not.
            {
                u32 mapAddress = (u32)GP(background->pBg0Data);
                mapAddress += value << 1;
                value = *(u16*)mapAddress;
            }
#else
            value = GP(background->pBg0Data)[value];
#endif
            value = attributes[value];
        } else {
            value = 0;
        }
        *(volatile u32*)outputCursor = value;
        ASM_BARRIER_VOLATILE(: "+r"(outputCursor) : "r"(value) : "memory");
        outputCursor++;
        i++;
    } while (i <= 1);

    i = 0;
    {
        register u32 yShift PIN("r7");
        s32* xEntry;
        u32* attributeEntry;
        yShift = (u32)yTile << 16;
        xEntry = xTiles;
        attributeEntry = bg1Attributes;
        do {
            u32 value;
            value = *attributeEntry;
            if (value == 3) {
                u16 xArg;
                u16 yArg;
                xArg = *(u16*)xEntry;
                yArg = (u16)(yShift >> 16);
                if (TryTriggerRoomTransitionAtTile(yArg, xArg)) {
                    return;
                }
            } else if (value == 9) {
                u16 xArg;
                u16 yArg;
                xArg = *(u16*)xEntry;
                yArg = (u16)(yShift >> 16);
                if (TryTriggerRoomTransitionAtTile(yArg, xArg)) {
                    m4aSongNumStart(25);
                    return;
                }
            } else if (value == 8) {
                gStageExitType = 6;
                gUnk_3000025 = 0;
                gStageEntryPauseMenuDisabled = 0;
                gSubGameMode = 3;
                StartRoomTransitionFade(2);
                {
                    const struct MusicPlayer* table;
                    table = gMPlayTable;
                    MPlayStop(GP(table[3].info));
                    m4aMPlayFadeOut(GP(table[0].info), 2);
                    m4aMPlayFadeOut(GP(table[1].info), 2);
                    m4aMPlayFadeOut(GP(table[2].info), 2);
                }
                return;
            }
            xEntry = (s32*)((u32)xEntry + 4);
            attributeEntry++;
            i++;
        } while (i <= 1);
    }

    switch (gWarioData.reaction) {
        case 10:
            if (gWarioData.pose != 6 && (frame.bg1Attributes[0] == 0x7D || frame.bg1Attributes[1] == 0x7D)) {
                GF(sWarioPoseRequestFuncTable[10])(6);
            }
            break;
        case 5:
            if (gWarioData.pose != 0) {
                RequestWarioPoseForTileAttributes(frame.bg0Attributes, 10);
            }
            break;
        case 9:
            if (gWarioData.pose != 0) {
                RequestWarioPoseForTileAttributes(frame.bg0Attributes, 4);
            }
            break;
    }
}

s32 RequestWarioPoseForTileAttributes(const u32* attributes, u32 pose)
{
    struct WarioData* wario;
    struct WarioData* loadedWario;
    u32 currentPose;
    s32 i;

    loadedWario = &gWarioData;
    currentPose = loadedWario->pose;
    wario = loadedWario;
    if (currentPose == pose) {
        return 0;
    }
    for (i = 0; i <= 5; i++, attributes++) {
        if (*attributes == 124) {
            GF(sWarioPoseRequestFuncTable[wario->reaction])((u8)pose);
            return 1;
        }
    }
    return 0;
}

s32 SetBg1Tile(u32 tile, u32 y, u32 x)
{
    /* Required for byte-exact agbcc allocation or scheduling; direct C alternatives were tested. */
    u32 tileReg;
    u32 yReg;
    u32 xReg;
    s32 result;

    tileReg = (u16)tile;
    yReg = (u16)y;
    xReg = (u16)x;
    result = 1;
    {
        struct BackgroundInfo* background;
        u32 width;
        register u32 product PIN("r2");
        register u32 index PIN("r0");
        register u16* mapEntry PIN("r1");

        background = &gBackgroundInfo;
        width = background->bg1Width;
        product = yReg;
        product *= width;
        COMPILER_BARRIER(product);
        index = product;
        COMPILER_BARRIER(index);
        index = xReg + index;
        index <<= 16;
        mapEntry = GP(background->pBg1Data);
        index >>= 15;
        mapEntry = (u16*)(index + (u32)mapEntry);
        *mapEntry = tileReg;
        if (tileReg == 0) {
            u16* count;
            register u32 offset PIN("r0");
            u16* list;

            count = &gClearedRoomTileCount;
            offset = *count;
            offset <<= 1;
            list = (u16*)0x0203A000;
            offset += (u32)list;
            mapEntry = (u16*)((s32)mapEntry >> 1);
            *(u16*)offset = (u32)mapEntry;
            offset = *count;
            offset++;
            *count = offset;
        }
    }

    {
        register u32 cameraTileY PIN("r4");
        u32 value;

        value = gBg1YPosition;
        cameraTileY = value >> 6;
        value = cameraTileY;
        value += 10;
        if ((s32)yReg > (s32)value) {
            result = 0;
        }

        value = cameraTileY - 4;
        if ((s32)value <= (s32)yReg) {
            value += 17;
            if ((s32)yReg <= (s32)value) {
                volatile u16* dst;
                register u32 mask PIN("r1");
                register u32 work PIN("r0");
                register u32 columnOffset PIN("r4");
                u32 tileIndex;
                u32 nextIndex;
                const u16* tiles;

                dst = (volatile u16*)0x06001000;
                work = 16;
                work &= xReg;
                if (work != 0) {
                    dst = (volatile u16*)0x06001800;
                }
                mask = 15;
                work = xReg;
                work &= mask;
                columnOffset = work << 1;
                work = yReg;
                work &= mask;
                work <<= 6;
                COMPILER_BARRIER2(work, columnOffset);
                work += columnOffset;
                work <<= 1;
                dst = (volatile u16*)((u32)dst + work);

                work = tileReg << 18;
                tileIndex = work >> 16;
                mask = tileIndex;
                {
                    struct BackgroundTileTables* tables;
                    volatile u16* lowerDst;
                    volatile u16* lastDst;

                    tables = &gBackgroundTileTables;
                    work = 1;
                    tileIndex |= work;
                    nextIndex = tileIndex;
                    tiles = GP(tables->top);

                    mask <<= 1;
                    mask += (u32)tiles;
                    work = *(const u16*)mask;
                    dst[0] = work;

                    work = nextIndex + 1;
                    tileIndex = (u16)work;
                    COMPILER_BARRIER(tileIndex);
                    mask = tileIndex;
                    nextIndex <<= 1;
                    nextIndex += (u32)tiles;
                    work = *(const u16*)nextIndex;
                    dst[1] = work;

                    lowerDst = dst;
                    lowerDst += 32;
                    work = mask + 1;
                    work <<= 16;
                    mask <<= 1;
                    mask += (u32)tiles;
                    mask = *(const u16*)mask;
                    lowerDst[0] = mask;

                    lastDst = dst;
                    lastDst += 33;
                    work >>= 15;
                    work += (u32)tiles;
                    work = *(const u16*)work;
                    lastDst[0] = work;
                }
            }
        }
    }
    return result;
}

void SetBg1TileAndGraphics(u32 tile, u32 y, u32 x)
{
    /* Required for byte-exact agbcc allocation or scheduling; direct C alternatives were tested. */
    u32 tileReg;
    register u32 yReg PIN("r3");
    register u32 xReg PIN("r2");
    u32 product;
    u32 xLong;
    u32 yLong;
    u32 work;

    tileReg = (u16)tile;
    yReg = (u16)y;
    yLong = yReg;
    xReg = (u16)x;
    xLong = xReg;

    {
        struct BackgroundInfo* backgroundReg;
        u32 width;
        register u32 mapWork PIN("r0");

        backgroundReg = &gBackgroundInfo;
        width = backgroundReg->bg1Width;
        product = yReg;
        product *= width;
        COMPILER_BARRIER(product);
        mapWork = product;
        mapWork = xReg + mapWork;
        mapWork <<= 16;
        backgroundReg = (struct BackgroundInfo*)backgroundReg;
        backgroundReg = (struct BackgroundInfo*)GP(backgroundReg->pBg1Data);
        mapWork >>= 15;
        mapWork += (u32)backgroundReg;
        *(u16*)mapWork = tileReg;
    }

    work = gBg1YPosition;
    product = work >> 6;
    work = product - 4;
    if ((s32)work <= (s32)yReg) {
        work += 17;
        if ((s32)yReg <= (s32)work) {
            volatile u16* dst;
            register u32 tileIndex PIN("r4");
            u32 originalIndex;
            u32 nextIndex;
            register u32 tableReg PIN("r3");

            dst = (volatile u16*)0x06001000;
            if (xReg & 16) {
                dst = (volatile u16*)0x06001800;
            }

            work = 15;
            xLong &= work;
            product = xLong << 1;
            yLong &= work;
            work = yLong << 6;
            work += product;
            work <<= 1;
            dst = (volatile u16*)((u32)dst + work);

            tileIndex = tileReg;
            work = tileIndex << 18;
            COMPILER_BARRIER2(work, tileIndex);
            tileIndex = work >> 16;
            originalIndex = tileIndex;
            tableReg = (u32)&gBackgroundTileTables;
            work = 1;
            tileIndex |= work;
            nextIndex = tileIndex;
            COMPILER_BARRIER5(originalIndex, tileIndex, nextIndex, tableReg, work);
            tableReg = *(u32*)tableReg;

            originalIndex <<= 1;
            originalIndex += tableReg;
            work = *(const u16*)originalIndex;
            dst[0] = work;

            work = nextIndex + 1;
            work <<= 16;
            tileIndex = work >> 16;
            originalIndex = tileIndex;
            COMPILER_BARRIER2(tileIndex, originalIndex);
            nextIndex <<= 1;
            nextIndex += tableReg;
            work = *(const u16*)nextIndex;
            dst[1] = work;

            nextIndex = (u32)dst;
            nextIndex += 64;
            work = originalIndex + 1;
            work <<= 16;
            originalIndex <<= 1;
            originalIndex += tableReg;
            originalIndex = *(const u16*)originalIndex;
            *(volatile u16*)nextIndex = originalIndex;

            originalIndex = (u32)dst;
            originalIndex += 66;
            work >>= 15;
            work += tableReg;
            work = *(const u16*)work;
            *(volatile u16*)originalIndex = work;
        }
    }
}

void SetBg2TileAndGraphics(u16 tile, u16 y, u16 x)
{
    /* Required for byte-exact agbcc allocation or scheduling; direct C alternatives were tested. */
    register u32 value PIN("r0");
    u32 arg1;
    u32 arg2;
    register u32 arg3 PIN("r3");
    register u32 work PIN("r4");
    u32 dst;
    u32 tileValue;

    value = tile;
    value <<= 16;
    value >>= 16;
    COMPILER_BARRIER(value);
    tileValue = value;
    arg1 = y;
    arg1 <<= 16;
    arg1 >>= 16;
    COMPILER_BARRIER(arg1);
    arg3 = arg1;
    arg2 = x;
    arg2 <<= 16;
    arg2 >>= 16;

    arg1 = (u32)&gBackgroundInfo;
    value = *(u16*)(arg1 + 20);
    work = arg3;
    work *= value;
    COMPILER_BARRIER(work);
    value = work;
    COMPILER_BARRIER(value);
    value = arg2 + value;
    value <<= 16;
    arg1 = *(u32*)(arg1 + 16);
    value >>= 15;
    value += arg1;
    *(u16*)value = tileValue;

    dst = 0x06002000;
    if (arg2 & 16) {
        dst = 0x06002800;
    }

    value = 15;
    arg2 &= value;
    work = arg2 << 1;
    arg3 &= value;
    value = arg3 << 6;
    value += work;
    value <<= 1;
    dst += value;

    value = tileValue << 18;
    work = value >> 16;
    arg1 = work;
    arg3 = (u32)&gBackgroundTileTables;
    value = 1;
    work |= value;
    COMPILER_BARRIER(work);
    arg2 = work;
    arg3 = *(u32*)arg3;
    arg1 <<= 1;
    arg1 += arg3;
    value = *(u16*)arg1;
    *(u16*)dst = value;

    value = arg2 + 1;
    value <<= 16;
    work = value >> 16;
    COMPILER_BARRIER(work);
    arg1 = work;
    arg2 <<= 1;
    arg2 += arg3;
    value = *(u16*)arg2;
    *(u16*)(dst + 2) = value;

    arg2 = dst;
    arg2 += 64;
    value = arg1 + 1;
    value <<= 16;
    arg1 <<= 1;
    arg1 += arg3;
    arg1 = *(u16*)arg1;
    *(u16*)arg2 = arg1;

    arg1 = dst;
    arg1 += 66;
    value >>= 15;
    value += arg3;
    value = *(u16*)value;
    *(u16*)arg1 = value;
}
