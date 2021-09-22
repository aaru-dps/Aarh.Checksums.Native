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

#define SPAMSUM_LENGTH 64
#define NUM_BLOCKHASHES  31
#define ROLLING_WINDOW   7
#define HASH_INIT         0x28021967
#define HASH_PRIME        0x01000193
#define MIN_BLOCKSIZE    3
#define FUZZY_MAX_RESULT  ((2 * SPAMSUM_LENGTH) + 20)

typedef        struct
{
    uint32_t   H;
    uint32_t   Halfh;
    uint8_t Digest[SPAMSUM_LENGTH];
    uint8_t Halfdigest;
    uint32_t Dlen;
} BlockhashContext;

typedef        struct
{
    uint8_t Window[ROLLING_WINDOW];
    uint32_t H1;
    uint32_t H2;
    uint32_t H3;
    uint32_t N;
}RollState;

typedef        struct
{
    uint32_t               Bhstart;
    uint32_t               Bhend;
    BlockhashContext Bh[NUM_BLOCKHASHES];
    uint64_t     TotalSize;
    RollState Roll;
} spamsum_ctx;

AARU_EXPORT spamsum_ctx* AARU_CALL spamsum_init(void);
AARU_EXPORT int AARU_CALL spamsum_update(spamsum_ctx* ctx, const uint8_t* data, uint32_t len);
AARU_EXPORT uint8_t* AARU_CALL spamsum_final(spamsum_ctx* ctx);
AARU_EXPORT void AARU_CALL spamsum_free(spamsum_ctx* ctx);

AARU_LOCAL void fuzzy_engine_step(spamsum_ctx* ctx, uint8_t c);
AARU_LOCAL void roll_hash(spamsum_ctx* ctx, uint8_t c);
AARU_LOCAL void fuzzy_try_reduce_blockhash(spamsum_ctx* ctx);
AARU_LOCAL void fuzzy_try_fork_blockhash(spamsum_ctx* ctx);

