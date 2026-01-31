#include "poly.h"
#include "params.h"
#include "ntt.h"
#include "reduce.h"
#include "rounding.h"
#include <stdint.h>

void poly_reduce(poly *a) {
    for(unsigned int i = 0; i < N; ++i)
        a->coeffs[i] = reduce32(a->coeffs[i]);
}

void poly_ntt(poly *a) {
    ntt(a->coeffs);
}

void poly_invntt_tomont(poly *a) {
    invntt_tomont(a->coeffs);
}

void poly_add(poly *c, const poly *a, const poly *b) {
    for(unsigned int i = 0; i < N; ++i)
        c->coeffs[i] = a->coeffs[i] + b->coeffs[i];
}

void poly_sub(poly *c, const poly *a, const poly *b) {
    for(unsigned int i = 0; i < N; ++i)
        c->coeffs[i] = a->coeffs[i] - b->coeffs[i];
}

void poly_shiftl(poly *a) {
    for(unsigned int i = 0; i < N; ++i)
        a->coeffs[i] <<= D;
}

void poly_pointwise_montgomery(poly *c, const poly *a, const poly *b) {
    for(unsigned int i = 0; i < N; ++i)
        c->coeffs[i] = montgomery_reduce((int64_t)a->coeffs[i] * b->coeffs[i]);
}

void poly_power2round(poly *a1, poly *a0, const poly *a) {
    for(unsigned int i = 0; i < N; ++i)
        a1->coeffs[i] = power2round(&a0->coeffs[i], a->coeffs[i]);
}

void poly_decompose(poly *a1, poly *a0, const poly *a) {
    for(unsigned int i = 0; i < N; ++i)
        a1->coeffs[i] = decompose(&a0->coeffs[i], a->coeffs[i]);
}

unsigned int poly_make_hint(poly *h, const poly *a0, const poly *a1) {
    unsigned int s = 0;
    for(unsigned int i = 0; i < N; ++i) {
        h->coeffs[i] = make_hint(a0->coeffs[i], a1->coeffs[i]);
        s += h->coeffs[i];
    }
    return s;
}

void poly_use_hint(poly *b, const poly *a, const poly *h) {
    for(unsigned int i = 0; i < N; ++i)
        b->coeffs[i] = use_hint(a->coeffs[i], h->coeffs[i]);
}

int poly_chknorm(const poly *a, int32_t B) {
    int32_t t;
    
    for(unsigned int i = 0; i < N; ++i) {
        t = a->coeffs[i] >> 31;
        t = a->coeffs[i] - (t & 2*a->coeffs[i]);
        
        if(t >= B)
            return 1;
    }
    return 0;
}

void polyt1_pack(uint8_t *r, const poly *a) {
    for(unsigned int i = 0; i < N/4; ++i) {
        r[5*i+0] = (a->coeffs[4*i+0] >> 0);
        r[5*i+1] = (a->coeffs[4*i+0] >> 8) | (a->coeffs[4*i+1] << 2);
        r[5*i+2] = (a->coeffs[4*i+1] >> 6) | (a->coeffs[4*i+2] << 4);
        r[5*i+3] = (a->coeffs[4*i+2] >> 4) | (a->coeffs[4*i+3] << 6);
        r[5*i+4] = (a->coeffs[4*i+3] >> 2);
    }
}

void polyt1_unpack(poly *r, const uint8_t *a) {
    for(unsigned int i = 0; i < N/4; ++i) {
        r->coeffs[4*i+0] = ((a[5*i+0] >> 0) | ((uint32_t)a[5*i+1] << 8)) & 0x3FF;
        r->coeffs[4*i+1] = ((a[5*i+1] >> 2) | ((uint32_t)a[5*i+2] << 6)) & 0x3FF;
        r->coeffs[4*i+2] = ((a[5*i+2] >> 4) | ((uint32_t)a[5*i+3] << 4)) & 0x3FF;
        r->coeffs[4*i+3] = ((a[5*i+3] >> 6) | ((uint32_t)a[5*i+4] << 2)) & 0x3FF;
    }
}

