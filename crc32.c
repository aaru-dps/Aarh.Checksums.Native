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
#include "crc32.h"

AARU_EXPORT crc32_ctx* AARU_CALL crc32_init(void)
{
    crc32_ctx* ctx = (crc32_ctx*)malloc(sizeof(crc32_ctx));

    if(!ctx) return NULL;

    ctx->crc = CRC32_ISO_SEED;

    return ctx;
}

AARU_EXPORT int AARU_CALL crc32_update(crc32_ctx* ctx, const uint8_t* data, uint32_t len)
{
    uint32_t i;
    if(!ctx || !data) return -1;

    for(i = 0; i < len; i++) ctx->crc = (ctx->crc >> 8) ^ crc32_table[data[i] ^ (ctx->crc & 0xff)];

    return 0;
}

AARU_EXPORT int AARU_CALL crc32_final(crc32_ctx* ctx, uint32_t* crc)
{
    if(!ctx) return -1;

    *crc = ctx->crc ^ CRC32_ISO_SEED;

    return 0;
}

AARU_EXPORT void AARU_CALL crc32_free(crc32_ctx* ctx)
{
    if(ctx) free(ctx);
}