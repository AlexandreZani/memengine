#include "message.h"

#include "unittest.h"
#include <stdlib.h>
#include <arpa/inet.h>
#include "cache.h"
#include "cache_entry.h"

void
test_header_size() {
  assert_equals(sizeof(message_header_t),
      sizeof(uint8_t) +  // protocol_version
      sizeof(uint8_t) +  // message_type
      sizeof(uint32_t) + // key_size
      sizeof(uint32_t)   // data_size
      );

  uint8_t protocol_version = 0x12;
  uint8_t message_type = 0x23;
  uint32_t key_size = 0x4567890a;
  uint32_t data_size = 0xbcdef012;

  uint8_t *header_buf = malloc(sizeof(message_header_t));
  message_header_t *header = (message_header_t *)header_buf;
  *(uint8_t *)header_buf = protocol_version;
  header_buf += sizeof(uint8_t);
  *(uint8_t *)header_buf = message_type;
  header_buf += sizeof(uint8_t);
  *(uint32_t *)header_buf = key_size;
  header_buf += sizeof(uint32_t);
  *(uint32_t *)header_buf = data_size;

  assert_equals(protocol_version, header->protocol_version);
  assert_equals(message_type, header->message_type);
  assert_equals(key_size, header->key_size);
  assert_equals(data_size, header->data_size);
  free(header);
}

void
test_message_header_ntoh() {
  uint8_t protocol_version = 0x12;
  uint8_t message_type = 0x23;
  uint32_t key_size = 0x4567890a;
  uint32_t data_size = 0xbcdef012;

  uint8_t *header_buf = malloc(sizeof(message_header_t));
  message_header_t *header = (message_header_t *)header_buf;
  *(uint8_t *)header_buf = protocol_version;
  header_buf += sizeof(uint8_t);
  *(uint8_t *)header_buf = message_type;
  header_buf += sizeof(uint8_t);
  *(uint32_t *)header_buf = htonl(key_size);
  header_buf += sizeof(uint32_t);
  *(uint32_t *)header_buf = htonl(data_size);

  message_header_ntoh(header);

  assert_equals(protocol_version, header->protocol_version);
  assert_equals(message_type, header->message_type);
  assert_equals(key_size, header->key_size);
  assert_equals(data_size, header->data_size);
  free(header);
}

void
test_dispatch_message_set() {
  cache_t *cache = create_cache(1024, 10, 128);
  char *key = "some key";
  char *data = "some data";
  message_header_t *header = malloc(sizeof(message_header_t));
  header->protocol_version = PROTOCOL_VERSION;
  header->message_type = MESSAGE_TYPE_SET;
  header->key_size = 8;
  header->data_size = 9;

  response_message_t *response;
  response = dispatch_message(cache, header, (uint8_t *)key, (uint8_t *)data);

  assert_is_not_null(response);
  assert_equals(sizeof(uint8_t), response->data_size);
  assert_equals(RESPONSE_SUCCESS, *(response->data));

  cache_entry_t *cache_entry = cache_get_item(cache, (uint8_t *)key,
      header->key_size);
  assert_is_not_null(cache_entry);
  assert_equals_buf(key, cache_entry->key, header->key_size);
  assert_equals_buf(data, cache_entry->data, header->data_size);


  free(header);
  destroy_cache(cache);
}

void
test_dispatch_message_unset() {
  cache_t *cache = create_cache(1024, 10, 128);
  char *key = "some key";
  char *data = "some data";

  cache_set_item(cache, (uint8_t *)key, 8, (uint8_t *)data, 9);

  message_header_t *header = malloc(sizeof(message_header_t));
  header->protocol_version = PROTOCOL_VERSION;
  header->message_type = MESSAGE_TYPE_UNSET;
  header->key_size = 8;
  header->data_size = 0;

  response_message_t *response;
  response = dispatch_message(cache, header, (uint8_t *)key, (uint8_t *)data);

  assert_equals(sizeof(uint8_t), response->data_size);
  assert_equals(RESPONSE_SUCCESS, *(response->data));

  cache_entry_t *cache_entry = cache_get_item(cache, (uint8_t *)key,
      header->key_size);
  assert_is_null(cache_entry);

  free(header);
  destroy_cache(cache);
}

void
test_dispatch_message_get() {
  cache_t *cache = create_cache(1024, 10, 128);
  char *key = "some key";
  char *data = "some data";

  cache_set_item(cache, (uint8_t *)key, 8, (uint8_t *)data, 9);

  message_header_t *header = malloc(sizeof(message_header_t));
  header->protocol_version = PROTOCOL_VERSION;
  header->message_type = MESSAGE_TYPE_GET;
  header->key_size = 8;
  header->data_size = 0;

  response_message_t *response;
  response = dispatch_message(cache, header, (uint8_t *)key, (uint8_t *)data);

  assert_is_not_null(response);
  assert_equals(9, response->data_size);
  assert_equals_buf(data, response->data, response->data_size);

  free(response);
  free(header);
  destroy_cache(cache);
}

void
test_dispatch_message_get_null() {
  cache_t *cache = create_cache(1024, 10, 128);
  char *key = "some key";
  char *data = "some data";

  message_header_t *header = malloc(sizeof(message_header_t));
  header->protocol_version = PROTOCOL_VERSION;
  header->message_type = MESSAGE_TYPE_GET;
  header->key_size = 8;
  header->data_size = 0;

  response_message_t *response;
  response = dispatch_message(cache, header, (uint8_t *)key, (uint8_t *)data);

  assert_is_not_null(response);
  assert_equals(0, response->data_size);
  assert_is_null(response->data);

  free(response);
  free(header);
  destroy_cache(cache);
}

int
main(int argc, char **argv) {
  test_header_size();
  test_message_header_ntoh();
  test_dispatch_message_set();
  test_dispatch_message_unset();
  test_dispatch_message_get();
  test_dispatch_message_get_null();
  return 0;
}
