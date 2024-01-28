#include <ien/svo_vector.hpp>

#include <catch2/catch_test_macros.hpp>

struct data_t
{
    int a,b,c;
    std::string x;
};

TEST_CASE("SVO Vector")
{
    SECTION("Construction works")
    {
        ien::svo_vector<float> x;
        ien::svo_vector<int> y;
        ien::svo_vector<std::string> z;
        ien::svo_vector<data_t> w;
    };

    SECTION("push_back()")
    {
        ien::svo_vector<int, sizeof(int) * 4> data;
        data.push_back(1);
        data.push_back(2);
        data.push_back(3);
        data.push_back(4);

        REQUIRE(!data.is_heap_allocated());
        REQUIRE(data[0] == 1);
        REQUIRE(data[1] == 2);
        REQUIRE(data[2] == 3);
        REQUIRE(data[3] == 4);
        
        data.push_back(5);
        REQUIRE(data.is_heap_allocated());
        REQUIRE(data[0] == 1);
        REQUIRE(data[1] == 2);
        REQUIRE(data[2] == 3);
        REQUIRE(data[3] == 4);
        REQUIRE(data[4] == 5);
    };
};