#ifndef SHOP_H
#define SHOP_H

#include "oam.h"
#include "types.h"

struct AGB_ALIGN ItemShopCursorState {
    u16 animationTimer;
    u16 animationFrame;
    s16 x;
    s16 y;
    u8 item;
    u8 padding[3];
};

struct AGB_ALIGN ItemShopAnimationState {
    u16 animationTimer;
    u16 animationFrame;
    s16 x;
    s16 y;
};

u32 ItemShopSubroutine(void);
void InitializeItemShop(void);
void SetItemShopVBlankCallback(void);
void ItemShopVBlank(void);
u32 HandleItemShopInput(void);
void MoveItemShopCursor(u32 direction);
u32 UpdateItemShopMedalDeduction(void);
void Shop_DrawText(const u8* text, u32 destination, u32 length);
void DrawItemShopSprites(void);

#endif  // SHOP_H
