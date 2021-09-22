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
#include "fletcher16.h"

AARU_EXPORT fletcher16_ctx* AARU_CALL fletcher16_init()
{
    fletcher16_ctx* ctx;

    ctx = (fletcher16_ctx*)malloc(sizeof(fletcher16_ctx));

    if(!ctx) return NULL;

    ctx->sum1 = 0xFF;
    ctx->sum2 = 0xFF;

    return ctx;
}

AARU_EXPORT int AARU_CALL fletcher16_update(fletcher16_ctx* ctx, const uint8_t* data, uint32_t len)
{
    uint32_t i;
    if(!ctx || !data) return -1;

    for(i = 0; i < len; i++)
    {
        ctx->sum1 = (ctx->sum1 + data[i]) % FLETCHER16_MODULE;
        ctx->sum2 = (ctx->sum2 + ctx->sum1) % FLETCHER16_MODULE;
    }

    return 0;
}

AARU_EXPORT int AARU_CALL fletcher16_final(fletcher16_ctx* ctx, uint16_t* checksum)
{
    if(!ctx) return -1;

    *checksum = (ctx->sum2 << 8) | ctx->sum1;
    return 0;
}

AARU_EXPORT void AARU_CALL fletcher16_free(fletcher16_ctx* ctx)
{
    if(!ctx) return;

    free(ctx);
}