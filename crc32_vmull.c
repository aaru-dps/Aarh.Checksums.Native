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
#include "arm_vmull.h"

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