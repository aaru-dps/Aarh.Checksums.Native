/*
 * This file is part of the Aaru Data Preservation Suite.
 * Copyright (c) 2019-2021 Natalia Portillo.
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of the
 * License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "library.h"

#include "spamsum.h"

static uint8_t _b64[] = {0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D,
                         0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A,
                         0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D,
                         0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A,
                         0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x2B, 0x2F};

AARU_EXPORT spamsum_ctx* AARU_CALL spamsum_init(void)
{
    spamsum_ctx* ctx = (spamsum_ctx*)malloc(sizeof(spamsum_ctx));
    if(!ctx) return NULL;

    memset(ctx, 0, sizeof(spamsum_ctx));

    ctx->Bhend       = 1;
    ctx->Bh[0].H     = HASH_INIT;
    ctx->Bh[0].Halfh = HASH_INIT;

    return ctx;
}

AARU_EXPORT int AARU_CALL spamsum_update(spamsum_ctx* ctx, const uint8_t* data, uint32_t len)
{
    if(!ctx || !data) return -1;

    for(int i = 0; i < len; i++) fuzzy_engine_step(ctx, data[i]);

    ctx->TotalSize += len;

    return 0;
}

AARU_EXPORT void AARU_CALL spamsum_free(spamsum_ctx* ctx)
{
    if(ctx) free(ctx);
}

#define roll_sum(ctx) (ctx->Roll.H1 + ctx->Roll.H2 + ctx->Roll.H3)
#define sum_hash(c, h) ((h * HASH_PRIME) ^ c);
#define SSDEEP_BS(index) (MIN_BLOCKSIZE << index)

AARU_LOCAL void fuzzy_engine_step(spamsum_ctx* ctx, uint8_t c)
{
    uint32_t i;
    /* At each character we update the rolling hash and the normal hashes.
     * When the rolling hash hits a reset value then we emit a normal hash
     * as a element of the signature and reset the normal hash. */
    roll_hash(ctx, c);
    uint64_t h = roll_sum(ctx);

    for(i = ctx->Bhstart; i < ctx->Bhend; ++i)
    {
        ctx->Bh[i].H     = sum_hash(c, ctx->Bh[i].H);
        ctx->Bh[i].Halfh = sum_hash(c, ctx->Bh[i].Halfh);
    }

    for(i = ctx->Bhstart; i < ctx->Bhend; ++i)
    {
        /* With growing blocksize almost no runs fail the next test. */
        if(h % SSDEEP_BS(i) != SSDEEP_BS(i) - 1)
            /* Once this condition is false for one bs, it is
             * automatically false for all further bs. I.e. if
             * h === -1 (mod 2*bs) then h === -1 (mod bs). */
            break;

        /* We have hit a reset point. We now emit hashes which are
         * based on all characters in the piece of the message between
         * the last reset point and this one */
        if(0 == ctx->Bh[i].Dlen) fuzzy_try_fork_blockhash(ctx);

        ctx->Bh[i].Digest[ctx->Bh[i].Dlen] = _b64[ctx->Bh[i].H % 64];
        ctx->Bh[i].Halfdigest              = _b64[ctx->Bh[i].Halfh % 64];

        if(ctx->Bh[i].Dlen < SPAMSUM_LENGTH - 1)
        {
            /* We can have a problem with the tail overflowing. The
             * easiest way to cope with this is to only reset the
             * normal hash if we have room for more characters in
             * our signature. This has the effect of combining the
             * last few pieces of the message into a single piece
             * */
            ctx->Bh[i].Digest[++ctx->Bh[i].Dlen] = 0;
            ctx->Bh[i].H                         = HASH_INIT;

            if(ctx->Bh[i].Dlen >= SPAMSUM_LENGTH / 2) continue;

            ctx->Bh[i].Halfh      = HASH_INIT;
            ctx->Bh[i].Halfdigest = 0;
        }
        else
            fuzzy_try_reduce_blockhash(ctx);
    }
}

AARU_LOCAL void roll_hash(spamsum_ctx* ctx, uint8_t c)
{
    ctx->Roll.H2 -= ctx->Roll.H1;
    ctx->Roll.H2 += ROLLING_WINDOW * c;

    ctx->Roll.H1 += c;
    ctx->Roll.H1 -= ctx->Roll.Window[ctx->Roll.N % ROLLING_WINDOW];

    ctx->Roll.Window[ctx->Roll.N % ROLLING_WINDOW] = c;
    ctx->Roll.N++;

    /* The original spamsum AND'ed this value with 0xFFFFFFFF which
     * in theory should have no effect. This AND has been removed
     * for performance (jk) */
    ctx->Roll.H3 <<= 5;
    ctx->Roll.H3 ^= c;
}

