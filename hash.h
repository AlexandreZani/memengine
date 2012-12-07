#ifndef __HASH_H
#define __HASH_H
#include <stdint.h>

extern uint32_t hash_32(uint32_t *data, uint32_t len);
extern uint32_t hash_8(uint8_t *data, uint32_t len);

#endif
