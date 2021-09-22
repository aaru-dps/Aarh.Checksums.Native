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

#include <stdint.h>
#include <stdlib.h>

#include "library.h"
#include "crc64.h"

AARU_EXPORT crc64_ctx* AARU_CALL crc64_init(void)
{
    crc64_ctx* ctx = (crc64_ctx*)malloc(sizeof(crc64_ctx));

    if(!ctx) return NULL;

    ctx->crc = CRC64_ECMA_SEED;

    for(int i = 0; i < 256; i++)
    {
        uint64_t entry = (uint64_t)i;

        for(int j = 0; j < 8; j++)
            if(entry & 1) ctx->table[i] = (entry >> 1) ^ CRC64_ECMA_POLY;
            else
                ctx->table[i] >>= 1;
    }

    return ctx;
}

AARU_EXPORT int AARU_CALL crc64_update(crc64_ctx* ctx, const uint8_t* data, uint32_t len)
{
    if(!ctx || !data) return -1;

    for(uint32_t i = 0; i < len; i++) ctx->crc = (ctx->crc >> 8) ^ ctx->table[data[i] ^ (ctx->crc & 0xff)];

    return 0;
}

AARU_EXPORT int AARU_CALL crc64_final(crc64_ctx* ctx, uint64_t* crc)
{
    if(!ctx) return -1;

    *crc = ctx->crc ^ CRC64_ECMA_SEED;

    return 0;
}

AARU_EXPORT void AARU_CALL crc64_free(crc64_ctx* ctx)
{
    if(ctx) free(ctx);
}