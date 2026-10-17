u8 ResolveWarioCeilingCollision(void)
{
    u16 adjX;
    u16 adjY;
    u16 sec;
    u32 side;
    u32 left;
    s16 offset;
    u32 savedX;
    u32 right;
    u16 pointValue;
    u32 result;
    u32 x;
    u32 y;
    u32 currentY;
    u32 previousY;
    u32 newY;

    side = gWarioCollisionData.unk_08;
    left = gWarioCollisionData.unk_04;
    right = gWarioCollisionData.unk_05;
    offset = gWarioCollisionData.unk_06;

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
                u32 initialY;
                x = (u16)(adjX + sWarioCollisionPointTable[side * 3 + left]);
                gWarioData.yPosition = (gWarioData.yPosition + gPreviousYPosition) >> 1;
                initialY = (u16)(gWarioData.yPosition + 1);
                CheckWarioTileCollision(x, initialY, &adjX, &adjY, &sec);
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
    result = ((u32 (*)(u32, u32, u16 *, u16 *, u16 *))CheckWarioTileCollision)(x, y, &adjX, &adjY, &sec);
    if (sec != 0) {
        if (result != 0) {
            gWarioData.unk_1C = result;
            newY = adjY;
            goto setYFD;
        }
        if (gPreviousYPosition < adjY) {
            y = gPreviousYPosition;
            result = ((u32 (*)(u32, u32, u16 *, u16 *, u16 *))CheckWarioTileCollision)(x, y, &adjX, &adjY, &sec);
            gWarioData.unk_1C = result;
            if (result != 0) {
                newY = adjY;
                goto setYFD;
            }
            gWarioData.yPosition = adjY + 63;
            goto returnFD;
        }
        if (sec != 0x10) {
            gWarioData.xPosition = offset + (adjX - pointValue);
            x = gWarioData.xPosition;
            result = ((u32 (*)(u32, u32, u16 *, u16 *, u16 *))CheckWarioTileCollision)(x, y, &adjX, &adjY, &sec);
            gWarioData.unk_1C = result;
            if (result != 0) {
                newY = adjY;
setYFD:
                gWarioData.yPosition = newY;
returnFD:
                return 0xFD;
            }
            if (sec != 0) {
                gWarioData.yPosition = adjY - 1;
                goto returnFD;
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
    result = ((u32 (*)(u32, u32, u16 *, u16 *, u16 *))CheckWarioTileCollision)(x, y, &adjX, &adjY, &sec);
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
        result = ((u32 (*)(u32, u32, u16 *, u16 *, u16 *))CheckWarioTileCollision)(x, y, &adjX, &adjY, &sec);
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
        result = ((u32 (*)(u32, u32, u16 *, u16 *, u16 *))CheckWarioTileCollision)(x, y, &adjX, &adjY, &sec);
        if (sec != 0 && gPreviousYPosition < adjY) {
            gWarioData.unk_1C = result;
            gWarioData.yPosition = adjY - 1;
            return 0xFD;
        }
    }
    return 0xFF;
}
