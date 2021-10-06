//
// Created by claunia on 5/10/21.
//

#include <climits>
#include <cstdint>

#include "../library.h"
#include "../crc32.h"
#include "gtest/gtest.h"

#define EXPECTED_CRC32 0x2B6E6854
#define EXPECTED_CRC32_15BYTES 0xad6da727
#define EXPECTED_CRC32_31BYTES 0xa2ad2faa
#define EXPECTED_CRC32_63BYTES 0xbff6a341
#define EXPECTED_CRC32_2352BYTES 0x08ba93ea

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

TEST_F(crc32Fixture, crc32_slicing)
{
    uint32_t crc = CRC32_ISO_SEED;

    crc32_slicing(&crc, buffer, 1048576);

    crc ^= CRC32_ISO_SEED;

    EXPECT_EQ(crc, EXPECTED_CRC32);
}

TEST_F(crc32Fixture, crc32_auto_15bytes)
{
    crc32_ctx* ctx = crc32_init();
    uint32_t   crc;

    EXPECT_NE(ctx, nullptr);

    crc32_update(ctx, buffer, 15);
    crc32_final(ctx, &crc);

    EXPECT_EQ(crc, EXPECTED_CRC32_15BYTES);
}

TEST_F(crc32Fixture, crc32_slicing_15bytes)
{
    uint32_t crc = CRC32_ISO_SEED;

    crc32_slicing(&crc, buffer, 15);

    crc ^= CRC32_ISO_SEED;

    EXPECT_EQ(crc, EXPECTED_CRC32_15BYTES);
}

TEST_F(crc32Fixture, crc32_auto_31bytes)
{
    crc32_ctx* ctx = crc32_init();
    uint32_t   crc;

    EXPECT_NE(ctx, nullptr);

    crc32_update(ctx, buffer, 31);
    crc32_final(ctx, &crc);

    EXPECT_EQ(crc, EXPECTED_CRC32_31BYTES);
}

TEST_F(crc32Fixture, crc32_slicing_31bytes)
{
    uint32_t crc = CRC32_ISO_SEED;

    crc32_slicing(&crc, buffer, 31);

    crc ^= CRC32_ISO_SEED;

    EXPECT_EQ(crc, EXPECTED_CRC32_31BYTES);
}

TEST_F(crc32Fixture, crc32_auto_63bytes)
{
    crc32_ctx* ctx = crc32_init();
    uint32_t   crc;

    EXPECT_NE(ctx, nullptr);

    crc32_update(ctx, buffer, 63);
    crc32_final(ctx, &crc);

    EXPECT_EQ(crc, EXPECTED_CRC32_63BYTES);
}

TEST_F(crc32Fixture, crc32_slicing_63bytes)
{
    uint32_t crc = CRC32_ISO_SEED;

    crc32_slicing(&crc, buffer, 63);

    crc ^= CRC32_ISO_SEED;

    EXPECT_EQ(crc, EXPECTED_CRC32_63BYTES);
}

TEST_F(crc32Fixture, crc32_auto_2352bytes)
{
    crc32_ctx* ctx = crc32_init();
    uint32_t   crc;

    EXPECT_NE(ctx, nullptr);

    crc32_update(ctx, buffer, 2352);
    crc32_final(ctx, &crc);

    EXPECT_EQ(crc, EXPECTED_CRC32_2352BYTES);
}

TEST_F(crc32Fixture, crc32_slicing_2352bytes)
{
    uint32_t crc = CRC32_ISO_SEED;

    crc32_slicing(&crc, buffer, 2352);

    crc ^= CRC32_ISO_SEED;

    EXPECT_EQ(crc, EXPECTED_CRC32_2352BYTES);
}

#if defined(__x86_64__) || defined(__amd64) || defined(_M_AMD64) || defined(_M_X64) || defined(__I386__) ||            \
    defined(__i386__) || defined(__THW_INTEL) || defined(_M_IX86)
TEST_F(crc32Fixture, crc32_clmul)
{
    if(!have_clmul()) return;

    uint32_t crc = CRC32_ISO_SEED;

    crc = ~crc32_clmul(buffer, 1048576, ~crc);

    crc ^= CRC32_ISO_SEED;

    EXPECT_EQ(crc, EXPECTED_CRC32);
}

TEST_F(crc32Fixture, crc32_clmul_15bytes)
{
    if(!have_clmul()) return;

    uint32_t crc = CRC32_ISO_SEED;

    crc = ~crc32_clmul(buffer, 15, ~crc);

    crc ^= CRC32_ISO_SEED;

    EXPECT_EQ(crc, EXPECTED_CRC32_15BYTES);
}

TEST_F(crc32Fixture, crc32_clmul_31bytes)
{
    if(!have_clmul()) return;

    uint32_t crc = CRC32_ISO_SEED;

    crc = ~crc32_clmul(buffer, 31, ~crc);

    crc ^= CRC32_ISO_SEED;

    EXPECT_EQ(crc, EXPECTED_CRC32_31BYTES);
}

