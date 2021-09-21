cmake_minimum_required(VERSION 3.20)
project("Aaru.Checksums.Native" C)

set(CMAKE_C_STANDARD 90)

add_library("Aaru.Checksums.Native" SHARED adler32.h adler32.c crc16.h crc16.c crc16_ccitt.h crc16_ccitt.c library.h)
