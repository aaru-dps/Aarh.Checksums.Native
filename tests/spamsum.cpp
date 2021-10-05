//
// Created by claunia on 5/10/21.
//

#include <climits>
#include <cstdint>

#include "../library.h"
#include "../spamsum.h"
#include "gtest/gtest.h"

#define EXPECTED_SPAMSUM "24576:3dvzuAsHTQ16pc7O1Q/gS9qze+Swwn9s6IX:8/TQQpaVqze+JN6IX"

static const uint8_t* buffer;

class spamsumFixture : public ::testing::Test
{
  public:
    spamsumFixture()
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

    ~spamsumFixture()
    {
        // resources cleanup, no exceptions allowed
    }

    // shared user data
};

TEST_F(spamsumFixture, spamsum_auto)
{
    spamsum_ctx* ctx     = spamsum_init();
    const char*  spamsum = (const char*)malloc(FUZZY_MAX_RESULT);

    EXPECT_NE(ctx, nullptr);
    EXPECT_NE(spamsum, nullptr);

    spamsum_update(ctx, buffer, 1048576);
    spamsum_final(ctx, (uint8_t*)spamsum);

    EXPECT_STREQ(spamsum, EXPECTED_SPAMSUM);
}
