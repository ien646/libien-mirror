#include <ien/svo_vector.hpp>

#include <catch2/catch_test_macros.hpp>

struct data_t
{
    int a, b, c;
    std::string x;
};

size_t alloc_count = 0;
size_t alloc_bytes = 0;
size_t dealloc_count = 0;

void* operator new(size_t sz)
{
    ++alloc_count;
    alloc_bytes += sz;
    return malloc(sz);
}

void operator delete(void* ptr) noexcept
{
    ++dealloc_count;
    free(ptr);
}

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
        const int initial_allocs = alloc_count;

        ien::svo_vector<int, sizeof(int) * 4> data;
        data.push_back(1);
        data.push_back(2);
        data.push_back(3);
        data.push_back(4);

        REQUIRE(alloc_count == initial_allocs);
        REQUIRE(!data.is_heap_allocated());
        REQUIRE(data[0] == 1);
        REQUIRE(data[1] == 2);
        REQUIRE(data[2] == 3);
        REQUIRE(data[3] == 4);

        data.push_back(5);
        REQUIRE(alloc_count == initial_allocs + 1);
        REQUIRE(data.is_heap_allocated());
        REQUIRE(data[0] == 1);
        REQUIRE(data[1] == 2);
        REQUIRE(data[2] == 3);
        REQUIRE(data[3] == 4);
        REQUIRE(data[4] == 5);
    };

    SECTION("orphan_vector")
    {
        // Stack allocated svo_vector<T> gets coerced into heap allocated std::vector<T>
        {
            const int initial_allocs = alloc_count;

            ien::svo_vector<int, sizeof(int) * 4> data;
            data.push_back(1);
            data.push_back(2);
            data.push_back(3);
            data.push_back(4);

            REQUIRE(alloc_count == initial_allocs);
            std::vector<int> orphan = data.orphan_vector();
            REQUIRE(alloc_count == initial_allocs + 1);
            REQUIRE(orphan[0] == 1);
            REQUIRE(orphan[1] == 2);
            REQUIRE(orphan[2] == 3);
            REQUIRE(orphan[3] == 4);
        }
        {
            const int initial_allocs = alloc_count;
            ien::svo_vector<int, sizeof(int) * 4> data;
            data.push_back(1);
            data.push_back(2);
            data.push_back(3);
            data.push_back(4);
            data.push_back(5);
            REQUIRE(alloc_count == initial_allocs + 1);

            std::vector<int> orphan = data.orphan_vector();
            REQUIRE(alloc_count == initial_allocs + 1);
            REQUIRE(orphan.size() == 5);
            REQUIRE(orphan[0] == 1);
            REQUIRE(orphan[1] == 2);
            REQUIRE(orphan[2] == 3);
            REQUIRE(orphan[3] == 4);
            REQUIRE(orphan[4] == 5);
        }
    }

    SECTION("begin()~end() iteration works (range-for loop)")
    {
        ien::svo_vector<int> data;
        data.push_back(0);
        data.push_back(1);
        data.push_back(2);
        data.push_back(3);

        int i = 0;
        for(const auto& e : data)
        {
            REQUIRE(e == i++);
        }
        REQUIRE(i == 4);
        
        data.push_back(4);
        i = 0;
        for(const auto& e : data)
        {
            REQUIRE(e == i++);
        }
        REQUIRE(i == 5);
    };
};