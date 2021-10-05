//
// Created by claunia on 5/10/21.
//

#include <climits>
#include <cstdint>

#include "../library.h"
#include "../adler32.h"
#include "gtest/gtest.h"

#define EXPECTED_ADLER32 0x3728d186

static const uint8_t* buffer;

class adler32Fixture : public ::testing::Test
{
  public:
    adler32Fixture()
    {
        // initialization;
        // can also be done in SetUp()
    }

  protected:
    void SetUp()
    {
        char path[PATH_MAX];
        char filename[PATH_MAX];

        getcwd(path, PATH_MAX);
        snprintf(filename, PATH_MAX, "%s/data/random", path);

        FILE* file = fopen(filename, "rb");
        buffer     = (const uint8_t*)malloc(1048576);
        fread((void*)buffer, 1, 1048576, file);
        fclose(file);
    }

    void TearDown() { free((void*)buffer); }

    ~adler32Fixture()
    {
        // resources cleanup, no exceptions allowed
    }

    // shared user data
};

TEST_F(adler32Fixture, adler32_auto)
{
    adler32_ctx* ctx = adler32_init();
    uint32_t     adler32;

    EXPECT_NE(ctx, nullptr);

    adler32_update(ctx, buffer, 1048576);
    adler32_final(ctx, &adler32);

    EXPECT_EQ(adler32, EXPECTED_ADLER32);
}

TEST_F(adler32Fixture, adler32_slicing)
{
    uint16_t sum1;
    uint16_t sum2;
    uint32_t adler32;

    sum1 = 1;
    sum2 = 0;

    adler32_slicing(&sum1, &sum2, buffer, 1048576);

    adler32 = (sum2 << 16) | sum1;

    EXPECT_EQ(adler32, EXPECTED_ADLER32);
}

#if defined(__aarch64__) || defined(_M_ARM64) || defined(__arm__) || defined(_M_ARM)
TEST_F(adler32Fixture, adler32_neon)
{
    if(!have_neon()) return;

    uint16_t sum1;
    uint16_t sum2;
    uint32_t adler32;

    sum1 = 1;
    sum2 = 0;

    adler32_neon(&sum1, &sum2, buffer, 1048576);

    adler32 = (sum2 << 16) | sum1;

    EXPECT_EQ(adler32, EXPECTED_ADLER32);
}
#endif

#if defined(__x86_64__) || defined(__amd64) || defined(_M_AMD64) || defined(_M_X64) || defined(__I386__) ||            \
    defined(__i386__) || defined(__THW_INTEL) || defined(_M_IX86)

TEST_F(adler32Fixture, adler32_avx2)
{
    if(!have_avx2()) return;

    uint16_t sum1;
    uint16_t sum2;
    uint32_t adler32;

    sum1 = 1;
    sum2 = 0;

    adler32_avx2(&sum1, &sum2, buffer, 1048576);

    adler32 = (sum2 << 16) | sum1;

    EXPECT_EQ(adler32, EXPECTED_ADLER32);
}

TEST_F(adler32Fixture, adler32_ssse3)
{
    if(!have_ssse3()) return;

    uint16_t sum1;
    uint16_t sum2;
    uint32_t adler32;

    sum1 = 1;
    sum2 = 0;

    adler32_ssse3(&sum1, &sum2, buffer, 1048576);

    adler32 = (sum2 << 16) | sum1;

    EXPECT_EQ(adler32, EXPECTED_ADLER32);
}
#endif
