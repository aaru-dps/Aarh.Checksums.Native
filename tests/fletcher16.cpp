//
// Created by claunia on 5/10/21.
//

#include <cstdint>

#include "../library.h"
#include "../fletcher16.h"
#include "gtest/gtest.h"

#define EXPECTED_FLETCHER16 0x3357

static const uint8_t* buffer;

class fletcher16Fixture : public ::testing::Test
{
  public:
    fletcher16Fixture()
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

    ~fletcher16Fixture()
    {
        // resources cleanup, no exceptions allowed
    }

    // shared user data
};

TEST_F(fletcher16Fixture, fletcher16_auto)
{
    fletcher16_ctx* ctx = fletcher16_init();
    uint16_t        fletcher;

    EXPECT_NE(ctx, nullptr);

    fletcher16_update(ctx, buffer, 1048576);
    fletcher16_final(ctx, &fletcher);

    EXPECT_EQ(fletcher, EXPECTED_FLETCHER16);
}
