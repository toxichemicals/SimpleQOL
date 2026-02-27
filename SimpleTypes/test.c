#include "simple_types.h"

int main() {
    printf("================================\n");
    printf("   TESTS 32-BIT TO 12288-BIT    \n");
    printf("================================\n\n");

    // --- The 12288-bit Stress Test ---
    printf("[12288-bit Stress Test: 2^12000]\n");
    suint12288 big_val; 
    memset(&big_val, 0, sizeof(big_val));
    big_val.limbs[0] = 1;
    suint12288_shl(&big_val, 12000);
    printf("DEC: "); slibprint(big_val); printf("\n");
    printf("HEX: "); slibnfprint(big_val); printf("\n\n");

    // --- All Tiers ---
    printf("[32-bit]\n");
    suint32 u32_v = {0}; u32_v.limbs[0] = 0xABCDEF12;
    slibprint(u32_v); printf(" | "); slibnfprint(u32_v); printf("\n\n");

    printf("[64-bit]\n");
    suint64 u64_v = {0}; u64_v.limbs[0] = 0x1; u64_v.limbs[1] = 0x1;
    slibprint(u64_v); printf(" | "); slibnfprint(u64_v); printf("\n\n");

    printf("[128-bit]\n");
    suint128 u128_v = {0}; u128_v.limbs[3] = 0x80000000;
    slibprint(u128_v); printf(" | "); slibnfprint(u128_v); printf("\n\n");

    printf("[256-bit]\n");
    suint256 u256_v = {0}; u256_v.limbs[7] = 0xFFFFFFFF;
    slibprint(u256_v); printf(" | "); slibnfprint(u256_v); printf("\n\n");

    printf("[1024-bit]\n");
    suint1024 u1024_v = {0}; u1024_v.limbs[0] = 1024;
    slibprint(u1024_v); printf(" | "); slibnfprint(u1024_v); printf("\n\n");

    printf("[12288-bit]\n");
    suint12288 u12288_v = {0}; u12288_v.limbs[0] = 12288;
    slibprint(u12288_v); printf(" | "); slibnfprint(u12288_v); printf("\n\n");

    printf("--- FLOAT POINT CHECK ---\n");
    sfloat256 f_v = {0}; 
    f_v.mantissa.limbs[0] = 123456789; 
    f_v.exponent = -50;
    slibprint(f_v); printf(" | "); slibnfprint(f_v); printf("\n\n");

    return 0;
}
