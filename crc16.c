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
#include "crc16.h"

AARU_EXPORT crc16_ctx* AARU_CALL crc16_init(void)
{
    crc16_ctx* ctx = (crc16_ctx*)malloc(sizeof(crc16_ctx));

    if(!ctx) return NULL;

    ctx->crc = 0x0000;

    return ctx;
}

AARU_EXPORT int AARU_CALL crc16_update(crc16_ctx* ctx, const uint8_t* data, uint32_t len)
{
    uint32_t i;
    if(!ctx || !data) return -1;

    for(i = 0; i < len; i++) ctx->crc = (ctx->crc >> 8) ^ crc16_table[data[i] ^ (ctx->crc & 0xFF)];

    return 0;
}

AARU_EXPORT int AARU_CALL crc16_final(crc16_ctx* ctx, uint16_t* crc)
{
    if(!ctx) return -1;

    *crc = ctx->crc;

    return 0;
}

AARU_EXPORT void AARU_CALL crc16_free(crc16_ctx* ctx)
{
    if(ctx) free(ctx);
}