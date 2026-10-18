// The libgcc division helpers that game code calls by name, with agbcc's behaviour:
// dividing by zero returns 0 instead of trapping, and INT_MIN / -1 wraps.

int __divsi3(int a, int b)
{
    if (b == 0)
        return 0;
    if (b == -1)
        return (int)(0u - (unsigned int)a);
    return a / b;
}

int __modsi3(int a, int b)
{
    if (b == 0 || b == -1)
        return 0;
    return a % b;
}

unsigned int __udivsi3(unsigned int a, unsigned int b)
{
    return b == 0 ? 0 : a / b;
}

unsigned int __umodsi3(unsigned int a, unsigned int b)
{
    return b == 0 ? 0 : a % b;
}

// m4a's asm helper (asm/m4a_asm.s, an ARM umull): the high 32 bits of an unsigned product.
unsigned int umul3232H32(unsigned int multiplier, unsigned int multiplicand)
{
    return (unsigned int)(((unsigned long long)multiplier * multiplicand) >> 32);
}
