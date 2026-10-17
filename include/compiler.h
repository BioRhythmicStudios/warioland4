#ifndef COMPILER_H
#define COMPILER_H

#include "gba/defines.h"

#define COMPILER_FORGET_VALUE(value) ASM_BARRIER(: "=r"(value))
#define COMPILER_BARRIER(value) ASM_BARRIER(: "+r"(value))
#define COMPILER_BARRIER_MEMORY(value) ASM_BARRIER_VOLATILE(: "+r"(value) : : "memory")
#define COMPILER_BARRIER_INPUT(value) ASM_BARRIER(: : "r"(value))
#define COMPILER_BARRIER2(first, second) \
    ASM_BARRIER(: "+r"(first), "+r"(second))
#define COMPILER_BARRIER4(first, second, third, fourth) \
    ASM_BARRIER(: "+r"(first), "+r"(second), "+r"(third), "+r"(fourth))

#define COMPILER_BARRIER5(first, second, third, fourth, fifth) \
    ASM_BARRIER(: "+r"(first), "+r"(second), "+r"(third), "+r"(fourth), "+r"(fifth))

#endif
