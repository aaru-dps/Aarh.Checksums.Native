/* adler32_simd.c
 *
 * Copyright 2017 The Chromium Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the Chromium source repository LICENSE file.
 *
 * Per http://en.wikipedia.org/wiki/Adler-32 the adler32 A value (aka s1) is
 * the sum of N input data bytes D1 ... DN,
 *
 *   A = A0 + D1 + D2 + ... + DN
 *
 * where A0 is the initial value.
 *
 * SSE2 _mm_sad_epu8() can be used for byte sums (see http://bit.ly/2wpUOeD,
 * for example) and accumulating the byte sums can use SSE shuffle-adds (see
 * the "Integer" section of http://bit.ly/2erPT8t for details). Arm NEON has
 * similar instructions.
 *
 * The adler32 B value (aka s2) sums the A values from each step:
 *
 *   B0 + (A0 + D1) + (A0 + D1 + D2) + ... + (A0 + D1 + D2 + ... + DN) or
 *
 *       B0 + N.A0 + N.D1 + (N-1).D2 + (N-2).D3 + ... + (N-(N-1)).DN
 *
 * B0 being the initial value. For 32 bytes (ideal for garden-variety SIMD):
 *
 *   B = B0 + 32.A0 + [D1 D2 D3 ... D32] x [32 31 30 ... 1].
 *
 * Adjacent blocks of 32 input bytes can be iterated with the expressions to
 * compute the adler32 s1 s2 of M >> 32 input bytes [1].
 *
 * As M grows, the s1 s2 sums grow. If left unchecked, they would eventually
 * overflow the precision of their integer representation (bad). However, s1
 * and s2 also need to be computed modulo the adler BASE value (reduced). If
 * at most NMAX bytes are processed before a reduce, s1 s2 _cannot_ overflow
 * a uint32_t type (the NMAX constraint) [2].
 *
 * [1] the iterative equations for s2 contain constant factors; these can be
 * hoisted from the n-blocks do loop of the SIMD code.
 *
 * [2] zlib adler32_z() uses this fact to implement NMAX-block-based updates
 * of the adler s1 s2 of uint32_t type (see adler32.c).
 */

#if defined(__x86_64__) || defined(__amd64) || defined(_M_AMD64) || defined(_M_X64) || defined(__I386__) ||            \
    defined(__i386__) || defined(__THW_INTEL) || defined(_M_IX86)

#include <stdint.h>
#include <tmmintrin.h>

#include "library.h"
#include "adler32.h"

SSSE3 void adler32_ssse3(uint16_t* sum1, uint16_t* sum2, const unsigned char* buf, long len)
{
    uint32_t s1 = *sum1;
    uint32_t s2 = *sum2;

    /*
     * Process the data in blocks.
     */
    const unsigned BLOCK_SIZE = 1 << 5;
    long           blocks     = len / BLOCK_SIZE;
    len -= blocks * BLOCK_SIZE;
    while(blocks)
    {
        unsigned n = NMAX / BLOCK_SIZE; /* The NMAX constraint. */
        if(n > blocks) n = (unsigned)blocks;
        blocks -= n;
        const __m128i tap1 = _mm_setr_epi8(32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17);
        const __m128i tap2 = _mm_setr_epi8(16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1);
        const __m128i zero = _mm_setr_epi8(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        const __m128i ones = _mm_set_epi16(1, 1, 1, 1, 1, 1, 1, 1);
        /*
         * Process n blocks of data. At most NMAX data bytes can be
         * processed before s2 must be reduced modulo BASE.
         */
        __m128i v_ps = _mm_set_epi32(0, 0, 0, s1 * n);
        __m128i v_s2 = _mm_set_epi32(0, 0, 0, s2);
        __m128i v_s1 = _mm_set_epi32(0, 0, 0, 0);
        do {
            /*
             * Load 32 input bytes.
             */
            const __m128i bytes1 = _mm_loadu_si128((__m128i*)(buf));
            const __m128i bytes2 = _mm_loadu_si128((__m128i*)(buf + 16));
            /*
             * Add previous block byte sum to v_ps.
             */
            v_ps = _mm_add_epi32(v_ps, v_s1);
            /*
             * Horizontally add the bytes for s1, multiply-adds the
             * bytes by [ 32, 31, 30, ... ] for s2.
             */
            v_s1               = _mm_add_epi32(v_s1, _mm_sad_epu8(bytes1, zero));
            const __m128i mad1 = _mm_maddubs_epi16(bytes1, tap1);
            v_s2               = _mm_add_epi32(v_s2, _mm_madd_epi16(mad1, ones));
            v_s1               = _mm_add_epi32(v_s1, _mm_sad_epu8(bytes2, zero));
            const __m128i mad2 = _mm_maddubs_epi16(bytes2, tap2);
            v_s2               = _mm_add_epi32(v_s2, _mm_madd_epi16(mad2, ones));
            buf += BLOCK_SIZE;
        } while(--n);
        v_s2 = _mm_add_epi32(v_s2, _mm_slli_epi32(v_ps, 5));
        /*
         * Sum epi32 ints v_s1(s2) and accumulate in s1(s2).
         */
#define S23O1 _MM_SHUFFLE(2, 3, 0, 1) /* A B C D -> B A D C */
#define S1O32 _MM_SHUFFLE(1, 0, 3, 2) /* A B C D -> C D A B */
        v_s1 = _mm_add_epi32(v_s1, _mm_shuffle_epi32(v_s1, S23O1));
        v_s1 = _mm_add_epi32(v_s1, _mm_shuffle_epi32(v_s1, S1O32));
        s1 += _mm_cvtsi128_si32(v_s1);
        v_s2 = _mm_add_epi32(v_s2, _mm_shuffle_epi32(v_s2, S23O1));
        v_s2 = _mm_add_epi32(v_s2, _mm_shuffle_epi32(v_s2, S1O32));
        s2   = _mm_cvtsi128_si32(v_s2);
#undef S23O1
#undef S1O32
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