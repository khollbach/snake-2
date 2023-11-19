#ifndef TRON_UTIL_MEMORY_H_
#define TRON_UTIL_MEMORY_H_

#define ADDR(a) ((volatile char*) a)
#define READ(a) (*ADDR(a))
#define WRITE(a, x) (*ADDR(a) = x)

#endif
