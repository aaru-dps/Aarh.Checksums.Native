#if defined(__x86_64__) || defined(__amd64) || defined(_M_AMD64) || defined(_M_X64) || defined(__I386__) ||            \
    defined(__i386__) || defined(__THW_INTEL) || defined(_M_IX86)

#ifdef _MSC_VER
#include <intrin.h>
#else
/*
 * Newer versions of GCC and clang come with cpuid.h
 * (ftr GCC 4.7 in Debian Wheezy has this)
 */
#include <cpuid.h>

#endif

static void cpuid(int info, unsigned* eax, unsigned* ebx, unsigned* ecx, unsigned* edx)
{
#ifdef _MSC_VER
    unsigned int registers[4];
    __cpuid(registers, info);
    *eax = registers[0];
    *ebx = registers[1];
    *ecx = registers[2];
    *edx = registers[3];
#else
    /* GCC, clang */
    unsigned int _eax;
    unsigned int _ebx;
    unsigned int _ecx;
    unsigned int _edx;
    __cpuid(info, _eax, _ebx, _ecx, _edx);
    *eax = _eax;
    *ebx = _ebx;
    *ecx = _ecx;
    *edx = _edx;
#endif
}

int have_clmul(void)
{
    unsigned eax, ebx, ecx, edx;
    int      has_pclmulqdq;
    int      has_sse41;
    cpuid(1 /* feature bits */, &eax, &ebx, &ecx, &edx);

    has_pclmulqdq = ecx & 0x2;     /* bit 1 */
    has_sse41     = ecx & 0x80000; /* bit 19 */

    return has_pclmulqdq && has_sse41;
}

#endif