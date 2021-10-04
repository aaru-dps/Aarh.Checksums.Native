//
// Created by claunia on 29/9/21.
//

#if defined(__aarch64__) || defined(_M_ARM64) || defined(__arm__) || defined(_M_ARM)

#include <arm_neon.h>
#include <glob.h>
#include <stdint.h>

#include "library.h"
#include "crc32.h"
#include "crc32_simd.h"

TARGET_WITH_CRYPTO static uint64x2_t sse2neon_vmull_p64_crypto(uint64x1_t _a, uint64x1_t _b)
{
    poly64_t a = vget_lane_p64(vreinterpret_p64_u64(_a), 0);
    poly64_t b = vget_lane_p64(vreinterpret_p64_u64(_b), 0);
    return vreinterpretq_u64_p128(vmull_p64(a, b));
}

FORCE_INLINE TARGET_WITH_SIMD uint64x2_t sse2neon_vmull_p64(uint64x1_t _a, uint64x1_t _b)
{
    // Wraps vmull_p64
    if(have_arm_crypto()) return sse2neon_vmull_p64_crypto(_a, _b);

    // ARMv7 polyfill
    // ARMv7/some A64 lacks vmull_p64, but it has vmull_p8.
    //
    // vmull_p8 calculates 8 8-bit->16-bit polynomial multiplies, but we need a
    // 64-bit->128-bit polynomial multiply.
    //
    // It needs some work and is somewhat slow, but it is still faster than all
    // known scalar methods.
    //
    // Algorithm adapted to C from
    // https://www.workofard.com/2017/07/ghash-for-low-end-cores/, which is adapted
    // from "Fast Software Polynomial Multiplication on ARM Processors Using the
    // NEON Engine" by Danilo Camara, Conrado Gouvea, Julio Lopez and Ricardo Dahab
    // (https://hal.inria.fr/hal-01506572)

    poly8x8_t a = vreinterpret_p8_u64(_a);
    poly8x8_t b = vreinterpret_p8_u64(_b);

    // Masks
    uint8x16_t k48_32 = vcombine_u8(vcreate_u8(0x0000ffffffffffff), vcreate_u8(0x00000000ffffffff));
    uint8x16_t k16_00 = vcombine_u8(vcreate_u8(0x000000000000ffff), vcreate_u8(0x0000000000000000));

    // Do the multiplies, rotating with vext to get all combinations
    uint8x16_t d = vreinterpretq_u8_p16(vmull_p8(a, b));                // D = A0 * B0
    uint8x16_t e = vreinterpretq_u8_p16(vmull_p8(a, vext_p8(b, b, 1))); // E = A0 * B1
    uint8x16_t f = vreinterpretq_u8_p16(vmull_p8(vext_p8(a, a, 1), b)); // F = A1 * B0
    uint8x16_t g = vreinterpretq_u8_p16(vmull_p8(a, vext_p8(b, b, 2))); // G = A0 * B2
    uint8x16_t h = vreinterpretq_u8_p16(vmull_p8(vext_p8(a, a, 2), b)); // H = A2 * B0
    uint8x16_t i = vreinterpretq_u8_p16(vmull_p8(a, vext_p8(b, b, 3))); // I = A0 * B3
    uint8x16_t j = vreinterpretq_u8_p16(vmull_p8(vext_p8(a, a, 3), b)); // J = A3 * B0
    uint8x16_t k = vreinterpretq_u8_p16(vmull_p8(a, vext_p8(b, b, 4))); // L = A0 * B4

    // Add cross products
    uint8x16_t l = veorq_u8(e, f); // L = E + F
    uint8x16_t m = veorq_u8(g, h); // M = G + H
    uint8x16_t n = veorq_u8(i, j); // N = I + J

    // Interleave. Using vzip1 and vzip2 prevents Clang from emitting TBL
    // instructions.
#if defined(__aarch64__)
    uint8x16_t lm_p0 = vreinterpretq_u8_u64(vzip1q_u64(vreinterpretq_u64_u8(l), vreinterpretq_u64_u8(m)));
    uint8x16_t lm_p1 = vreinterpretq_u8_u64(vzip2q_u64(vreinterpretq_u64_u8(l), vreinterpretq_u64_u8(m)));
    uint8x16_t nk_p0 = vreinterpretq_u8_u64(vzip1q_u64(vreinterpretq_u64_u8(n), vreinterpretq_u64_u8(k)));
    uint8x16_t nk_p1 = vreinterpretq_u8_u64(vzip2q_u64(vreinterpretq_u64_u8(n), vreinterpretq_u64_u8(k)));
#else
    uint8x16_t lm_p0 = vcombine_u8(vget_low_u8(l), vget_low_u8(m));
    uint8x16_t lm_p1 = vcombine_u8(vget_high_u8(l), vget_high_u8(m));
    uint8x16_t nk_p0 = vcombine_u8(vget_low_u8(n), vget_low_u8(k));
    uint8x16_t nk_p1 = vcombine_u8(vget_high_u8(n), vget_high_u8(k));
#endif
    // t0 = (L) (P0 + P1) << 8
    // t1 = (M) (P2 + P3) << 16
    uint8x16_t t0t1_tmp = veorq_u8(lm_p0, lm_p1);
    uint8x16_t t0t1_h   = vandq_u8(lm_p1, k48_32);
    uint8x16_t t0t1_l   = veorq_u8(t0t1_tmp, t0t1_h);

    // t2 = (N) (P4 + P5) << 24
    // t3 = (K) (P6 + P7) << 32
    uint8x16_t t2t3_tmp = veorq_u8(nk_p0, nk_p1);
    uint8x16_t t2t3_h   = vandq_u8(nk_p1, k16_00);
    uint8x16_t t2t3_l   = veorq_u8(t2t3_tmp, t2t3_h);

    // De-interleave
#if defined(__aarch64__)
    uint8x16_t t0 = vreinterpretq_u8_u64(vuzp1q_u64(vreinterpretq_u64_u8(t0t1_l), vreinterpretq_u64_u8(t0t1_h)));
    uint8x16_t t1 = vreinterpretq_u8_u64(vuzp2q_u64(vreinterpretq_u64_u8(t0t1_l), vreinterpretq_u64_u8(t0t1_h)));
    uint8x16_t t2 = vreinterpretq_u8_u64(vuzp1q_u64(vreinterpretq_u64_u8(t2t3_l), vreinterpretq_u64_u8(t2t3_h)));
    uint8x16_t t3 = vreinterpretq_u8_u64(vuzp2q_u64(vreinterpretq_u64_u8(t2t3_l), vreinterpretq_u64_u8(t2t3_h)));
#else
    uint8x16_t t1    = vcombine_u8(vget_high_u8(t0t1_l), vget_high_u8(t0t1_h));
    uint8x16_t t0    = vcombine_u8(vget_low_u8(t0t1_l), vget_low_u8(t0t1_h));
    uint8x16_t t3    = vcombine_u8(vget_high_u8(t2t3_l), vget_high_u8(t2t3_h));
    uint8x16_t t2    = vcombine_u8(vget_low_u8(t2t3_l), vget_low_u8(t2t3_h));
#endif
    // Shift the cross products
    uint8x16_t t0_shift = vextq_u8(t0, t0, 15); // t0 << 8
    uint8x16_t t1_shift = vextq_u8(t1, t1, 14); // t1 << 16
    uint8x16_t t2_shift = vextq_u8(t2, t2, 13); // t2 << 24
    uint8x16_t t3_shift = vextq_u8(t3, t3, 12); // t3 << 32

    // Accumulate the products
    uint8x16_t cross1 = veorq_u8(t0_shift, t1_shift);
    uint8x16_t cross2 = veorq_u8(t2_shift, t3_shift);
    uint8x16_t mix    = veorq_u8(d, cross1);
    uint8x16_t r      = veorq_u8(mix, cross2);
    return vreinterpretq_u64_u8(r);
}

