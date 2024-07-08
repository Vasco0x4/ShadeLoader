#include "encryption.h"

void xor_encrypt_decrypt(unsigned char* data, size_t data_len, const unsigned char* key, size_t key_len) {
    for (size_t i = 0; i < data_len; ++i) {
        data[i] ^= key[i % key_len];
    }
}
