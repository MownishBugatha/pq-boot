#ifndef CRYPTO_H
#define CRYPTO_H

#include "types.h"

// SHA3-256 constants
#define SHA3_256_DIGEST_SIZE 32
#define SHA3_256_BLOCK_SIZE 136
extern const uint8_t _binary_keys_dilithium2_pub_start[];
extern const uint8_t _binary_keys_dilithium2_pub_end[];
extern const uint8_t _binary_keys_dilithium2_pub_size[];

// SHA3-256 context
typedef struct {
    uint64_t state[25];
    uint8_t buffer[SHA3_256_BLOCK_SIZE];
    size_t buf_len;
} sha3_ctx_t;

// SHA3 functions
void sha3_256_init(sha3_ctx_t *ctx);
void sha3_256_update(sha3_ctx_t *ctx, const uint8_t *data, size_t len);
void sha3_256_final(sha3_ctx_t *ctx, uint8_t *hash);
void sha3_256(const uint8_t *data, size_t len, uint8_t *hash);

// Dilithium2 verification
int dilithium2_verify(const uint8_t *sig, size_t siglen,
                      const uint8_t *msg, size_t msglen,
                      const uint8_t *pk);

// Public key (embedded in .rodata)
extern const uint8_t __pubkey_start[];
extern const uint8_t __pubkey_end[];

#define DILITHIUM2_PUBLICKEYBYTES 1312
#define DILITHIUM2_SIGNBYTES 2420

#endif