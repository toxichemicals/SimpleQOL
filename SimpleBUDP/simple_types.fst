#ifndef SIMPLE_TYPES_H
#define SIMPLE_TYPES_H

#include <stdint.h>
#include <string.h>
#include <stdio.h>

// --- Base Types ---
typedef uint8_t u8;   typedef uint16_t u16; typedef uint32_t u32; typedef uint64_t u64;
typedef int8_t s8;    typedef int16_t s16;  typedef int32_t s32;  typedef int64_t s64;

// --- Structural Definitions ---
#define DEF_STRUCTS(BITS, LIMBS) \
    typedef struct { u32 limbs[LIMBS]; } suint##BITS; \
    typedef struct { s32 limbs[LIMBS]; } sint##BITS; \
    typedef struct { suint##BITS mantissa; s32 exponent; } sfloat##BITS;

DEF_STRUCTS(32, 1)    DEF_STRUCTS(64, 2)    DEF_STRUCTS(128, 4)
DEF_STRUCTS(256, 8)   DEF_STRUCTS(512, 16)  DEF_STRUCTS(1024, 32)
DEF_STRUCTS(2048, 64) DEF_STRUCTS(4096, 128) DEF_STRUCTS(8192, 256)
DEF_STRUCTS(12288, 384)

// --- Internal Helpers ---
static inline void _internal_raw_hex(u32* limbs, int count) {
    printf("0x");
    int started = 0;
    for (int i = count - 1; i >= 0; i--) {
        if (limbs[i] != 0 || started || i == 0) {
            if (started) printf("%08X", limbs[i]);
            else printf("%X", limbs[i]);
            started = 1;
        }
    }
}

static inline void _internal_dec_ascii(u32* limbs, int count) {
    u32 temp[count];
    memcpy(temp, limbs, count * sizeof(u32));
    char digits[5000]; 
    int pos = 0, digit_count = 0;
    while (1) {
        u64 remainder = 0;
        int all_zero = 1;
        for (int i = count - 1; i >= 0; i--) {
            u64 val = temp[i] + (remainder << 32);
            temp[i] = (u32)(val / 10);
            remainder = val % 10;
            if (temp[i] > 0) all_zero = 0;
        }
        if (digit_count > 0 && digit_count % 3 == 0) digits[pos++] = ',';
        digits[pos++] = (char)('0' + remainder);
        digit_count++;
        if (all_zero) break;
    }
    for (int i = pos - 1; i >= 0; i--) putchar(digits[i]);
}

// --- Shift Operations ---
#define DEF_SHIFT(BITS, COUNT) \
    static inline void suint##BITS##_shl(suint##BITS *v, int shift) { \
        if (shift <= 0) return; \
        int limb_shift = shift / 32; \
        int bit_shift = shift % 32; \
        if (limb_shift >= COUNT) { memset(v->limbs, 0, sizeof(v->limbs)); return; } \
        for (int i = COUNT - 1; i >= 0; i--) { \
            if (i >= limb_shift) { \
                v->limbs[i] = v->limbs[i - limb_shift] << bit_shift; \
                if (bit_shift > 0 && i - limb_shift - 1 >= 0) \
                    v->limbs[i] |= v->limbs[i - limb_shift - 1] >> (32 - bit_shift); \
            } else v->limbs[i] = 0; \
        } \
    }

DEF_SHIFT(32, 1)    DEF_SHIFT(64, 2)    DEF_SHIFT(128, 4)
DEF_SHIFT(256, 8)   DEF_SHIFT(512, 16)  DEF_SHIFT(1024, 32)
DEF_SHIFT(2048, 64) DEF_SHIFT(4096, 128) DEF_SHIFT(8192, 256)
DEF_SHIFT(12288, 384)

// --- Printers ---
#define DEF_PRINTERS(BITS, COUNT) \
    static inline void _nf_u##BITS(suint##BITS v) { _internal_raw_hex(v.limbs, COUNT); } \
    static inline void _f_u##BITS(suint##BITS v)  { _internal_dec_ascii(v.limbs, COUNT); } \
    static inline void _nf_s##BITS(sint##BITS v)  { printf("(S)"); _internal_raw_hex((u32*)v.limbs, COUNT); } \
    static inline void _f_s##BITS(sint##BITS v)   { if(v.limbs[COUNT-1] < 0) printf("-"); _internal_dec_ascii((u32*)v.limbs, COUNT); } \
    static inline void _nf_flt##BITS(sfloat##BITS v) { _internal_raw_hex(v.mantissa.limbs, COUNT); printf(" E%d", v.exponent); } \
    static inline void _f_flt##BITS(sfloat##BITS v)  { _internal_dec_ascii(v.mantissa.limbs, COUNT); printf(" * 2^%d", v.exponent); }

