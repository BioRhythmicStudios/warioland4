#ifndef CAMERA_H
#define CAMERA_H

#include "types.h"

struct AGB_ALIGN CameraControlArea {
    u8 mode;
    u8 unused01;
    u16 maxX;
    u16 minX;
    u16 minY;
    u16 maxY;
    u16 unused0A;
};

extern struct CameraControlArea gCameraControlAreas[2];
extern GBA_PTR(const u8*) gCurrentRoomCameraControlData;

void LoadCurrentRoomCameraControlData(void);
void UpdateCamera(void);

#endif  // CAMERA_H
