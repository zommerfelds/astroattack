#include <gtest/gtest.h>

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

    // run all the tests
    return RUN_ALL_TESTS();
}
