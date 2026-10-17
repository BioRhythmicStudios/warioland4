#ifndef WARIO_MATCHED_HELPERS_H
#define WARIO_MATCHED_HELPERS_H

#include "types.h"

/*
 * These declarations preserve the ABI view used by the matched Wario-state
 * modules.  The convenience declarations in wario.h use narrower types for
 * some helpers, which changes agbcc code generation at the call sites.
 */
void CopyWarioPalette(const void* source, u32 destination, u32 size);
void UpdateWarioHorizontalCollisionOffset(void);
s32 GetAdjustedWarioXVelocity(void);
#ifdef PORTABLE
// They return u8. Only agbcc guarantees the upper bits of the register.
u8 ResolveWarioStandardCollision(void);
u8 ResolveWarioLandingCollision(void);
u8 ResolveWarioCeilingCollision(void);
u8 ResolveWarioFloorCollision(void);
#else
s32 ResolveWarioStandardCollision(void);
s32 ResolveWarioLandingCollision(void);
s32 ResolveWarioCeilingCollision(void);
s32 ResolveWarioFloorCollision(void);
#endif

#endif  // WARIO_MATCHED_HELPERS_H