void polyt0_pack(uint8_t *r, const poly *a) {
    uint32_t t[8];
    for(unsigned int i = 0; i < N/8; ++i) {
        t[0] = (1 << (D-1)) - a->coeffs[8*i+0];
        t[1] = (1 << (D-1)) - a->coeffs[8*i+1];
        t[2] = (1 << (D-1)) - a->coeffs[8*i+2];
        t[3] = (1 << (D-1)) - a->coeffs[8*i+3];
        t[4] = (1 << (D-1)) - a->coeffs[8*i+4];
        t[5] = (1 << (D-1)) - a->coeffs[8*i+5];
        t[6] = (1 << (D-1)) - a->coeffs[8*i+6];
        t[7] = (1 << (D-1)) - a->coeffs[8*i+7];
        
        r[13*i+ 0] = t[0];
        r[13*i+ 1] = t[0] >> 8;
        r[13*i+ 1] |= t[1] << 5;
        r[13*i+ 2] = t[1] >> 3;
        r[13*i+ 3] = t[1] >> 11;
        r[13*i+ 3] |= t[2] << 2;
        r[13*i+ 4] = t[2] >> 6;
        r[13*i+ 4] |= t[3] << 7;
        r[13*i+ 5] = t[3] >> 1;
        r[13*i+ 6] = t[3] >> 9;
        r[13*i+ 6] |= t[4] << 4;
        r[13*i+ 7] = t[4] >> 4;
        r[13*i+ 8] = t[4] >> 12;
        r[13*i+ 8] |= t[5] << 1;
        r[13*i+ 9] = t[5] >> 7;
        r[13*i+ 9] |= t[6] << 6;
        r[13*i+10] = t[6] >> 2;
        r[13*i+11] = t[6] >> 10;
        r[13*i+11] |= t[7] << 3;
        r[13*i+12] = t[7] >> 5;
    }
}

void polyt0_unpack(poly *r, const uint8_t *a) {
    for(unsigned int i = 0; i < N/8; ++i) {
        r->coeffs[8*i+0] = a[13*i+0];
        r->coeffs[8*i+0] |= (uint32_t)a[13*i+1] << 8;
        r->coeffs[8*i+0] &= 0x1FFF;
        
        r->coeffs[8*i+1] = a[13*i+1] >> 5;
        r->coeffs[8*i+1] |= (uint32_t)a[13*i+2] << 3;
        r->coeffs[8*i+1] |= (uint32_t)a[13*i+3] << 11;
        r->coeffs[8*i+1] &= 0x1FFF;
        
        r->coeffs[8*i+2] = a[13*i+3] >> 2;
        r->coeffs[8*i+2] |= (uint32_t)a[13*i+4] << 6;
        r->coeffs[8*i+2] &= 0x1FFF;
        
        r->coeffs[8*i+3] = a[13*i+4] >> 7;
        r->coeffs[8*i+3] |= (uint32_t)a[13*i+5] << 1;
        r->coeffs[8*i+3] |= (uint32_t)a[13*i+6] << 9;
        r->coeffs[8*i+3] &= 0x1FFF;
        
        r->coeffs[8*i+4] = a[13*i+6] >> 4;
        r->coeffs[8*i+4] |= (uint32_t)a[13*i+7] << 4;
        r->coeffs[8*i+4] |= (uint32_t)a[13*i+8] << 12;
        r->coeffs[8*i+4] &= 0x1FFF;
        
        r->coeffs[8*i+5] = a[13*i+8] >> 1;
        r->coeffs[8*i+5] |= (uint32_t)a[13*i+9] << 7;
        r->coeffs[8*i+5] &= 0x1FFF;
        
        r->coeffs[8*i+6] = a[13*i+9] >> 6;
        r->coeffs[8*i+6] |= (uint32_t)a[13*i+10] << 2;
        r->coeffs[8*i+6] |= (uint32_t)a[13*i+11] << 10;
        r->coeffs[8*i+6] &= 0x1FFF;
        
        r->coeffs[8*i+7] = a[13*i+11] >> 3;
        r->coeffs[8*i+7] |= (uint32_t)a[13*i+12] << 5;
        r->coeffs[8*i+7] &= 0x1FFF;
        
        r->coeffs[8*i+0] = (1 << (D-1)) - r->coeffs[8*i+0];
        r->coeffs[8*i+1] = (1 << (D-1)) - r->coeffs[8*i+1];
        r->coeffs[8*i+2] = (1 << (D-1)) - r->coeffs[8*i+2];
        r->coeffs[8*i+3] = (1 << (D-1)) - r->coeffs[8*i+3];
        r->coeffs[8*i+4] = (1 << (D-1)) - r->coeffs[8*i+4];
        r->coeffs[8*i+5] = (1 << (D-1)) - r->coeffs[8*i+5];
        r->coeffs[8*i+6] = (1 << (D-1)) - r->coeffs[8*i+6];
        r->coeffs[8*i+7] = (1 << (D-1)) - r->coeffs[8*i+7];
    }
}

void polyeta_pack(uint8_t *r, const poly *a) {
    uint8_t t[8];
    for(unsigned int i = 0; i < N/8; ++i) {
        t[0] = ETA - a->coeffs[8*i+0];
        t[1] = ETA - a->coeffs[8*i+1];
        t[2] = ETA - a->coeffs[8*i+2];
        t[3] = ETA - a->coeffs[8*i+3];
        t[4] = ETA - a->coeffs[8*i+4];
        t[5] = ETA - a->coeffs[8*i+5];
        t[6] = ETA - a->coeffs[8*i+6];
        t[7] = ETA - a->coeffs[8*i+7];
        
        r[3*i+0] = (t[0] >> 0) | (t[1] << 3) | (t[2] << 6);
        r[3*i+1] = (t[2] >> 2) | (t[3] << 1) | (t[4] << 4) | (t[5] << 7);
        r[3*i+2] = (t[5] >> 1) | (t[6] << 2) | (t[7] << 5);
    }
}

