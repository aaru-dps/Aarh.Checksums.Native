#!/usr/bin/env bash

## Android (ARM)
# Detected system processor: armv7-a
rm -f CMakeCache.txt
mkdir -p runtimes/android-arm/native
docker run --rm dockcross/android-arm >docker/dockcross-android-arm
chmod +x docker/dockcross-android-arm
docker/dockcross-android-arm cmake -DCMAKE_BUILD_TYPE=Release
docker/dockcross-android-arm make
mv libAaru.Checksums.Native.so runtimes/android-arm/native/

## Android (ARM64)
# Detected system processor: aarch64
rm -f CMakeCache.txt
mkdir -p runtimes/android-arm64/native
docker run --rm dockcross/android-arm64 >docker/dockcross-android-arm64
chmod +x docker/dockcross-android-arm64
docker/dockcross-android-arm64 cmake -DCMAKE_BUILD_TYPE=Release
docker/dockcross-android-arm64 make
mv libAaru.Checksums.Native.so runtimes/android-arm64/native/

## Android (amd64)
# Detected system processor: x86_64
rm -f CMakeCache.txt
mkdir -p runtimes/android-x64/native
docker run --rm dockcross/android-x86_64 >docker/dockcross-android-x64
chmod +x docker/dockcross-android-x64
docker/dockcross-android-x64 cmake -DCMAKE_BUILD_TYPE=Release
docker/dockcross-android-x64 make
mv libAaru.Checksums.Native.so runtimes/android-x64/native/

## Android (x86)
# Detected system processor: i686
rm -f CMakeCache.txt
mkdir -p runtimes/android-x86/native
docker run --rm dockcross/android-x86 >docker/dockcross-android-x86
chmod +x docker/dockcross-android-x86
docker/dockcross-android-x86 cmake -DCMAKE_BUILD_TYPE=Release
docker/dockcross-android-x86 make
mv libAaru.Checksums.Native.so runtimes/android-x86/native/

## Linux (ARMv7-A)
# Detected system processor: arm
rm -f CMakeCache.txt
mkdir -p runtimes/linux-arm/native
docker run --rm dockcross/linux-armv7a >docker/dockcross-linux-arm
chmod +x docker/dockcross-linux-arm
docker/dockcross-linux-arm cmake -DCMAKE_BUILD_TYPE=Release
docker/dockcross-linux-arm make
mv libAaru.Checksums.Native.so runtimes/linux-arm/native/

## Linux (ARM64)
# Detected system processor: aarch64
rm -f CMakeCache.txt
mkdir -p runtimes/linux-arm64/native
docker run --rm dockcross/linux-arm64-lts >docker/dockcross-linux-arm64
chmod +x docker/dockcross-linux-arm64
docker/dockcross-linux-arm64 cmake -DCMAKE_BUILD_TYPE=Release
docker/dockcross-linux-arm64 make
mv libAaru.Checksums.Native.so runtimes/linux-arm64/native/

## Linux (MIPS64)
# Detected system processor: mips
rm -f CMakeCache.txt
mkdir -p runtimes/linux-mips64/native
docker run --rm dockcross/linux-mips >docker/dockcross-linux-mips64
chmod +x docker/dockcross-linux-mips64
docker/dockcross-linux-mips64 cmake -DCMAKE_BUILD_TYPE=Release
docker/dockcross-linux-mips64 make
mv libAaru.Checksums.Native.so runtimes/linux-mips64/native/

## Linux (ARM), musl
# Detected system processor: arm
rm -f CMakeCache.txt
mkdir -p runtimes/linux-musl-arm/native
docker run --rm dockcross/linux-armv7l-musl >docker/dockcross-linux-musl-arm
chmod +x docker/dockcross-linux-musl-arm
docker/dockcross-linux-musl-arm cmake -DCMAKE_BUILD_TYPE=Release
docker/dockcross-linux-musl-arm make
mv libAaru.Checksums.Native.so runtimes/linux-musl-arm/native/

## Linux (ARM64), musl
# Detected system processor: aarch64
rm -f CMakeCache.txt
mkdir -p runtimes/linux-musl-arm64/native
docker run --rm dockcross/linux-arm64-musl >docker/dockcross-linux-musl-arm64
chmod +x docker/dockcross-linux-musl-arm64
docker/dockcross-linux-musl-arm64 cmake -DCMAKE_BUILD_TYPE=Release
docker/dockcross-linux-musl-arm64 make
mv libAaru.Checksums.Native.so runtimes/linux-musl-arm64/native/

