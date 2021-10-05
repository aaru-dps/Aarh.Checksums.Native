//
// Created by claunia on 5/10/21.
//

#include <cstdint>

#include "../library.h"
#include "../crc16.h"
#include "gtest/gtest.h"

#define EXPECTED_CRC16 0x2d6d

static const uint8_t* buffer;

class crc16Fixture : public ::testing::Test
{
  public:
    crc16Fixture()
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

    ~crc16Fixture()
    {
        // resources cleanup, no exceptions allowed
    }

    // shared user data
};

TEST_F(crc16Fixture, crc16_auto)
{
    crc16_ctx* ctx = crc16_init();
    uint16_t   crc;

    EXPECT_NE(ctx, nullptr);

    crc16_update(ctx, buffer, 1048576);
    crc16_final(ctx, &crc);

    EXPECT_EQ(crc, EXPECTED_CRC16);
}