TEST_F(crc32Fixture, crc32_clmul_63bytes)
{
    if(!have_clmul()) return;

    uint32_t crc = CRC32_ISO_SEED;

    crc = ~crc32_clmul(buffer, 63, ~crc);

    crc ^= CRC32_ISO_SEED;

    EXPECT_EQ(crc, EXPECTED_CRC32_63BYTES);
}

TEST_F(crc32Fixture, crc32_clmul_2352bytes)
{
    if(!have_clmul()) return;

    uint32_t crc = CRC32_ISO_SEED;

    crc = ~crc32_clmul(buffer, 2352, ~crc);

    crc ^= CRC32_ISO_SEED;

    EXPECT_EQ(crc, EXPECTED_CRC32_2352BYTES);
}
#endif

#if defined(__aarch64__) || defined(_M_ARM64) || defined(__arm__) || defined(_M_ARM)
#if __ARM_ARCH >= 8
TEST_F(crc32Fixture, crc32_arm_crc32)
{
    if(!have_arm_crc32()) return;

    uint32_t crc = CRC32_ISO_SEED;

    crc = armv8_crc32_little(crc, buffer, 1048576);

    crc ^= CRC32_ISO_SEED;

    EXPECT_EQ(crc, EXPECTED_CRC32);
}

TEST_F(crc32Fixture, crc32_arm_crc32_15bytes)
{
    if(!have_arm_crc32()) return;

    uint32_t crc = CRC32_ISO_SEED;

    crc = armv8_crc32_little(crc, buffer, 15);

    crc ^= CRC32_ISO_SEED;

    EXPECT_EQ(crc, EXPECTED_CRC32_15BYTES);
}

TEST_F(crc32Fixture, crc32_arm_crc32_31bytes)
{
    if(!have_arm_crc32()) return;

    uint32_t crc = CRC32_ISO_SEED;

    crc = armv8_crc32_little(crc, buffer, 31);

    crc ^= CRC32_ISO_SEED;

    EXPECT_EQ(crc, EXPECTED_CRC32_31BYTES);
}

TEST_F(crc32Fixture, crc32_arm_crc32_63bytes)
{
    if(!have_arm_crc32()) return;

    uint32_t crc = CRC32_ISO_SEED;

    crc = armv8_crc32_little(crc, buffer, 63);

    crc ^= CRC32_ISO_SEED;

    EXPECT_EQ(crc, EXPECTED_CRC32_63BYTES);
}

TEST_F(crc32Fixture, crc32_arm_crc32_2352bytes)
{
    if(!have_arm_crc32()) return;

    uint32_t crc = CRC32_ISO_SEED;

    crc = armv8_crc32_little(crc, buffer, 2352);

    crc ^= CRC32_ISO_SEED;

    EXPECT_EQ(crc, EXPECTED_CRC32_2352BYTES);
}
#endif

TEST_F(crc32Fixture, crc32_vmull)
{
    if(!have_neon()) return;

    uint32_t crc = CRC32_ISO_SEED;

    crc = ~crc32_vmull(buffer, 1048576, ~crc);

    crc ^= CRC32_ISO_SEED;

    EXPECT_EQ(crc, EXPECTED_CRC32);
}

TEST_F(crc32Fixture, crc32_vmull_15bytes)
{
    if(!have_neon()) return;

    uint32_t crc = CRC32_ISO_SEED;

    crc = ~crc32_vmull(buffer, 15, ~crc);

    crc ^= CRC32_ISO_SEED;

    EXPECT_EQ(crc, EXPECTED_CRC32_15BYTES);
}

TEST_F(crc32Fixture, crc32_vmull_31bytes)
{
    if(!have_neon()) return;

    uint32_t crc = CRC32_ISO_SEED;

    crc = ~crc32_vmull(buffer, 31, ~crc);

    crc ^= CRC32_ISO_SEED;

    EXPECT_EQ(crc, EXPECTED_CRC32_31BYTES);
}

TEST_F(crc32Fixture, crc32_vmull_63bytes)
{
    if(!have_neon()) return;

    uint32_t crc = CRC32_ISO_SEED;

    crc = ~crc32_vmull(buffer, 63, ~crc);

    crc ^= CRC32_ISO_SEED;

    EXPECT_EQ(crc, EXPECTED_CRC32_63BYTES);
}

TEST_F(crc32Fixture, crc32_vmull_2352bytes)
{
    if(!have_neon()) return;

    uint32_t crc = CRC32_ISO_SEED;

    crc = ~crc32_vmull(buffer, 2352, ~crc);

    crc ^= CRC32_ISO_SEED;

    EXPECT_EQ(crc, EXPECTED_CRC32_2352BYTES);
}
#endif
