//
// Created by claunia on 5/10/21.
//

#include <climits>
#include <cstdint>

#include "../library.h"
#include "../fletcher32.h"
#include "gtest/gtest.h"

#define EXPECTED_FLETCHER32 0x211261f5

static const uint8_t* buffer;

class fletcher32Fixture : public ::testing::Test
{
  public:
    fletcher32Fixture()
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

    ~fletcher32Fixture()
    {
        // resources cleanup, no exceptions allowed
    }

    // shared user data
};

TEST_F(fletcher32Fixture, fletcher32_auto)
{
    fletcher32_ctx* ctx = fletcher32_init();
    uint32_t        fletcher;

    EXPECT_NE(ctx, nullptr);

    fletcher32_update(ctx, buffer, 1048576);
    fletcher32_final(ctx, &fletcher);

    EXPECT_EQ(fletcher, EXPECTED_FLETCHER32);
}
