//
// Created by claunia on 28/9/21.
//

#if defined(__aarch64__) || defined(_M_ARM64) || defined(__arm__) || defined(_M_ARM)

#include <arm_neon.h>

#include "library.h"
#include "adler32.h"
#include "simd.h"

void adler32_neon(uint16_t* sum1, uint16_t* sum2, const unsigned char* buf, uint32_t len)
{
    /*
     * Split Adler-32 into component sums.
     */
    uint32_t s1 = *sum1;
    uint32_t s2 = *sum2;
    /*
     * Serially compute s1 & s2, until the data is 16-byte aligned.
     */
    if((uintptr_t)buf & 15)
    {
        while((uintptr_t)buf & 15)
        {
            s2 += (s1 += *buf++);
            --len;
        }
        if(s1 >= ADLER_MODULE) s1 -= ADLER_MODULE;
        s2 %= ADLER_MODULE;
    }
    /*
     * Process the data in blocks.
     */
    const unsigned BLOCK_SIZE = 1 << 5;
    uint32_t       blocks     = len / BLOCK_SIZE;
    len -= blocks * BLOCK_SIZE;
    while(blocks)
    {
        unsigned n = NMAX / BLOCK_SIZE; /* The NMAX constraint. */
        if(n > blocks) n = (unsigned)blocks;
        blocks -= n;
        /*
         * Process n blocks of data. At most NMAX data bytes can be
         * processed before s2 must be reduced modulo ADLER_MODULE.
         */
        uint32x4_t v_s2           = (uint32x4_t){0, 0, 0, s1 * n};
        uint32x4_t v_s1           = (uint32x4_t){0, 0, 0, 0};
        uint16x8_t v_column_sum_1 = vdupq_n_u16(0);
        uint16x8_t v_column_sum_2 = vdupq_n_u16(0);
        uint16x8_t v_column_sum_3 = vdupq_n_u16(0);
        uint16x8_t v_column_sum_4 = vdupq_n_u16(0);
        do {
            /*
             * Load 32 input bytes.
             */
            const uint8x16_t bytes1 = vld1q_u8((uint8_t*)(buf));
            const uint8x16_t bytes2 = vld1q_u8((uint8_t*)(buf + 16));
            /*
             * Add previous block byte sum to v_s2.
             */
            v_s2 = vaddq_u32(v_s2, v_s1);
            /*
             * Horizontally add the bytes for s1.
             */
            v_s1 = vpadalq_u16(v_s1, vpadalq_u8(vpaddlq_u8(bytes1), bytes2));
            /*
             * Vertically add the bytes for s2.
             */
            v_column_sum_1 = vaddw_u8(v_column_sum_1, vget_low_u8(bytes1));
            v_column_sum_2 = vaddw_u8(v_column_sum_2, vget_high_u8(bytes1));
            v_column_sum_3 = vaddw_u8(v_column_sum_3, vget_low_u8(bytes2));
            v_column_sum_4 = vaddw_u8(v_column_sum_4, vget_high_u8(bytes2));
            buf += BLOCK_SIZE;
        } while(--n);
        v_s2 = vshlq_n_u32(v_s2, 5);
        /*
         * Multiply-add bytes by [ 32, 31, 30, ... ] for s2.
         */
        v_s2 = vmlal_u16(v_s2, vget_low_u16(v_column_sum_1), (uint16x4_t){32, 31, 30, 29});
        v_s2 = vmlal_u16(v_s2, vget_high_u16(v_column_sum_1), (uint16x4_t){28, 27, 26, 25});
        v_s2 = vmlal_u16(v_s2, vget_low_u16(v_column_sum_2), (uint16x4_t){24, 23, 22, 21});
        v_s2 = vmlal_u16(v_s2, vget_high_u16(v_column_sum_2), (uint16x4_t){20, 19, 18, 17});
        v_s2 = vmlal_u16(v_s2, vget_low_u16(v_column_sum_3), (uint16x4_t){16, 15, 14, 13});
        v_s2 = vmlal_u16(v_s2, vget_high_u16(v_column_sum_3), (uint16x4_t){12, 11, 10, 9});
        v_s2 = vmlal_u16(v_s2, vget_low_u16(v_column_sum_4), (uint16x4_t){8, 7, 6, 5});
        v_s2 = vmlal_u16(v_s2, vget_high_u16(v_column_sum_4), (uint16x4_t){4, 3, 2, 1});
        /*
         * Sum epi32 ints v_s1(s2) and accumulate in s1(s2).
         */
        uint32x2_t sum1 = vpadd_u32(vget_low_u32(v_s1), vget_high_u32(v_s1));
        uint32x2_t sum2 = vpadd_u32(vget_low_u32(v_s2), vget_high_u32(v_s2));
        uint32x2_t s1s2 = vpadd_u32(sum1, sum2);
        s1 += vget_lane_u32(s1s2, 0);
        s2 += vget_lane_u32(s1s2, 1);
        /*
         * Reduce.
         */
        s1 %= ADLER_MODULE;
        s2 %= ADLER_MODULE;
    }
    /*
     * Handle leftover data.
     */
    if(len)
    {
        if(len >= 16)
        {
            s2 += (s1 += *buf++);
            s2 += (s1 += *buf++);
            s2 += (s1 += *buf++);
            s2 += (s1 += *buf++);
            s2 += (s1 += *buf++);
            s2 += (s1 += *buf++);
            s2 += (s1 += *buf++);
            s2 += (s1 += *buf++);
            s2 += (s1 += *buf++);
            s2 += (s1 += *buf++);
            s2 += (s1 += *buf++);
            s2 += (s1 += *buf++);
            s2 += (s1 += *buf++);
            s2 += (s1 += *buf++);
            s2 += (s1 += *buf++);
            s2 += (s1 += *buf++);
            len -= 16;
        }
        while(len--) { s2 += (s1 += *buf++); }
        if(s1 >= ADLER_MODULE) s1 -= ADLER_MODULE;
        s2 %= ADLER_MODULE;
    }
    /*
     * Return the recombined sums.
     */
    *sum1 = s1 & 0xFFFF;
    *sum2 = s2 & 0xFFFF;
}

#endif