FORCE_INLINE uint64x2_t mm_shuffle_epi8(uint64x2_t a, uint64x2_t b)
{
    uint8x16_t tbl        = vreinterpretq_u8_u64(a);         // input a
    uint8x16_t idx        = vreinterpretq_u8_u64(b);         // input b
    uint8x16_t idx_masked = vandq_u8(idx, vdupq_n_u8(0x8F)); // avoid using meaningless bits
#if defined(__aarch64__)
    return vreinterpretq_u64_u8(vqtbl1q_u8(tbl, idx_masked));
#else
    // use this line if testing on aarch64
    uint8x8x2_t a_split = {vget_low_u8(tbl), vget_high_u8(tbl)};
    return vreinterpretq_u64_u8(
        vcombine_u8(vtbl2_u8(a_split, vget_low_u8(idx_masked)), vtbl2_u8(a_split, vget_high_u8(idx_masked))));
#endif
}

/*
 * somewhat surprisingly the "naive" way of doing this, ie. with a flag and a cond. branch,
 * is consistently ~5 % faster on average than the implied-recommended branchless way (always xor,
 * always zero q_initial). Guess speculative execution and branch prediction got the better of
 * yet another "optimization tip".
 */
#define XOR_INITIAL(where)                                                                                             \
    ONCE(where = vreinterpretq_u64_u32(veorq_u32(vreinterpretq_u32_u64(where), vreinterpretq_u32_u64(q_initial))))