AARU_LOCAL void fuzzy_try_reduce_blockhash(spamsum_ctx* ctx)
{
    assert(ctx->Bhstart < ctx->Bhend);

    if(ctx->Bhend - ctx->Bhstart < 2) /* Need at least two working hashes. */
        return;

    if((uint64_t)SSDEEP_BS(ctx->Bhstart) * SPAMSUM_LENGTH >= ctx->TotalSize)
        /* Initial blocksize estimate would select this or a smaller
         * blocksize. */
        return;

    if(ctx->Bh[ctx->Bhstart + 1].Dlen < SPAMSUM_LENGTH / 2) /* Estimate adjustment would select this blocksize. */
        return;

    /* At this point we are clearly no longer interested in the
     * start_blocksize. Get rid of it. */
    ++ctx->Bhstart;
}

AARU_LOCAL void fuzzy_try_fork_blockhash(spamsum_ctx* ctx)
{
    if(ctx->Bhend >= NUM_BLOCKHASHES) return;

    assert(ctx->Bhend != 0);

    uint32_t obh            = ctx->Bhend - 1;
    uint32_t nbh            = ctx->Bhend;
    ctx->Bh[nbh].H          = ctx->Bh[obh].H;
    ctx->Bh[nbh].Halfh      = ctx->Bh[obh].Halfh;
    ctx->Bh[nbh].Digest[0]  = 0;
    ctx->Bh[nbh].Halfdigest = 0;
    ctx->Bh[nbh].Dlen       = 0;
    ++ctx->Bhend;
}

AARU_EXPORT uint8_t* AARU_CALL spamsum_final(spamsum_ctx* ctx)
{
    uint32_t bi     = ctx->Bhstart;
    uint32_t h      = roll_sum(ctx);
    int      remain = (int)(FUZZY_MAX_RESULT - 1); /* Exclude terminating '\0'. */
    uint8_t* result = (uint8_t*)malloc(FUZZY_MAX_RESULT);

    if(!result) return NULL;

    /* Verify that our elimination was not overeager. */
    assert(bi == 0 || (uint64_t)SSDEEP_BS(bi) / 2 * SPAMSUM_LENGTH < ctx->TotalSize);

    /* Initial blocksize guess. */
    while((uint64_t)SSDEEP_BS(bi) * SPAMSUM_LENGTH < ctx->TotalSize)
    {
        ++bi;

        if(bi >= NUM_BLOCKHASHES)
        {
            errno = EOVERFLOW;
            return NULL;
        }
    }

    /* Adapt blocksize guess to actual digest length. */
    while(bi >= ctx->Bhend) --bi;

    while(bi > ctx->Bhstart && ctx->Bh[bi].Dlen < SPAMSUM_LENGTH / 2) --bi;

    assert(!(bi > 0 && ctx->Bh[bi].Dlen < SPAMSUM_LENGTH / 2));

    int i = snprintf((char*)result, (size_t)remain, "%lu:", (unsigned long)SSDEEP_BS(bi));

    if(i <= 0) /* Maybe snprintf has set errno here? */
        return NULL;

    assert(i < remain);

    remain -= i;
    result += i;

    i = (int)ctx->Bh[bi].Dlen;

    assert(i <= remain);

    memcpy(result, ctx->Bh[bi].Digest, (size_t)i);
    result += i;
    remain -= i;

    if(h != 0)
    {
        assert(remain > 0);

        *result = _b64[ctx->Bh[bi].H % 64];

        if(i < 3 || *result != result[-1] || *result != result[-2] || *result != result[-3])
        {
            ++result;
            --remain;
        }
    }
    else if(ctx->Bh[bi].Digest[i] != 0)
    {
        assert(remain > 0);

        *result = ctx->Bh[bi].Digest[i];

        if(i < 3 || *result != result[-1] || *result != result[-2] || *result != result[-3])
        {
            ++result;
            --remain;
        }
    }

    assert(remain > 0);

    *result++ = ':';
    --remain;

    if(bi < ctx->Bhend - 1)
    {
        ++bi;
        i = (int)ctx->Bh[bi].Dlen;

        if(i <= remain)
            ;

        memcpy(result, ctx->Bh[bi].Digest, (size_t)i);
        result += i;
        remain -= i;

        if(h != 0)
        {
            assert(remain > 0);

            h       = ctx->Bh[bi].Halfh;
            *result = _b64[h % 64];

            if(i < 3 || *result != result[-1] || *result != result[-2] || *result != result[-3])
            {
                ++result;
                --remain;
            }
        }
        else
        {
            i = ctx->Bh[bi].Halfdigest;

            if(i != 0)
            {
                assert(remain > 0);

                *result = (uint8_t)i;

                if(i < 3 || *result != result[-1] || *result != result[-2] || *result != result[-3])
                {
                    ++result;
                    --remain;
                }
            }
        }
    }
    else if(h != 0)
    {
        assert(ctx->Bh[bi].Dlen == 0);

        assert(remain > 0);

        *result++ = _b64[ctx->Bh[bi].H % 64];
        /* No need to bother with FUZZY_FLAG_ELIMSEQ, because this
         * digest has length 1. */
        --remain;
    }

    *result = 0;

    return result;
}
