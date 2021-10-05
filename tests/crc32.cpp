//
// Created by claunia on 5/10/21.
//

#include <cstdint>

#include "../library.h"
#include "../crc32.h"
#include "gtest/gtest.h"

#define EXPECTED_CRC32 0x2B6E6854

static const uint8_t* buffer;

class crc32Fixture : public ::testing::Test
{
  public:
    crc32Fixture()
    {
        // initialization;
        // can also be done in SetUp()
    }

  protected:
    void SetUp()
    {
        FILE* file = fopen("/home/claunia/random", "rb");
        buffer     = (const uint8_t*)malloc(1048576);
        fread((void*)buffer, 1, 1048576, file);
        fclose(file);
    }

    void TearDown() { free((void*)buffer); }

    ~crc32Fixture()
    {
        // resources cleanup, no exceptions allowed
    }

    // shared user data
};

TEST_F(crc32Fixture, crc32_auto)
{
    crc32_ctx* ctx = crc32_init();
    uint32_t   crc;

    EXPECT_NE(ctx, nullptr);

    crc32_update(ctx, buffer, 1048576);
    crc32_final(ctx, &crc);

    EXPECT_EQ(crc, EXPECTED_CRC32);
}
