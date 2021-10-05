//
// Created by claunia on 5/10/21.
//

#include <cstdint>

#include "../crc16_ccitt.h"
#include "../library.h"
#include "gtest/gtest.h"

#define EXPECTED_CRC16_CCITT 0x3640

static const uint8_t* buffer;

class crc16_ccittFixture : public ::testing::Test
{
  public:
    crc16_ccittFixture()
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

    ~crc16_ccittFixture()
    {
        // resources cleanup, no exceptions allowed
    }

    // shared user data
};

TEST_F(crc16_ccittFixture, crc16_ccitt_auto)
{
    crc16_ccitt_ctx* ctx = crc16_ccitt_init();
    uint16_t         crc;

    EXPECT_NE(ctx, nullptr);

    crc16_ccitt_update(ctx, buffer, 1048576);
    crc16_ccitt_final(ctx, &crc);

    EXPECT_EQ(crc, EXPECTED_CRC16_CCITT);
}
