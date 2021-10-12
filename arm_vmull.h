//
// Created by claunia on 12/10/21.
//

#ifndef AARU_CHECKSUMS_NATIVE__ARM_VMULL_H_
#define AARU_CHECKSUMS_NATIVE__ARM_VMULL_H_

#if defined(__aarch64__) || defined(_M_ARM64) || defined(__arm__) || defined(_M_ARM)

TARGET_WITH_CRYPTO static uint64x2_t sse2neon_vmull_p64_crypto(uint64x1_t _a, uint64x1_t _b);
TARGET_WITH_SIMD uint64x2_t          sse2neon_vmull_p64(uint64x1_t _a, uint64x1_t _b);
TARGET_WITH_SIMD uint64x2_t          mm_shuffle_epi8(uint64x2_t a, uint64x2_t b);
TARGET_WITH_SIMD uint64x2_t          mm_srli_si128(uint64x2_t a, int imm);
TARGET_WITH_SIMD uint64x2_t          mm_slli_si128(uint64x2_t a, int imm);

#endif

#endif // AARU_CHECKSUMS_NATIVE__ARM_VMULL_H_
