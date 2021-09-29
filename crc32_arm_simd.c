//
// Created by claunia on 29/9/21.
//

#if defined(__aarch64__) || defined(_M_ARM64) || defined(__arm__) || defined(_M_ARM)

#include <arm_acle.h>

#include "library.h"
#include "crc32.h"

TARGET_ARMV8_WITH_CRC uint32_t armv8_crc32_little(uint32_t crc, const unsigned char* buf, uint32_t len)
{
    uint32_t c = (uint32_t)~crc;

#if defined(__aarch64__) || defined(_M_ARM64)
    while(len && ((uintptr_t)buf & 7))
    {
        c = __crc32b(c, *buf++);
        --len;
    }
    const uint64_t* buf8 = (const uint64_t*)buf;
    while(len >= 64)
    {
        c = __crc32d(c, *buf8++);
        c = __crc32d(c, *buf8++);
        c = __crc32d(c, *buf8++);
        c = __crc32d(c, *buf8++);
        c = __crc32d(c, *buf8++);
        c = __crc32d(c, *buf8++);
        c = __crc32d(c, *buf8++);
        c = __crc32d(c, *buf8++);
        len -= 64;
    }
    while(len >= 8)
    {
        c = __crc32d(c, *buf8++);
        len -= 8;
    }

    buf = (const unsigned char*)buf8;
#else // AARCH64
    while(len && ((uintptr_t)buf & 3))
    {
        c = __crc32b(c, *buf++);
        --len;
    }
    const uint32_t* buf4 = (const uint32_t*)buf;
    while(len >= 32)
    {
        c = __crc32w(c, *buf4++);
        c = __crc32w(c, *buf4++);
        c = __crc32w(c, *buf4++);
        c = __crc32w(c, *buf4++);
        c = __crc32w(c, *buf4++);
        c = __crc32w(c, *buf4++);
        c = __crc32w(c, *buf4++);
        c = __crc32w(c, *buf4++);
        len -= 32;
    }
    while(len >= 4)
    {
        c = __crc32d(c, *buf4++);
        len -= 4;
    }

    buf = (const unsigned char*)buf4;
#endif

    while(len--) { c = __crc32b(c, *buf++); }
    return ~c;
}
#endif
