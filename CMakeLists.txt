cmake_minimum_required(VERSION 3.15)
project("Aaru.Checksums.Native" C)

set(CMAKE_C_STANDARD 90)

message("Detected system processor: ${CMAKE_SYSTEM_PROCESSOR}")
message("Detected compiler: ${CMAKE_C_COMPILER_ID}")
message("Detected build type: ${CMAKE_BUILD_TYPE}")

if("${CMAKE_BUILD_TYPE}" MATCHES "Release")
    if("${CMAKE_C_COMPILER_ID}" MATCHES "MSVC")
        add_compile_options("/O2" "/fp:fast")
        if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64" OR ${CMAKE_SYSTEM_PROCESSOR} MATCHES "i686" OR ${CMAKE_SYSTEM_PROCESSOR} MATCHES "AMD64")
            add_compile_options("/arch:SSE2")
        endif()
    else()
        add_compile_options(-ffast-math -O3)

        if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64" OR ${CMAKE_SYSTEM_PROCESSOR} MATCHES "i686" OR ${CMAKE_SYSTEM_PROCESSOR} MATCHES "AMD64")
            add_compile_options(-march=core2 -mfpmath=sse -msse3 -mtune=westmere -flto)
        elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64")
            add_compile_options(-march=armv8-a -flto)
        elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "armv7l")
            add_compile_options(-march=armv7)
        endif()
    endif()
endif()

add_library("Aaru.Checksums.Native" SHARED adler32.h adler32.c crc16.h crc16.c crc16_ccitt.h crc16_ccitt.c crc32.c crc32.h crc64.c crc64.h fletcher16.h fletcher16.c fletcher32.h fletcher32.c library.h spamsum.c spamsum.h crc32_clmul.c crc64_clmul.c simd.c simd.h adler32_ssse3.c adler32_avx2.c adler32_neon.c crc32_arm_simd.c crc32_vmull.c crc32_simd.h)

add_subdirectory(tests)