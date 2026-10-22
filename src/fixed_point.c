#include "fixed_point.h"

s32 FixedMul(s32 a, s32 b)
{
    s32 res;

    res = (s16)a * (s16)b;
    res = Q_8_8_TO_SHORT(res);
    return res;
}

s32 FixedDiv(s32 a, s32 b)
{
    s32 res;

    res = Q_24_8((s16)a) / (s16)b;
    return (s16)res;
}

s32 FixedInverse(s32 a)
{
    s32 one;

    one = Q_16_16(1.0);
#ifdef PORTABLE
    // The stage-select renderer can draw the boss-door scene on the frame
    // before its scale is initialized. Use a zero affine coefficient for
    // that transient frame; native x86 division would raise SIGFPE.
    if ((s16)a == 0) return 0;
#endif
    return (s16)(one / (s16)a);
}
