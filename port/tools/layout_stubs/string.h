// The <string.h> that gen_layout_asserts.py parses the game sources with: it runs without a libc.
#ifndef LAYOUT_STUB_STRING_H
#define LAYOUT_STUB_STRING_H
typedef __SIZE_TYPE__ size_t;
void *memcpy(void *destination, const void *source, size_t size);
void *memset(void *destination, int value, size_t size);
int memcmp(const void *a, const void *b, size_t size);
#endif
