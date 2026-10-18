#ifndef GUARD_GBA_GBA_PTR_H
#define GUARD_GBA_GBA_PTR_H

// Pointers held in GBA memory: in RAM structs, I/O registers and ROM tables.
//
// On the GBA these are ordinary pointers. The PC port keeps them as 32-bit GBA addresses, so
// every struct and table keeps its GBA layout and RAM stays byte-identical to the GBA's. GBA
// memory is mapped at its real addresses there, so a GBA address is also a native pointer.
//
//   GBA_PTR(T *) name;        a data pointer of type T * held in GBA memory
//   GBA_FUNC_PTR(F) name;     a function pointer held in GBA memory; F is a pointer typedef
//   GP(x)                     the value of x, as a native T *
//   GP_SET(x, p)              stores p in x
//   GP_ADD(x, n)              x += n, and GP_SUB(x, n) for x -= n
//   GP_POSTINC(x)             x++, and GP_PREINC, GP_POSTDEC, GP_PREDEC
//   GF(x)                     the value of a function pointer, as a native function
//   GF_SET(x, f)              stores the GBA address of the function f in x
//   GBA_TYPEOF(T, expr)       type T on the GBA, the type of expr on the PC
//   GBA_AS(var, expr)         expr, converted to the type of var on the PC
//
// On the GBA every macro expands to the original expression, so agbcc's output is unchanged.
// port/tools/gbaptr_rewrite.py converts the declarations and their uses, and
// port/tools/gbaptr_derived.py the locals and casts that point into converted tables.

#ifdef PORTABLE

// The port's native and GBA function addresses (port/src/gba_functions.c).
void (*PortGbaFunctionToNative(u32 gbaAddress))(void);
u32 PortNativeFunctionToGba(void (*func)(void));
// Code the game copies into RAM and runs there: the native function to run for the copy at a
// RAM address. A function pointer holding that address (with the Thumb bit) then calls it.
void PortSetRamFunction(u32 gbaAddress, void (*func)(void));
// Reports a native address that does not fit a GBA pointer, and stops.
__attribute__((noreturn)) void PortBadGbaPointer(unsigned long long address);

static inline u32 GbaPtrAddress_(__UINTPTR_TYPE__ address)
{
    if (address >> 32)
        PortBadGbaPointer(address);
    return (u32)address;
}

#define GBA_PTR(T) union __attribute__((packed, aligned(4))) { u32 a; T t_[0]; }
#define GBA_FUNC_PTR(F) union __attribute__((packed, aligned(4))) { u32 a; F f_[0]; }

// The __typeof__ checks the assignment the GBA code makes, without evaluating it.
#define GP(x) ((__typeof__((x).t_[0]))(__UINTPTR_TYPE__)(x).a)
#define GP_SET(x, p) ((void)(__typeof__((x).t_[0] = (p)) *)0, (x).a = GbaPtrAddress_((__UINTPTR_TYPE__)(p)))
#define GP_ADD(x, n) GP_SET(x, GP(x) + (n))
#define GP_SUB(x, n) GP_SET(x, GP(x) - (n))
#define GP_PREINC(x) GP_ADD(x, 1)
#define GP_PREDEC(x) GP_SUB(x, 1)
#define GP_POSTINC(x) ({ __typeof__(GP(x)) gpOld_ = GP(x); GP_SET(x, gpOld_ + 1); gpOld_; })
#define GP_POSTDEC(x) ({ __typeof__(GP(x)) gpOld_ = GP(x); GP_SET(x, gpOld_ - 1); gpOld_; })

#define GF(x) ((__typeof__((x).f_[0]))PortGbaFunctionToNative((x).a))
#define GF_SET(x, f) ((void)(__typeof__((x).f_[0] = (f)) *)0, (x).a = PortNativeFunctionToGba((void (*)(void))(f)))

// Every GBA_PTR() is its own union type, so a local that walks a table of them takes the
// type of the table's element pointer on the PC.
#define GBA_TYPEOF(T, expr) __typeof__(expr)
#define GBA_AS(var, expr) ((__typeof__(var))(expr))

#else

#define GBA_PTR(T) T
#define GBA_FUNC_PTR(F) F

#define GP(x) (x)
#define GP_SET(x, p) ((x) = (p))
#define GP_ADD(x, n) ((x) += (n))
#define GP_SUB(x, n) ((x) -= (n))
#define GP_PREINC(x) (++(x))
#define GP_PREDEC(x) (--(x))
#define GP_POSTINC(x) ((x)++)
#define GP_POSTDEC(x) ((x)--)

#define GF(x) (x)
#define GF_SET(x, f) ((x) = (f))

#define GBA_TYPEOF(T, expr) T
#define GBA_AS(var, expr) (expr)

#endif

#endif  // GUARD_GBA_GBA_PTR_H
