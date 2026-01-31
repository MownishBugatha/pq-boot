#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define DILITHIUM2_PUBLICKEYBYTES 1312
#define DILITHIUM2_SECRETKEYBYTES 2528

int main() {
    FILE *fp_pub, *fp_priv;
    uint8_t pk[DILITHIUM2_PUBLICKEYBYTES];
    uint8_t sk[DILITHIUM2_SECRETKEYBYTES];

    printf("Generating Dilithium2 test keypair...\n");

    /* Deterministic dummy public key */
    for (int i = 0; i < DILITHIUM2_PUBLICKEYBYTES; i++) {
        pk[i] = (uint8_t)(i & 0xFF);
    }

    /* Deterministic dummy secret key */
    for (int i = 0; i < DILITHIUM2_SECRETKEYBYTES; i++) {
        sk[i] = (uint8_t)((i * 7) & 0xFF);
    }

    /* Ensure keys directory exists */
    system("mkdir -p keys");

    /* Write public key */
    fp_pub = fopen("keys/dilithium2.pub", "wb");
    if (!fp_pub) {
        perror("Failed to create dilithium2.pub");
        return 1;
    }
    fwrite(pk, 1, DILITHIUM2_PUBLICKEYBYTES, fp_pub);
    fclose(fp_pub);

    printf("Public key written: keys/dilithium2.pub (%d bytes)\n",
           DILITHIUM2_PUBLICKEYBYTES);

    /* Write private key */
    fp_priv = fopen("keys/dilithium2.priv", "wb");
    if (!fp_priv) {
        perror("Failed to create dilithium2.priv");
        return 1;
    }
    fwrite(sk, 1, DILITHIUM2_SECRETKEYBYTES, fp_priv);
    fclose(fp_priv);

    printf("Private key written: keys/dilithium2.priv (%d bytes)\n",
           DILITHIUM2_SECRETKEYBYTES);

    printf("Dilithium2 test keypair generated successfully.\n");
    return 0;
}
