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

typedef struct
{
    uint64_t crc;
    uint64_t table[256];
} crc64_ctx;

#define CRC64_ECMA_POLY 0xC96C5795D7870F42
#define CRC64_ECMA_SEED 0xFFFFFFFFFFFFFFFF

AARU_EXPORT crc64_ctx* AARU_CALL crc64_init();
AARU_EXPORT int AARU_CALL        crc64_update(crc64_ctx* ctx, const uint8_t* data, uint32_t len);
AARU_EXPORT int AARU_CALL        crc64_final(crc64_ctx* ctx, uint64_t* crc);
AARU_EXPORT void AARU_CALL       crc64_free(crc64_ctx* ctx);