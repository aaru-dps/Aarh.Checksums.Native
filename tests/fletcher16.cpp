//
// Created by claunia on 5/10/21.
//

#include <climits>
#include <cstdint>
#include <cstring>

#include "../library.h"
#include "../fletcher16.h"
#include "gtest/gtest.h"

#define EXPECTED_FLETCHER16 0x3357
#define EXPECTED_FLETCHER16_15BYTES 0x0282
#define EXPECTED_FLETCHER16_31BYTES 0xABB7
#define EXPECTED_FLETCHER16_63BYTES 0x1CA0
#define EXPECTED_FLETCHER16_2352BYTES 0x0AC5

static const uint8_t* buffer;
static const uint8_t* buffer_misaligned;

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

TEST_F(fletcher16Fixture, fletcher16_auto_misaligned)
{
    fletcher16_ctx* ctx = fletcher16_init();
    uint16_t        fletcher;

    EXPECT_NE(ctx, nullptr);

    fletcher16_update(ctx, buffer_misaligned+1, 1048576);
    fletcher16_final(ctx, &fletcher);

    EXPECT_EQ(fletcher, EXPECTED_FLETCHER16);
}

TEST_F(fletcher16Fixture, fletcher16_auto_15bytes)
{
    fletcher16_ctx* ctx = fletcher16_init();
    uint16_t        fletcher;

    EXPECT_NE(ctx, nullptr);

    fletcher16_update(ctx, buffer, 15);
    fletcher16_final(ctx, &fletcher);

    EXPECT_EQ(fletcher, EXPECTED_FLETCHER16_15BYTES);
}

TEST_F(fletcher16Fixture, fletcher16_auto_31bytes)
{
    fletcher16_ctx* ctx = fletcher16_init();
    uint16_t        fletcher;

    EXPECT_NE(ctx, nullptr);

    fletcher16_update(ctx, buffer, 31);
    fletcher16_final(ctx, &fletcher);

    EXPECT_EQ(fletcher, EXPECTED_FLETCHER16_31BYTES);
}

TEST_F(fletcher16Fixture, fletcher16_auto_63bytes)
{
    fletcher16_ctx* ctx = fletcher16_init();
    uint16_t        fletcher;

    EXPECT_NE(ctx, nullptr);

    fletcher16_update(ctx, buffer, 63);
    fletcher16_final(ctx, &fletcher);

    EXPECT_EQ(fletcher, EXPECTED_FLETCHER16_63BYTES);
}

TEST_F(fletcher16Fixture, fletcher16_auto_2352bytes)
{
    fletcher16_ctx* ctx = fletcher16_init();
    uint16_t        fletcher;

    EXPECT_NE(ctx, nullptr);

    fletcher16_update(ctx, buffer, 2352);
    fletcher16_final(ctx, &fletcher);

    EXPECT_EQ(fletcher, EXPECTED_FLETCHER16_2352BYTES);
}
