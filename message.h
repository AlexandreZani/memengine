#ifndef __MESSAGE_H
#define __MESSAGE_H

#include <stdint.h>

#include "cache.h"

#define PROTOCOL_VERSION 0x00

#define MESSAGE_TYPE_SET 0x00
#define MESSAGE_TYPE_UNSET 0x01

typedef struct __attribute__((__packed__)) {
  uint8_t protocol_version;
  uint8_t message_type;
  uint32_t key_size;
  uint32_t data_size;
} message_header_t;

void message_header_ntoh(message_header_t *message_header);
void dispatch_message(cache_t *cache, message_header_t *header, uint8_t *key,
    uint8_t *data);

#endif