## Linux (s390x)
# Detected system processor: s390x
rm -f CMakeCache.txt
mkdir -p runtimes/linux-s390x/native
docker run --rm dockcross/linux-s390x >docker/dockcross-linux-s390x
chmod +x docker/dockcross-linux-s390x
docker/dockcross-linux-s390x cmake -DCMAKE_BUILD_TYPE=Release
docker/dockcross-linux-s390x make
mv libAaru.Checksums.Native.so runtimes/linux-s390x/native/

## Linux (amd64)
# Detected system processor: x86_64
rm -f CMakeCache.txt
mkdir -p runtimes/linux-x64/native
docker run --rm dockcross/linux-x64 >docker/dockcross-linux-x64
chmod +x docker/dockcross-linux-x64
docker/dockcross-linux-x64 cmake -DCMAKE_BUILD_TYPE=Release
docker/dockcross-linux-x64 make
mv libAaru.Checksums.Native.so runtimes/linux-x64/native/

## Linux (x86)
# Detected system processor: i686
# TODO: xmmintrin.h:258:1: error: inlining failed in call to 'always_inline' '_mm_xor_ps': target specific option mismatch
#rm -f CMakeCache.txt
#mkdir -p runtimes/linux-x86/native
#docker run --rm dockcross/linux-x86 > docker/dockcross-linux-x86
#chmod +x docker/dockcross-linux-x86
#docker/dockcross-linux-x86 cmake -DCMAKE_BUILD_TYPE=Release
#docker/dockcross-linux-x86 make
#mv libAaru.Checksums.Native.so runtimes/linux-x86/native/

## Windows (ARM)
# Detected system processor: arm
# TODO: clang-12: error: unsupported option '-fPIC' for target 'armv7-w64-windows-gnu'
#rm -f CMakeCache.txt
#mkdir -p runtimes/win-arm/native
#docker run --rm dockcross/windows-armv7 > docker/dockcross-win-arm
#chmod +x docker/dockcross-win-arm
#docker/dockcross-win-arm cmake -DCMAKE_BUILD_TYPE=Release
#docker/dockcross-win-arm make
#mv libAaru.Checksums.Native.dll runtimes/win-arm/native/

## Windows (ARM64)
# Detected system processor: aarch64
# TODO: clang-12: error: unsupported option '-fPIC' for target 'aarch64-w64-windows-gnu'
#rm -f CMakeCache.txt
#mkdir -p runtimes/win-arm64/native
#docker run --rm dockcross/windows-arm64 > docker/dockcross-win-arm64
#chmod +x docker/dockcross-win-arm64
#docker/dockcross-win-arm64 cmake -DCMAKE_BUILD_TYPE=Release
#docker/dockcross-win-arm64 make
#mv libAaru.Checksums.Native.dll runtimes/win-arm64/native/

## Windows (AMD64)
# Detected system processor: x86_64
# TODO: Requires MSVCRT.DLL
rm -f CMakeCache.txt
mkdir -p runtimes/win-x64/native
docker run --rm dockcross/windows-shared-x64 >docker/dockcross-win-x64
chmod +x docker/dockcross-win-x64
docker/dockcross-win-x64 cmake -DCMAKE_BUILD_TYPE=Release
docker/dockcross-win-x64 make
mv libAaru.Checksums.Native.dll runtimes/win-x64/native/

## Windows (x86)
# Detected system processor: i686
# TODO: Requires MSVCRT.DLL
# TODO: xmmintrin.h:258:1: error: inlining failed in call to 'always_inline' '_mm_xor_ps': target specific option mismatch
#rm -f CMakeCache.txt
#mkdir -p runtimes/win-x86/native
#docker run --rm dockcross/windows-shared-x86 > docker/dockcross-win-x86
#chmod +x docker/dockcross-win-x86
#docker/dockcross-win-x86 cmake -DCMAKE_BUILD_TYPE=Release
#docker/dockcross-win-x86 make
#mv libAaru.Checksums.Native.dll runtimes/win-x86/native/

# TODO: "linux-musl-x64"
# TODO: "linux-musl-x86"
# TODO: "osx-arm64"
# TODO: "osx-x64"
