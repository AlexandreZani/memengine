#include "message.h"
#include <arpa/inet.h>

void
message_header_ntoh(message_header_t *message_header) {
  message_header->key_size = ntohl(message_header->key_size);
  message_header->data_size = ntohl(message_header->data_size);
}


void
dispatch_message(cache_t *cache, message_header_t *header, uint8_t *key,
    uint8_t *data) {
  switch(header->message_type) {
    case MESSAGE_TYPE_SET:
      cache_set_item(cache, key, header->key_size, data, header->data_size);
      break;
    case MESSAGE_TYPE_UNSET:
      cache_unset_item(cache, key, header->key_size);
      break;
  }
}
