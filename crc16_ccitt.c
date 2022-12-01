/*
 * This file is part of the Aaru Data Preservation Suite.
 * Copyright (c) 2019-2023 Natalia Portillo.
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
#include "crc16_ccitt.h"

AARU_EXPORT crc16_ccitt_ctx* AARU_CALL crc16_ccitt_init(void)
{
    crc16_ccitt_ctx* ctx = (crc16_ccitt_ctx*)malloc(sizeof(crc16_ccitt_ctx));

    if(!ctx) return NULL;

    ctx->crc = 0x0000;

    return ctx;
}

AARU_EXPORT int AARU_CALL crc16_ccitt_update(crc16_ccitt_ctx* ctx, const uint8_t* data, uint32_t len)
{
    // Unroll according to Intel slicing by uint8_t
    // http://www.intel.com/technology/comms/perfnet/download/CRC_generators.pdf
    // http://sourceforge.net/projects/slicing-by-8/

    if(!ctx || !data) return -1;

    uint16_t       crc;
    const uint8_t* current_char     = (const uint8_t*)data;
    const size_t   unroll           = 4;
    const size_t   bytes_at_once    = 8 * unroll;
    uintptr_t      unaligned_length = (4 - (((uintptr_t)current_char) & 3)) & 3;

    crc = ctx->crc;

    while((len != 0) && (unaligned_length != 0))
    {
        crc = (crc << 8) ^ crc16_ccitt_table[0][(crc >> 8) ^ *current_char++];
        len--;
        unaligned_length--;
    }

    while(len >= bytes_at_once)
    {
        size_t unrolling;
        for(unrolling = 0; unrolling < unroll; unrolling++)
        {
            crc = crc16_ccitt_table[7][current_char[0] ^ (crc >> 8)] ^
                  crc16_ccitt_table[6][current_char[1] ^ (crc & 0xFF)] ^ crc16_ccitt_table[5][current_char[2]] ^
                  crc16_ccitt_table[4][current_char[3]] ^ crc16_ccitt_table[3][current_char[4]] ^
                  crc16_ccitt_table[2][current_char[5]] ^ crc16_ccitt_table[1][current_char[6]] ^
                  crc16_ccitt_table[0][current_char[7]];

            current_char += 8;
        }

        len -= bytes_at_once;
    }

    while(len-- != 0) crc = (crc << 8) ^ crc16_ccitt_table[0][(crc >> 8) ^ *current_char++];

    ctx->crc = crc;
    return 0;
}

AARU_EXPORT int AARU_CALL crc16_ccitt_final(crc16_ccitt_ctx* ctx, uint16_t* crc)
{
    if(!ctx) return -1;

    *crc = ~ctx->crc;

    return 0;
}

AARU_EXPORT void AARU_CALL crc16_ccitt_free(crc16_ccitt_ctx* ctx)
{
    if(ctx) free(ctx);
}