void polyeta_unpack(poly *r, const uint8_t *a) {
    for(unsigned int i = 0; i < N/8; ++i) {
        r->coeffs[8*i+0] = (a[3*i+0] >> 0) & 7;
        r->coeffs[8*i+1] = (a[3*i+0] >> 3) & 7;
        r->coeffs[8*i+2] = ((a[3*i+0] >> 6) | (a[3*i+1] << 2)) & 7;
        r->coeffs[8*i+3] = (a[3*i+1] >> 1) & 7;
        r->coeffs[8*i+4] = (a[3*i+1] >> 4) & 7;
        r->coeffs[8*i+5] = ((a[3*i+1] >> 7) | (a[3*i+2] << 1)) & 7;
        r->coeffs[8*i+6] = (a[3*i+2] >> 2) & 7;
        r->coeffs[8*i+7] = (a[3*i+2] >> 5) & 7;
        
        r->coeffs[8*i+0] = ETA - r->coeffs[8*i+0];
        r->coeffs[8*i+1] = ETA - r->coeffs[8*i+1];
        r->coeffs[8*i+2] = ETA - r->coeffs[8*i+2];
        r->coeffs[8*i+3] = ETA - r->coeffs[8*i+3];
        r->coeffs[8*i+4] = ETA - r->coeffs[8*i+4];
        r->coeffs[8*i+5] = ETA - r->coeffs[8*i+5];
        r->coeffs[8*i+6] = ETA - r->coeffs[8*i+6];
        r->coeffs[8*i+7] = ETA - r->coeffs[8*i+7];
    }
}

void polyz_pack(uint8_t *r, const poly *a) {
    uint32_t t[4];
    for(unsigned int i = 0; i < N/4; ++i) {
        t[0] = GAMMA1 - a->coeffs[4*i+0];
        t[1] = GAMMA1 - a->coeffs[4*i+1];
        t[2] = GAMMA1 - a->coeffs[4*i+2];
        t[3] = GAMMA1 - a->coeffs[4*i+3];
        
        r[9*i+0] = t[0];
        r[9*i+1] = t[0] >> 8;
        r[9*i+2] = t[0] >> 16;
        r[9*i+2] |= t[1] << 2;
        r[9*i+3] = t[1] >> 6;
        r[9*i+4] = t[1] >> 14;
        r[9*i+4] |= t[2] << 4;
        r[9*i+5] = t[2] >> 4;
        r[9*i+6] = t[2] >> 12;
        r[9*i+6] |= t[3] << 6;
        r[9*i+7] = t[3] >> 2;
        r[9*i+8] = t[3] >> 10;
    }
}

void polyz_unpack(poly *r, const uint8_t *a) {
    for(unsigned int i = 0; i < N/4; ++i) {
        r->coeffs[4*i+0] = a[9*i+0];
        r->coeffs[4*i+0] |= (uint32_t)a[9*i+1] << 8;
        r->coeffs[4*i+0] |= (uint32_t)a[9*i+2] << 16;
        r->coeffs[4*i+0] &= 0x3FFFF;
        
        r->coeffs[4*i+1] = a[9*i+2] >> 2;
        r->coeffs[4*i+1] |= (uint32_t)a[9*i+3] << 6;
        r->coeffs[4*i+1] |= (uint32_t)a[9*i+4] << 14;
        r->coeffs[4*i+1] &= 0x3FFFF;
        
        r->coeffs[4*i+2] = a[9*i+4] >> 4;
        r->coeffs[4*i+2] |= (uint32_t)a[9*i+5] << 4;
        r->coeffs[4*i+2] |= (uint32_t)a[9*i+6] << 12;
        r->coeffs[4*i+2] &= 0x3FFFF;
        
        r->coeffs[4*i+3] = a[9*i+6] >> 6;
        r->coeffs[4*i+3] |= (uint32_t)a[9*i+7] << 2;
        r->coeffs[4*i+3] |= (uint32_t)a[9*i+8] << 10;
        r->coeffs[4*i+3] &= 0x3FFFF;
        
        r->coeffs[4*i+0] = GAMMA1 - r->coeffs[4*i+0];
        r->coeffs[4*i+1] = GAMMA1 - r->coeffs[4*i+1];
        r->coeffs[4*i+2] = GAMMA1 - r->coeffs[4*i+2];
        r->coeffs[4*i+3] = GAMMA1 - r->coeffs[4*i+3];
    }
}