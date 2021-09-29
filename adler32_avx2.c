//
// Created by claunia on 28/9/21.
//

#if defined(__x86_64__) || defined(__amd64) || defined(_M_AMD64) || defined(_M_X64) || defined(__I386__) ||            \
    defined(__i386__) || defined(__THW_INTEL) || defined(_M_IX86)

#include <immintrin.h>
#include <stdint.h>

#include "library.h"
#include "adler32.h"
#include "simd.h"

AVX2 void adler32_avx2(uint16_t* sum1, uint16_t* sum2, const unsigned char* buf, size_t len)
{
    uint32_t s1 = *sum1;
    uint32_t s2 = *sum2;

    /*
     * Process the data in blocks.
     */
    const unsigned BLOCK_SIZE = 1 << 5;
    size_t         blocks     = len / BLOCK_SIZE;
    len -= blocks * BLOCK_SIZE;

    while(blocks)
    {
        unsigned n = NMAX / BLOCK_SIZE; /* The NMAX constraint. */

        if(n > blocks) n = (unsigned)blocks;
        blocks -= n;

        const __m256i tap  = _mm256_set_epi8(1,
                                            2,
                                            3,
                                            4,
                                            5,
                                            6,
                                            7,
                                            8,
                                            9,
                                            10,
                                            11,
                                            12,
                                            13,
                                            14,
                                            15,
                                            16,
                                            17,
                                            18,
                                            19,
                                            20,
                                            21,
                                            22,
                                            23,
                                            24,
                                            25,
                                            26,
                                            27,
                                            28,
                                            29,
                                            30,
                                            31,
                                            32);
        const __m256i zero = _mm256_setzero_si256();
        const __m256i ones = _mm256_set1_epi16(1);

        /*
         * Process n blocks of data. At most NMAX data bytes can be
         * processed before s2 must be reduced modulo BASE.
         */
        __m256i v_ps = _mm256_set_epi32(0, 0, 0, 0, 0, 0, 0, (s1 * n));
        __m256i v_s2 = _mm256_set_epi32(0, 0, 0, 0, 0, 0, 0, s2);
        __m256i v_s1 = _mm256_setzero_si256();
        do {
            /*
             * Load 32 input bytes.
             */
            const __m256i bytes = _mm256_lddqu_si256((__m256i*)(buf));

            /*
             * Add previous block byte sum to v_ps.
             */
            v_ps = _mm256_add_epi32(v_ps, v_s1);
            /*
             * Horizontally add the bytes for s1, multiply-adds the
             * bytes by [ 32, 31, 30, ... ] for s2.
             */
            v_s1              = _mm256_add_epi32(v_s1, _mm256_sad_epu8(bytes, zero));
            const __m256i mad = _mm256_maddubs_epi16(bytes, tap);
            v_s2              = _mm256_add_epi32(v_s2, _mm256_madd_epi16(mad, ones));

            buf += BLOCK_SIZE;
        } while(--n);

        __m128i sum = _mm_add_epi32(_mm256_castsi256_si128(v_s1), _mm256_extracti128_si256(v_s1, 1));
        __m128i hi  = _mm_unpackhi_epi64(sum, sum);
        sum         = _mm_add_epi32(hi, sum);
        hi          = _mm_shuffle_epi32(sum, 177);
        sum         = _mm_add_epi32(sum, hi);
        s1 += _mm_cvtsi128_si32(sum);

        v_s2 = _mm256_add_epi32(v_s2, _mm256_slli_epi32(v_ps, 5));
        sum  = _mm_add_epi32(_mm256_castsi256_si128(v_s2), _mm256_extracti128_si256(v_s2, 1));
        hi   = _mm_unpackhi_epi64(sum, sum);
        sum  = _mm_add_epi32(hi, sum);
        hi   = _mm_shuffle_epi32(sum, 177);
        sum  = _mm_add_epi32(sum, hi);
        s2   = _mm_cvtsi128_si32(sum);

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