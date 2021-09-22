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
    uint32_t crc;
    uint32_t table[256];
} crc32_ctx;

#define CRC32_ISO_POLY 0xEDB88320
#define CRC32_ISO_SEED 0xFFFFFFFF

AARU_EXPORT crc32_ctx* AARU_CALL crc32_init();
AARU_EXPORT int AARU_CALL        crc32_update(crc32_ctx* ctx, const uint8_t* data, uint32_t len);
AARU_EXPORT int AARU_CALL        crc32_final(crc32_ctx* ctx, uint32_t* crc);
AARU_EXPORT void AARU_CALL       crc32_free(crc32_ctx* ctx);