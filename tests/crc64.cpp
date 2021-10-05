//
// Created by claunia on 5/10/21.
//

#include <climits>
#include <cstdint>

#include "../library.h"
#include "../crc64.h"
#include "gtest/gtest.h"

#define EXPECTED_CRC64 0xbf09992cc5ede38e

static const uint8_t* buffer;

class crc64Fixture : public ::testing::Test
{
  public:
    crc64Fixture()
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

    ~crc64Fixture()
    {
        // resources cleanup, no exceptions allowed
    }

    // shared user data
};

TEST_F(crc64Fixture, crc64_auto)
{
    crc64_ctx* ctx = crc64_init();
    uint64_t   crc;

    EXPECT_NE(ctx, nullptr);

    crc64_update(ctx, buffer, 1048576);
    crc64_final(ctx, &crc);

    EXPECT_EQ(crc, EXPECTED_CRC64);
}

TEST_F(crc64Fixture, crc64_slicing)
{
    uint64_t crc = CRC64_ECMA_SEED;

    crc64_slicing(&crc, buffer, 1048576);

    crc ^= CRC64_ECMA_SEED;

    EXPECT_EQ(crc, EXPECTED_CRC64);
}

#if defined(__x86_64__) || defined(__amd64) || defined(_M_AMD64) || defined(_M_X64) || defined(__I386__) ||            \
    defined(__i386__) || defined(__THW_INTEL) || defined(_M_IX86)
TEST_F(crc64Fixture, crc64_clmul)
{
    if(!have_clmul()) return;

    uint64_t crc = CRC64_ECMA_SEED;

    crc = ~crc64_clmul(~crc, buffer, 1048576);

    crc ^= CRC64_ECMA_SEED;

    EXPECT_EQ(crc, EXPECTED_CRC64);
}
#endif
