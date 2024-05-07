#include <ien/initializers.hpp>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("initialize_array_with_generator")
{
    constexpr auto arr = ien::initialize_array_with_generator<int, 5>([] { return std::numeric_limits<int>::max(); });
    REQUIRE(
        arr == std::array<int, 5>{ std::numeric_limits<int>::max(),
                                   std::numeric_limits<int>::max(),
                                   std::numeric_limits<int>::max(),
                                   std::numeric_limits<int>::max(),
                                   std::numeric_limits<int>::max() }
    );

    const auto vgenerator = [] {
        std::vector<int> v;
        for (int i = 0; i < 5; ++i)
        {
            v.push_back(i);
        }
        return v;
    };

    auto arr2 = ien::initialize_array_with_generator<std::vector<int>, 6>(vgenerator);
    REQUIRE(arr2.size() == 6);
    for (const auto& v : arr2)
    {
        REQUIRE(v.size() == 5);
        REQUIRE(v == std::vector<int>{ 0, 1, 2, 3, 4 });
    }
};