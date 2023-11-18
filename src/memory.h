#ifndef TRON_MEMORY_H_
#define TRON_MEMORY_H_

#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef int8_t i8;
typedef int16_t i16;

#define ADDR(a) ((volatile char*) a)
#define READ(a) (*ADDR(a))
#define WRITE(a, x) (*ADDR(a) = x)

#endif
