#include "enumerate.hpp"
#include <array>
#include <catch2/catch_test_macros.hpp>
#include <vector>

TEST_CASE("enumerate")
{
    SECTION("const auto&, non-const vector")
    {
        const int arrSize = 10;
        std::vector<int> vec(arrSize);
        size_t expectedIndex = 0;
        for (const auto& [index, value] : enumerate(vec))
        {
            // value = 42; //should not compile
            REQUIRE(index == expectedIndex);
            expectedIndex++;
        }
    }

    SECTION("const auto&, const array")
    {
        const std::array<int, 10> arr{};
        size_t expectedIndex = 0;
        for (const auto& [index, value] : enumerate(arr))
        {
            REQUIRE(index == expectedIndex);
            expectedIndex++;
        }
    }

    SECTION("auto&, non-const array")
    {
        const int arrSize = 10;
        std::array<int, arrSize> arr{};
        arr.fill(0);
        for (auto& [index, value] : enumerate(arr))
        {
            // index = 42; // should not compile
            value = static_cast<int>(index);
        }
        for (const auto [index, value] : enumerate(arr))
        {
            REQUIRE(value == index);
        }
    }
}