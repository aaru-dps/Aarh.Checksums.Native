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

#ifndef AARU_CHECKSUMS_NATIVE_ADLER32_H
#define AARU_CHECKSUMS_NATIVE_ADLER32_H

#define ADLER_MODULE 65521
/* NMAX is the largest n such that 255n(n+1)/2 + (n+1)(ADLER_MODULE-1) <= 2^32-1 */
#define NMAX 5552

typedef struct
{
    uint16_t sum1;
    uint16_t sum2;
} adler32_ctx;

AARU_EXPORT adler32_ctx* AARU_CALL adler32_init();
AARU_EXPORT int AARU_CALL          adler32_update(adler32_ctx* ctx, const uint8_t* data, uint32_t len);
AARU_EXPORT int AARU_CALL          adler32_final(adler32_ctx* ctx, uint32_t* checksum);
AARU_EXPORT void AARU_CALL         adler32_free(adler32_ctx* ctx);

#endif // AARU_CHECKSUMS_NATIVE_ADLER32_H
