#include <ien/algorithm.hpp>

#include <catch2/catch_test_macros.hpp>

#include <deque>
#include <functional>
#include <list>
#include <set>
#include <span>
#include <vector>

struct deletable_object
{
    bool deleted = false;
    bool moved = false;

    constexpr deletable_object() {}

    deletable_object(deletable_object&& o)
        : deleted(o.deleted)
        , moved(o.moved)
    {
        o.moved = true;
    }

    deletable_object& operator=(deletable_object&& o)
    {
        deleted = o.deleted;
        moved = o.moved;
        o.moved = true;

        return *this;
    }

    ~deletable_object() { deleted = true; }
};

template <template <typename...> typename TSrc, template <typename...> typename TDst>
void test_method(std::function<void(TSrc<deletable_object>&, int)> add_func)
{
    TSrc<deletable_object> src;
    TDst<deletable_object> dst;
    for (size_t i = 0; i < 10; ++i)
    {
        add_func(src, i);
    }

    //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    ien::push_many(dst, std::move(src));
    //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

    REQUIRE(src.size() == 10);
    REQUIRE(dst.size() == 10);

    for (const auto& v : src)
    {
        REQUIRE(v.moved == true);
        REQUIRE(v.deleted == false);
    }

    for (const auto& v : dst)
    {
        REQUIRE(v.moved == false);
        REQUIRE(v.deleted == false);
    }
}

TEST_CASE("algorithm")
{
    SECTION("push_many")
    {
        const auto push_back_func = [](auto& src, int i) { src.push_back(deletable_object{}); };
        test_method<std::vector, std::vector>(push_back_func);
        test_method<std::deque, std::vector>(push_back_func);
        test_method<std::list, std::vector>(push_back_func);

        test_method<std::vector, std::deque>(push_back_func);
        test_method<std::deque, std::deque>(push_back_func);
        test_method<std::list, std::deque>(push_back_func);

        test_method<std::vector, std::list>(push_back_func);
        test_method<std::deque, std::list>(push_back_func);
        test_method<std::list, std::list>(push_back_func);
    };
};