TARGET_WITH_SIMD FORCE_INLINE void
    fold_1(uint64x2_t* q_crc0, uint64x2_t* q_crc1, uint64x2_t* q_crc2, uint64x2_t* q_crc3)
{
    uint32_t         ALIGNED_(16) data[4] = {0xc6e41596, 0x00000001, 0x54442bd4, 0x00000001};
    const uint64x2_t q_fold4              = vreinterpretq_u64_u32(vld1q_u32(data));

    uint64x2_t x_tmp3;
    uint32x4_t ps_crc0, ps_crc3, ps_res;

    x_tmp3 = *q_crc3;

    *q_crc3 = *q_crc0;
    *q_crc0 = sse2neon_vmull_p64(vget_high_u64(*q_crc0), vget_low_u64(q_fold4));
    *q_crc3 = sse2neon_vmull_p64(vget_low_u64(*q_crc3), vget_high_u64(q_fold4));
    ps_crc0 = vreinterpretq_u32_u64(*q_crc0);
    ps_crc3 = vreinterpretq_u32_u64(*q_crc3);
    ps_res  = veorq_u32(ps_crc0, ps_crc3);

    *q_crc0 = *q_crc1;
    *q_crc1 = *q_crc2;
    *q_crc2 = x_tmp3;
    *q_crc3 = vreinterpretq_u64_u32(ps_res);
}

TARGET_WITH_SIMD FORCE_INLINE void
    fold_2(uint64x2_t* q_crc0, uint64x2_t* q_crc1, uint64x2_t* q_crc2, uint64x2_t* q_crc3)
{
    uint32_t         ALIGNED_(16) data[4] = {0xc6e41596, 0x00000001, 0x54442bd4, 0x00000001};
    const uint64x2_t q_fold4              = vreinterpretq_u64_u32(vld1q_u32(data));

    uint64x2_t x_tmp3, x_tmp2;
    uint32x4_t ps_crc0, ps_crc1, ps_crc2, ps_crc3, ps_res31, ps_res20;

    x_tmp3 = *q_crc3;
    x_tmp2 = *q_crc2;

    *q_crc3  = *q_crc1;
    *q_crc1  = sse2neon_vmull_p64(vget_high_u64(*q_crc1), vget_low_u64(q_fold4));
    *q_crc3  = sse2neon_vmull_p64(vget_low_u64(*q_crc3), vget_high_u64(q_fold4));
    ps_crc3  = vreinterpretq_u32_u64(*q_crc3);
    ps_crc1  = vreinterpretq_u32_u64(*q_crc1);
    ps_res31 = veorq_u32(ps_crc3, ps_crc1);

    *q_crc2  = *q_crc0;
    *q_crc0  = sse2neon_vmull_p64(vget_high_u64(*q_crc0), vget_low_u64(q_fold4));
    *q_crc2  = sse2neon_vmull_p64(vget_low_u64(*q_crc2), vget_high_u64(q_fold4));
    ps_crc0  = vreinterpretq_u32_u64(*q_crc0);
    ps_crc2  = vreinterpretq_u32_u64(*q_crc2);
    ps_res20 = veorq_u32(ps_crc0, ps_crc2);

    *q_crc0 = x_tmp2;
    *q_crc1 = x_tmp3;
    *q_crc2 = vreinterpretq_u64_u32(ps_res20);
    *q_crc3 = vreinterpretq_u64_u32(ps_res31);
}

