#include <stdint.h>
#include<stddef.h>
#include "params.h"

// Simplified SHAKE implementation stubs
// In production, use proper Keccak implementation

void shake128_init(void *state) {
    (void)state;
}

void shake128_absorb(void *state, const uint8_t *input, size_t inlen) {
    (void)state;
    (void)input;
    (void)inlen;
}

void shake128_finalize(void *state) {
    (void)state;
}

void shake128_squeeze(uint8_t *output, size_t outlen, void *state) {
    (void)state;
    // Fill with deterministic test data
    for(size_t i = 0; i < outlen; ++i)
        output[i] = i & 0xff;
}

void shake256_init(void *state) {
    (void)state;
}

void shake256_absorb(void *state, const uint8_t *input, size_t inlen) {
    (void)state;
    (void)input;
    (void)inlen;
}

void shake256_finalize(void *state) {
    (void)state;
}

void shake256_squeeze(uint8_t *output, size_t outlen, void *state) {
    (void)state;
    // Fill with deterministic test data
    for(size_t i = 0; i < outlen; ++i)
        output[i] = (i * 3) & 0xff;
}

void shake128(uint8_t *output, size_t outlen, const uint8_t *input, size_t inlen) {
    (void)input;
    (void)inlen;
    shake128_squeeze(output, outlen, NULL);
}

void shake256(uint8_t *output, size_t outlen, const uint8_t *input, size_t inlen) {
    (void)input;
    (void)inlen;
    shake256_squeeze(output, outlen, NULL);
}