//
// Created by claunia on 5/10/21.
//

#include <climits>
#include <cstdint>
#include <cstring>

#include "../library.h"
#include "../fletcher32.h"
#include "gtest/gtest.h"

#define EXPECTED_FLETCHER32 0x211261f5
#define EXPECTED_FLETCHER32_15BYTES 0x34CD067C
#define EXPECTED_FLETCHER32_31BYTES 0xD8D20EA9
#define EXPECTED_FLETCHER32_63BYTES 0xD8432080
#define EXPECTED_FLETCHER32_2352BYTES 0xCB3E7352

static const uint8_t* buffer;
static const uint8_t* buffer_misaligned;

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

        buffer_misaligned = (const uint8_t*)malloc(1048577);
        memcpy((void*)(buffer_misaligned + 1), buffer, 1048576);
    }

    void TearDown() {
        free((void*)buffer);
        free((void*)buffer_misaligned);
    }

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

TEST_F(fletcher32Fixture, fletcher32_auto_misaligned)
{
    fletcher32_ctx* ctx = fletcher32_init();
    uint32_t        fletcher;

    EXPECT_NE(ctx, nullptr);

    fletcher32_update(ctx, buffer_misaligned+1, 1048576);
    fletcher32_final(ctx, &fletcher);

    EXPECT_EQ(fletcher, EXPECTED_FLETCHER32);
}

TEST_F(fletcher32Fixture, fletcher32_auto_15bytes)
{
    fletcher32_ctx* ctx = fletcher32_init();
    uint32_t        fletcher;

    EXPECT_NE(ctx, nullptr);

    fletcher32_update(ctx, buffer, 15);
    fletcher32_final(ctx, &fletcher);

    EXPECT_EQ(fletcher, EXPECTED_FLETCHER32_15BYTES);
}

TEST_F(fletcher32Fixture, fletcher32_auto_31bytes)
{
    fletcher32_ctx* ctx = fletcher32_init();
    uint32_t        fletcher;

    EXPECT_NE(ctx, nullptr);

    fletcher32_update(ctx, buffer, 31);
    fletcher32_final(ctx, &fletcher);

    EXPECT_EQ(fletcher, EXPECTED_FLETCHER32_31BYTES);
}

TEST_F(fletcher32Fixture, fletcher32_auto_63bytes)
{
    fletcher32_ctx* ctx = fletcher32_init();
    uint32_t        fletcher;

    EXPECT_NE(ctx, nullptr);

    fletcher32_update(ctx, buffer, 63);
    fletcher32_final(ctx, &fletcher);

    EXPECT_EQ(fletcher, EXPECTED_FLETCHER32_63BYTES);
}

TEST_F(fletcher32Fixture, fletcher32_auto_2352bytes)
{
    fletcher32_ctx* ctx = fletcher32_init();
    uint32_t        fletcher;

    EXPECT_NE(ctx, nullptr);

    fletcher32_update(ctx, buffer, 2352);
    fletcher32_final(ctx, &fletcher);

    EXPECT_EQ(fletcher, EXPECTED_FLETCHER32_2352BYTES);
}
