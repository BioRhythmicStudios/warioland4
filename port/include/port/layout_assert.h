#ifndef GUARD_PORT_LAYOUT_ASSERT_H
#define GUARD_PORT_LAYOUT_ASSERT_H

// Checks used by port/generated/layout_asserts_*.c: the expected values are agbcc's.
#include <stddef.h>

#define LAYOUT_SIZE(type, size) \
    _Static_assert(sizeof(type) == (size), "sizeof(" #type ") differs from agbcc")
#define LAYOUT_OFFSET(type, field, offset) \
    _Static_assert(offsetof(type, field) == (offset), "offsetof(" #type ", " #field ") differs from agbcc")

#endif  // GUARD_PORT_LAYOUT_ASSERT_H
