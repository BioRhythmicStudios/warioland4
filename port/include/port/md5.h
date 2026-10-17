#ifndef GUARD_PORT_MD5_H
#define GUARD_PORT_MD5_H

#include <stddef.h>
#include <stdint.h>

void PortMd5(const void *data, size_t size, uint8_t digest[16]);

#endif  // GUARD_PORT_MD5_H
