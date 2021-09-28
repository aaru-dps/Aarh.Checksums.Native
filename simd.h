#if defined(__x86_64__) || defined(__amd64) || defined(_M_AMD64) || defined(_M_X64) || defined(__I386__) ||            \
    defined(__i386__) || defined(__THW_INTEL) || defined(_M_IX86)

#define AVX2 __attribute__((target("avx2")))

int have_clmul(void);
int have_ssse3(void);
int have_avx2(void);
#endif