TARGET_WITH_SIMD FORCE_INLINE void
    fold_3(uint64x2_t* q_crc0, uint64x2_t* q_crc1, uint64x2_t* q_crc2, uint64x2_t* q_crc3)
{
    uint32_t         ALIGNED_(16) data[4] = {0xc6e41596, 0x00000001, 0x54442bd4, 0x00000001};
    const uint64x2_t q_fold4              = vreinterpretq_u64_u32(vld1q_u32(data));

    uint64x2_t x_tmp3;
    uint32x4_t ps_crc0, ps_crc1, ps_crc2, ps_crc3, ps_res32, ps_res21, ps_res10;

    x_tmp3 = *q_crc3;

    *q_crc3  = *q_crc2;
    *q_crc2  = sse2neon_vmull_p64(vget_high_u64(*q_crc2), vget_low_u64(q_fold4));
    *q_crc3  = sse2neon_vmull_p64(vget_low_u64(*q_crc3), vget_high_u64(q_fold4));
    ps_crc2  = vreinterpretq_u32_u64(*q_crc2);
    ps_crc3  = vreinterpretq_u32_u64(*q_crc3);
    ps_res32 = veorq_u32(ps_crc2, ps_crc3);

    *q_crc2  = *q_crc1;
    *q_crc1  = sse2neon_vmull_p64(vget_high_u64(*q_crc1), vget_low_u64(q_fold4));
    *q_crc2  = sse2neon_vmull_p64(vget_low_u64(*q_crc2), vget_high_u64(q_fold4));
    ps_crc1  = vreinterpretq_u32_u64(*q_crc1);
    ps_crc2  = vreinterpretq_u32_u64(*q_crc2);
    ps_res21 = veorq_u32(ps_crc1, ps_crc2);

    *q_crc1  = *q_crc0;
    *q_crc0  = sse2neon_vmull_p64(vget_high_u64(*q_crc0), vget_low_u64(q_fold4));
    *q_crc1  = sse2neon_vmull_p64(vget_low_u64(*q_crc1), vget_high_u64(q_fold4));
    ps_crc0  = vreinterpretq_u32_u64(*q_crc0);
    ps_crc1  = vreinterpretq_u32_u64(*q_crc1);
    ps_res10 = veorq_u32(ps_crc0, ps_crc1);

    *q_crc0 = x_tmp3;
    *q_crc1 = vreinterpretq_u64_u32(ps_res10);
    *q_crc2 = vreinterpretq_u64_u32(ps_res21);
    *q_crc3 = vreinterpretq_u64_u32(ps_res32);
}

TARGET_WITH_SIMD FORCE_INLINE void
    fold_4(uint64x2_t* q_crc0, uint64x2_t* q_crc1, uint64x2_t* q_crc2, uint64x2_t* q_crc3)
{
    uint32_t         ALIGNED_(16) data[4] = {0xc6e41596, 0x00000001, 0x54442bd4, 0x00000001};
    const uint64x2_t q_fold4              = vreinterpretq_u64_u32(vld1q_u32(data));

    uint64x2_t x_tmp0;
    uint64x2_t x_tmp1;
    uint64x2_t x_tmp2;
    uint64x2_t x_tmp3;
    uint32x4_t ps_crc0, ps_crc1, ps_crc2, ps_crc3, ps_t0, ps_t1, ps_t2, ps_t3, ps_res0, ps_res1, ps_res2, ps_res3;

    x_tmp0 = *q_crc0;
    x_tmp1 = *q_crc1;
    x_tmp2 = *q_crc2;
    x_tmp3 = *q_crc3;

    *q_crc0 = sse2neon_vmull_p64(vget_high_u64(*q_crc0), vget_low_u64(q_fold4));
    x_tmp0  = sse2neon_vmull_p64(vget_low_u64(x_tmp0), vget_high_u64(q_fold4));
    ps_crc0 = vreinterpretq_u32_u64(*q_crc0);
    ps_t0   = vreinterpretq_u32_u64(x_tmp0);
    ps_res0 = veorq_u32(ps_crc0, ps_t0);

    *q_crc1 = sse2neon_vmull_p64(vget_high_u64(*q_crc1), vget_low_u64(q_fold4));
    x_tmp1  = sse2neon_vmull_p64(vget_low_u64(x_tmp1), vget_high_u64(q_fold4));
    ps_crc1 = vreinterpretq_u32_u64(*q_crc1);
    ps_t1   = vreinterpretq_u32_u64(x_tmp1);
    ps_res1 = veorq_u32(ps_crc1, ps_t1);

    *q_crc2 = sse2neon_vmull_p64(vget_high_u64(*q_crc2), vget_low_u64(q_fold4));
    x_tmp2  = sse2neon_vmull_p64(vget_low_u64(x_tmp2), vget_high_u64(q_fold4));
    ps_crc2 = vreinterpretq_u32_u64(*q_crc2);
    ps_t2   = vreinterpretq_u32_u64(x_tmp2);
    ps_res2 = veorq_u32(ps_crc2, ps_t2);

    *q_crc3 = sse2neon_vmull_p64(vget_high_u64(*q_crc3), vget_low_u64(q_fold4));
    x_tmp3  = sse2neon_vmull_p64(vget_low_u64(x_tmp3), vget_high_u64(q_fold4));
    ps_crc3 = vreinterpretq_u32_u64(*q_crc3);
    ps_t3   = vreinterpretq_u32_u64(x_tmp3);
    ps_res3 = veorq_u32(ps_crc3, ps_t3);

    *q_crc0 = vreinterpretq_u64_u32(ps_res0);
    *q_crc1 = vreinterpretq_u64_u32(ps_res1);
    *q_crc2 = vreinterpretq_u64_u32(ps_res2);
    *q_crc3 = vreinterpretq_u64_u32(ps_res3);
}

