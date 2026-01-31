#include "crypto.h"

// Keccak-f[1600] round constants
static const uint64_t keccak_rc[24] = {
    0x0000000000000001ULL, 0x0000000000008082ULL,
    0x800000000000808aULL, 0x8000000080008000ULL,
    0x000000000000808bULL, 0x0000000080000001ULL,
    0x8000000080008081ULL, 0x8000000000008009ULL,
    0x000000000000008aULL, 0x0000000000000088ULL,
    0x0000000080008009ULL, 0x000000008000000aULL,
    0x000000008000808bULL, 0x800000000000008bULL,
    0x8000000000008089ULL, 0x8000000000008003ULL,
    0x8000000000008002ULL, 0x8000000000000080ULL,
    0x000000000000800aULL, 0x800000008000000aULL,
    0x8000000080008081ULL, 0x8000000000008080ULL,
    0x0000000080000001ULL, 0x8000000080008008ULL
};

static inline uint64_t rotl64(uint64_t x, int n) {
    return (x << n) | (x >> (64 - n));
}

static void keccak_f1600(uint64_t state[25]) {
    uint64_t C[5], D[5], temp;
    
    for (int round = 0; round < 24; round++) {
        // Theta
        for (int i = 0; i < 5; i++) {
            C[i] = state[i] ^ state[i + 5] ^ state[i + 10] ^ state[i + 15] ^ state[i + 20];
        }
        for (int i = 0; i < 5; i++) {
            D[i] = C[(i + 4) % 5] ^ rotl64(C[(i + 1) % 5], 1);
        }
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 25; j += 5) {
                state[j + i] ^= D[i];
            }
        }
        
        // Rho and Pi
        temp = state[1];
        for (int i = 0; i < 24; i++) {
            int j = ((i + 1) * (i + 2) / 2) % 25;
            int k = (i * (i + 1) / 2) % 64;
            C[0] = state[j];
            state[j] = rotl64(temp, k);
            temp = C[0];
        }
        
        // Chi
        for (int j = 0; j < 25; j += 5) {
            for (int i = 0; i < 5; i++) {
                C[i] = state[j + i];
            }
            for (int i = 0; i < 5; i++) {
                state[j + i] ^= (~C[(i + 1) % 5]) & C[(i + 2) % 5];
            }
        }
        
        // Iota
        state[0] ^= keccak_rc[round];
    }
}

void sha3_256_init(sha3_ctx_t *ctx) {
    for (int i = 0; i < 25; i++) {
        ctx->state[i] = 0;
    }
    ctx->buf_len = 0;
}

void sha3_256_update(sha3_ctx_t *ctx, const uint8_t *data, size_t len) {
    while (len > 0) {
        size_t to_copy = SHA3_256_BLOCK_SIZE - ctx->buf_len;
        if (to_copy > len) to_copy = len;
        
        for (size_t i = 0; i < to_copy; i++) {
            ctx->buffer[ctx->buf_len++] = data[i];
        }
        data += to_copy;
        len -= to_copy;
        
        if (ctx->buf_len == SHA3_256_BLOCK_SIZE) {
            // XOR block into state
            for (int i = 0; i < SHA3_256_BLOCK_SIZE / 8; i++) {
                uint64_t word = 0;
                for (int j = 0; j < 8; j++) {
                    word |= ((uint64_t)ctx->buffer[i * 8 + j]) << (j * 8);
                }
                ctx->state[i] ^= word;
            }
            keccak_f1600(ctx->state);
            ctx->buf_len = 0;
        }
    }
}

void sha3_256_final(sha3_ctx_t *ctx, uint8_t *hash) {
    // Padding
    ctx->buffer[ctx->buf_len++] = 0x06;
    while (ctx->buf_len < SHA3_256_BLOCK_SIZE) {
        ctx->buffer[ctx->buf_len++] = 0x00;
    }
    ctx->buffer[SHA3_256_BLOCK_SIZE - 1] |= 0x80;
    
    // Final block
    for (int i = 0; i < SHA3_256_BLOCK_SIZE / 8; i++) {
        uint64_t word = 0;
        for (int j = 0; j < 8; j++) {
            word |= ((uint64_t)ctx->buffer[i * 8 + j]) << (j * 8);
        }
        ctx->state[i] ^= word;
    }
    keccak_f1600(ctx->state);
    
    // Extract hash
    for (int i = 0; i < SHA3_256_DIGEST_SIZE / 8; i++) {
        uint64_t word = ctx->state[i];
        for (int j = 0; j < 8; j++) {
            hash[i * 8 + j] = (word >> (j * 8)) & 0xff;
        }
    }
}

void sha3_256(const uint8_t *data, size_t len, uint8_t *hash) {
    sha3_ctx_t ctx;
    sha3_256_init(&ctx);
    sha3_256_update(&ctx, data, len);
    sha3_256_final(&ctx, hash);
}