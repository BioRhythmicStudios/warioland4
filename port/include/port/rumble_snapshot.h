#ifndef PORT_RUMBLE_SNAPSHOT_H
#define PORT_RUMBLE_SNAPSHOT_H
enum PortRumbleEvent {
    PORT_RUMBLE_CONTACT = 1, PORT_RUMBLE_HEAD = 2,
    PORT_RUMBLE_SWITCH = 4, PORT_RUMBLE_FROG = 8,
    PORT_RUMBLE_TRANSFORM = 16, PORT_RUMBLE_RECOVER = 32,
    PORT_RUMBLE_FIRE = 64, PORT_RUMBLE_ASH = 128,
    PORT_RUMBLE_SNOW = 256, PORT_RUMBLE_SNOW_END = 512,
    PORT_RUMBLE_FORM_IMPACT = 1024
};
void PortRumbleImpact(unsigned events);
void PortRumbleContactBegin(int slot);
void PortRumbleContactEnd(int slot);
typedef struct PortRumbleSnapshot { int active, health, shakeX, shakeY, rock, boss; unsigned events; } PortRumbleSnapshot;
void PortRumbleBossHealth(void);
void PortRumbleTileBroken(void);
void PortRumbleRead(PortRumbleSnapshot *snapshot);
#endif