TARGET_WITH_SIMD FORCE_INLINE void partial_fold(const size_t len,
                                                uint64x2_t*  q_crc0,
                                                uint64x2_t*  q_crc1,
                                                uint64x2_t*  q_crc2,
                                                uint64x2_t*  q_crc3,
                                                uint64x2_t*  q_crc_part)
{
    uint32_t         ALIGNED_(16) data[4] = {0xc6e41596, 0x00000001, 0x54442bd4, 0x00000001};
    const uint64x2_t q_fold4              = vreinterpretq_u64_u32(vld1q_u32(data));
    const uint64x2_t q_mask3              = vreinterpretq_u64_u32(vdupq_n_u32(0x80808080));

    uint64x2_t q_shl, q_shr, q_tmp1, q_tmp2, q_tmp3, q_a0_0, q_a0_1;
    uint32x4_t ps_crc3, psa0_0, psa0_1, ps_res;

    q_shl = vreinterpretq_u64_u32(vld1q_u32((const uint32_t*)pshufb_shf_table + (len - 1) * 4));
    q_shr = q_shl;
    q_shr = vreinterpretq_u64_u32(veorq_u32(vreinterpretq_u32_u64(q_shr), vreinterpretq_u32_u64(q_mask3)));

    q_a0_0 = mm_shuffle_epi8(*q_crc0, q_shl);

    *q_crc0 = mm_shuffle_epi8(*q_crc0, q_shr);
    q_tmp1  = mm_shuffle_epi8(*q_crc1, q_shl);
    *q_crc0 = vreinterpretq_u64_u32(vorrq_u32(vreinterpretq_u32_u64(*q_crc0), vreinterpretq_u32_u64(q_tmp1)));

    *q_crc1 = mm_shuffle_epi8(*q_crc1, q_shr);
    q_tmp2  = mm_shuffle_epi8(*q_crc2, q_shl);
    *q_crc1 = vreinterpretq_u64_u32(vorrq_u32(vreinterpretq_u32_u64(*q_crc1), vreinterpretq_u32_u64(q_tmp2)));

    *q_crc2 = mm_shuffle_epi8(*q_crc2, q_shr);
    q_tmp3  = mm_shuffle_epi8(*q_crc3, q_shl);
    *q_crc2 = vreinterpretq_u64_u32(vorrq_u32(vreinterpretq_u32_u64(*q_crc2), vreinterpretq_u32_u64(q_tmp3)));

    *q_crc3     = mm_shuffle_epi8(*q_crc3, q_shr);
    *q_crc_part = mm_shuffle_epi8(*q_crc_part, q_shl);
    *q_crc3     = vreinterpretq_u64_u32(vorrq_u32(vreinterpretq_u32_u64(*q_crc3), vreinterpretq_u32_u64(*q_crc_part)));

    q_a0_1 = sse2neon_vmull_p64(vget_low_u64(q_a0_0), vget_high_u64(q_fold4));
    q_a0_0 = sse2neon_vmull_p64(vget_high_u64(q_a0_0), vget_low_u64(q_fold4));

    ps_crc3 = vreinterpretq_u32_u64(*q_crc3);
    psa0_0  = vreinterpretq_u32_u64(q_a0_0);
    psa0_1  = vreinterpretq_u32_u64(q_a0_1);

    ps_res = veorq_u32(ps_crc3, psa0_0);
    ps_res = veorq_u32(ps_res, psa0_1);

    *q_crc3 = vreinterpretq_u64_u32(ps_res);
}

