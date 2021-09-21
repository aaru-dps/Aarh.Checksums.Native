cmake_minimum_required(VERSION 3.20)
project("Aaru.Checksums.Native" C)

set(CMAKE_C_STANDARD 90)

add_library("Aaru.Checksums.Native" SHARED library.c)