DEF_PRINTERS(32, 1)    DEF_PRINTERS(64, 2)    DEF_PRINTERS(128, 4)
DEF_PRINTERS(256, 8)   DEF_PRINTERS(512, 16)  DEF_PRINTERS(1024, 32)
DEF_PRINTERS(2048, 64) DEF_PRINTERS(4096, 128) DEF_PRINTERS(8192, 256)
DEF_PRINTERS(12288, 384)

#define slibnfprint(val) _Generic((val), \
    suint32: _nf_u32, suint64: _nf_u64, suint128: _nf_u128, suint256: _nf_u256, suint512: _nf_u512, \
    suint1024: _nf_u1024, suint2048: _nf_u2048, suint4096: _nf_u4096, suint8192: _nf_u8192, suint12288: _nf_u12288, \
    sint32: _nf_s32, sint64: _nf_s64, sint128: _nf_s128, sint256: _nf_s256, sint512: _nf_s512, \
    sint1024: _nf_s1024, sint2048: _nf_s2048, sint4096: _nf_s4096, sint8192: _nf_s8192, sint12288: _nf_s12288, \
    sfloat32: _nf_flt32, sfloat64: _nf_flt64, sfloat128: _nf_flt128, sfloat256: _nf_flt256, sfloat512: _nf_flt512, \
    sfloat1024: _nf_flt1024, sfloat2048: _nf_flt2048, sfloat4096: _nf_flt4096, sfloat8192: _nf_flt8192, sfloat12288: _nf_flt12288 \
)(val)

#define slibprint(val) _Generic((val), \
    suint32: _f_u32, suint64: _f_u64, suint128: _f_u128, suint256: _f_u256, suint512: _f_u512, \
    suint1024: _f_u1024, suint2048: _f_u2048, suint4096: _f_u4096, suint8192: _f_u8192, suint12288: _f_u12288, \
    sint32: _f_s32, sint64: _f_s64, sint128: _f_s128, sint256: _f_s256, sint512: _f_s512, \
    sint1024: _f_s1024, sint2048: _f_s2048, sint4096: _f_s4096, sint8192: _f_s8192, sint12288: _f_s12288, \
    sfloat32: _f_flt32, sfloat64: _f_flt64, sfloat128: _f_flt128, sfloat256: _f_flt256, sfloat512: _f_flt512, \
    sfloat1024: _f_flt1024, sfloat2048: _f_flt2048, sfloat4096: _f_flt4096, sfloat8192: _f_flt8192, sfloat12288: _f_flt12288 \
)(val)
// --- Addition ---
#define DEF_ADD(BITS, COUNT) \
    static inline u32 suint##BITS##_add(suint##BITS *res, suint##BITS a, suint##BITS b) { \
        u64 carry = 0; \
        for (int i = 0; i < COUNT; i++) { \
            u64 sum = (u64)a.limbs[i] + b.limbs[i] + carry; \
            res->limbs[i] = (u32)sum; \
            carry = sum >> 32; \
        } \
        return (u32)carry; \
    }

// --- Multiplication ---
#define DEF_MUL(BITS, COUNT) \
    static inline void suint##BITS##_mul(suint##BITS *res, suint##BITS a, suint##BITS b) { \
        suint##BITS out = {0}; \
        for (int i = 0; i < COUNT; i++) { \
            if (a.limbs[i] == 0) continue; \
            u64 carry = 0; \
            for (int j = 0; i + j < COUNT; j++) { \
                u64 cur = out.limbs[i + j] + (u64)a.limbs[i] * b.limbs[j] + carry; \
                out.limbs[i + j] = (u32)cur; \
                carry = cur >> 32; \
            } \
        } \
        *res = out; \
    }

DEF_ADD(12288, 384)
DEF_MUL(12288, 384)
// Power function: res = base ^ exp
static inline void suint12288_pow(suint12288 *res, u32 base_val, u32 exp) {
    suint12288 b = {0}; b.limbs[0] = base_val;
    suint12288 result = {0}; result.limbs[0] = 1;
    
    for (u32 i = 0; i < exp; i++) {
        suint12288_mul(&result, result, b);
    }
    *res = result;
}

// Tetration: base ^^ height
static inline void suint12288_tetrate(suint12288 *res, u32 base, u32 height) {
    if (height == 0) { res->limbs[0] = 1; return; }
    if (height == 1) { res->limbs[0] = base; return; }
    
    // Starting with base^^2
    suint12288 current_val;
    suint12288_pow(&current_val, base, base);
    
    for (u32 i = 2; i < height; i++) {
        // This is the "Horrid" part. We use the previous result 
        // as the exponent for the next layer.
        // NOTE: We only take the first limb as the exponent here 
        // because suint12288_pow expects a u32 exponent.
        suint12288_pow(&current_val, base, current_val.limbs[0]);
    }
    *res = current_val;
}
#endif