TARGET_WITH_SIMD uint32_t crc32_vmull(const uint8_t* src, long len, uint32_t initial_crc)
{
    unsigned long algn_diff;
    uint64x2_t    q_t0;
    uint64x2_t    q_t1;
    uint64x2_t    q_t2;
    uint64x2_t    q_t3;
    uint64x2_t    q_initial = vreinterpretq_u64_u32(vsetq_lane_u32(initial_crc, vdupq_n_u32(0), 0));
    uint64x2_t    q_crc0    = vreinterpretq_u64_u32(vsetq_lane_u32(0x9db42487, vdupq_n_u32(0), 0));
    uint64x2_t    q_crc1    = vreinterpretq_u64_u32(vdupq_n_u32(0));
    uint64x2_t    q_crc2    = vreinterpretq_u64_u32(vdupq_n_u32(0));
    uint64x2_t    q_crc3    = vreinterpretq_u64_u32(vdupq_n_u32(0));
    uint64x2_t    q_crc_part;

    int first = 1;

    /* fold 512 to 32 step variable declarations for ISO-C90 compat. */
    const uint64x2_t q_mask  = vreinterpretq_u64_u32(vld1q_u32((const uint32_t*)crc_mask));
    const uint64x2_t q_mask2 = vreinterpretq_u64_u32(vld1q_u32((const uint32_t*)crc_mask2));

    uint32_t   crc;
    uint64x2_t x_tmp0;
    uint64x2_t x_tmp1;
    uint64x2_t x_tmp2;
    uint64x2_t crc_fold;

    if(len < 16)
    {
        if(len == 0) return initial_crc;
        if(len < 4)
        {
            /*
             * no idea how to do this for <4 bytes, delegate to classic impl.
             */
            uint32_t crc = ~initial_crc;
            switch(len)
            {
                case 3: crc = (crc >> 8) ^ crc32_table[0][(crc & 0xFF) ^ *src++];
                case 2: crc = (crc >> 8) ^ crc32_table[0][(crc & 0xFF) ^ *src++];
                case 1: crc = (crc >> 8) ^ crc32_table[0][(crc & 0xFF) ^ *src++];
            }
            return ~crc;
        }
        q_crc_part = vreinterpretq_u64_u32(vld1q_u32((const uint32_t*)src));
        XOR_INITIAL(q_crc_part);
        goto partial;
    }

    /* this alignment computation would be wrong for len<16 handled above */
    algn_diff = (0 - (uintptr_t)src) & 0xF;
    if(algn_diff)
    {
        q_crc_part = vreinterpretq_u64_u32(vld1q_u32((const uint32_t*)src));
        XOR_INITIAL(q_crc_part);

        src += algn_diff;
        len -= algn_diff;

        partial_fold(algn_diff, &q_crc0, &q_crc1, &q_crc2, &q_crc3, &q_crc_part);
    }

    while((len -= 64) >= 0)
    {
        q_t0 = vreinterpretq_u64_u32(vld1q_u32((const uint32_t*)src));
        q_t1 = vreinterpretq_u64_u32(vld1q_u32((const uint32_t*)src + 4));
        q_t2 = vreinterpretq_u64_u32(vld1q_u32((const uint32_t*)src + 8));
        q_t3 = vreinterpretq_u64_u32(vld1q_u32((const uint32_t*)src + 12));

        XOR_INITIAL(q_t0);

        fold_4(&q_crc0, &q_crc1, &q_crc2, &q_crc3);

        q_crc0 = vreinterpretq_u64_u32(veorq_u32(vreinterpretq_u32_u64(q_crc0), vreinterpretq_u32_u64(q_t0)));
        q_crc1 = vreinterpretq_u64_u32(veorq_u32(vreinterpretq_u32_u64(q_crc1), vreinterpretq_u32_u64(q_t1)));
        q_crc2 = vreinterpretq_u64_u32(veorq_u32(vreinterpretq_u32_u64(q_crc2), vreinterpretq_u32_u64(q_t2)));
        q_crc3 = vreinterpretq_u64_u32(veorq_u32(vreinterpretq_u32_u64(q_crc3), vreinterpretq_u32_u64(q_t3)));

        src += 64;
    }

    /*
     * len = num bytes left - 64
     */
    if(len + 16 >= 0)
    {
        len += 16;

        q_t0 = vreinterpretq_u64_u32(vld1q_u32((const uint32_t*)src));
        q_t1 = vreinterpretq_u64_u32(vld1q_u32((const uint32_t*)src + 4));
        q_t2 = vreinterpretq_u64_u32(vld1q_u32((const uint32_t*)src + 8));

        XOR_INITIAL(q_t0);

        fold_3(&q_crc0, &q_crc1, &q_crc2, &q_crc3);

        q_crc1 = vreinterpretq_u64_u32(veorq_u32(vreinterpretq_u32_u64(q_crc1), vreinterpretq_u32_u64(q_t0)));
        q_crc2 = vreinterpretq_u64_u32(veorq_u32(vreinterpretq_u32_u64(q_crc2), vreinterpretq_u32_u64(q_t1)));
        q_crc3 = vreinterpretq_u64_u32(veorq_u32(vreinterpretq_u32_u64(q_crc3), vreinterpretq_u32_u64(q_t2)));

        if(len == 0) goto done;

        q_crc_part = vreinterpretq_u64_u32(vld1q_u32((const uint32_t*)src + 12));
    }
    else if(len + 32 >= 0)
    {
        len += 32;

        q_t0 = vreinterpretq_u64_u32(vld1q_u32((const uint32_t*)src));
        q_t1 = vreinterpretq_u64_u32(vld1q_u32((const uint32_t*)src + 4));

        XOR_INITIAL(q_t0);

        fold_2(&q_crc0, &q_crc1, &q_crc2, &q_crc3);

        q_crc2 = vreinterpretq_u64_u32(veorq_u32(vreinterpretq_u32_u64(q_crc2), vreinterpretq_u32_u64(q_t0)));
        q_crc3 = vreinterpretq_u64_u32(veorq_u32(vreinterpretq_u32_u64(q_crc3), vreinterpretq_u32_u64(q_t1)));

        if(len == 0) goto done;

        q_crc_part = vreinterpretq_u64_u32(vld1q_u32((const uint32_t*)src + 8));
    }
    else if(len + 48 >= 0)
    {
        len += 48;

        q_t0 = vreinterpretq_u64_u32(vld1q_u32((const uint32_t*)src));

        XOR_INITIAL(q_t0);

        fold_1(&q_crc0, &q_crc1, &q_crc2, &q_crc3);

        q_crc3 = vreinterpretq_u64_u32(veorq_u32(vreinterpretq_u32_u64(q_crc3), vreinterpretq_u32_u64(q_t0)));

        if(len == 0) goto done;

        q_crc_part = vreinterpretq_u64_u32(vld1q_u32((const uint32_t*)src + 4));
    }
    else
    {
        len += 64;
        if(len == 0) goto done;
        q_crc_part = vreinterpretq_u64_u32(vld1q_u32((const uint32_t*)src));
        XOR_INITIAL(q_crc_part);
    }

partial:
    partial_fold(len, &q_crc0, &q_crc1, &q_crc2, &q_crc3, &q_crc_part);

done:
    (void)0;

    /* fold 512 to 32 */

    /*
     * k1
     */
    crc_fold = vreinterpretq_u64_u32(vld1q_u32(crc_k));

    x_tmp0 = sse2neon_vmull_p64(vget_low_u64((q_crc0)), vget_high_u64((crc_fold)));
    q_crc0 = sse2neon_vmull_p64(vget_high_u64((q_crc0)), vget_low_u64((crc_fold)));
    q_crc1 = vreinterpretq_u64_u32(veorq_u32(vreinterpretq_u32_u64(q_crc1), vreinterpretq_u32_u64(x_tmp0)));
    q_crc1 = vreinterpretq_u64_u32(veorq_u32(vreinterpretq_u32_u64(q_crc1), vreinterpretq_u32_u64(q_crc0)));

    x_tmp1 = sse2neon_vmull_p64(vget_low_u64((q_crc1)), vget_high_u64((crc_fold)));
    q_crc1 = sse2neon_vmull_p64(vget_high_u64((q_crc1)), vget_low_u64((crc_fold)));
    q_crc2 = vreinterpretq_u64_u32(veorq_u32(vreinterpretq_u32_u64(q_crc2), vreinterpretq_u32_u64(x_tmp1)));
    q_crc2 = vreinterpretq_u64_u32(veorq_u32(vreinterpretq_u32_u64(q_crc2), vreinterpretq_u32_u64(q_crc1)));

    x_tmp2 = sse2neon_vmull_p64(vget_low_u64((q_crc2)), vget_high_u64((crc_fold)));
    q_crc2 = sse2neon_vmull_p64(vget_high_u64((q_crc2)), vget_low_u64((crc_fold)));
    q_crc3 = vreinterpretq_u64_u32(veorq_u32(vreinterpretq_u32_u64(q_crc3), vreinterpretq_u32_u64(x_tmp2)));
    q_crc3 = vreinterpretq_u64_u32(veorq_u32(vreinterpretq_u32_u64(q_crc3), vreinterpretq_u32_u64(q_crc2)));

    /*
     * k5
     */
    crc_fold = vreinterpretq_u64_u32(vld1q_u32(crc_k + 4));

    q_crc0            = q_crc3;
    q_crc3            = (sse2neon_vmull_p64(vget_low_u64((q_crc3)), vget_low_u64((crc_fold))));
    uint8x16_t tmp[2] = {vreinterpretq_u8_u64(q_crc0), vdupq_n_u8(0)};
    q_crc0            = vreinterpretq_u64_u8(vld1q_u8(((uint8_t const*)tmp) + 8));
    q_crc3            = vreinterpretq_u64_u32(veorq_u32(vreinterpretq_u32_u64(q_crc3), vreinterpretq_u32_u64(q_crc0)));

    q_crc0              = q_crc3;
    uint8x16_t tmp_1[2] = {vdupq_n_u8(0), vreinterpretq_u8_u64(q_crc3)};
    q_crc3              = vreinterpretq_u64_u8(vld1q_u8(((uint8_t const*)tmp_1) + (16 - 4)));
    q_crc3              = (sse2neon_vmull_p64(vget_low_u64((q_crc3)), vget_high_u64((crc_fold))));
    q_crc3 = vreinterpretq_u64_u32(veorq_u32(vreinterpretq_u32_u64(q_crc3), vreinterpretq_u32_u64(q_crc0)));
    q_crc3 = vreinterpretq_u64_u32(vandq_u32(vreinterpretq_u32_u64(q_crc3), vreinterpretq_u32_u64(q_mask2)));

    /*
     * k7
     */
    q_crc1   = q_crc3;
    q_crc2   = q_crc3;
    crc_fold = vreinterpretq_u64_u32(vld1q_u32(crc_k + 8));

    q_crc3 = (sse2neon_vmull_p64(vget_low_u64((q_crc3)), vget_low_u64((crc_fold))));
    q_crc3 = vreinterpretq_u64_u32(veorq_u32(vreinterpretq_u32_u64(q_crc3), vreinterpretq_u32_u64(q_crc2)));
    q_crc3 = vreinterpretq_u64_u32(vandq_u32(vreinterpretq_u32_u64(q_crc3), vreinterpretq_u32_u64(q_mask)));

    q_crc2 = q_crc3;
    q_crc3 = (sse2neon_vmull_p64(vget_low_u64((q_crc3)), vget_high_u64((crc_fold))));
    q_crc3 = vreinterpretq_u64_u32(veorq_u32(vreinterpretq_u32_u64(q_crc3), vreinterpretq_u32_u64(q_crc2)));
    q_crc3 = vreinterpretq_u64_u32(veorq_u32(vreinterpretq_u32_u64(q_crc3), vreinterpretq_u32_u64(q_crc1)));

    /*
     * could just as well write q_crc3[2], doing a movaps and truncating, but
     * no real advantage - it's a tiny bit slower per call, while no additional CPUs
     * would be supported by only requiring SSSE3 and CLMUL instead of SSE4.1 + CLMUL
     */
    crc = vgetq_lane_u32(vreinterpretq_u32_u64(q_crc3), (2));
    return ~crc;
}

#endif