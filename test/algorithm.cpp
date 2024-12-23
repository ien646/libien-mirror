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

struct move_tracking_object
{
    int value = -1;
    bool moved_into = false;
    bool moved_from = false;
    bool deleted = false;

    constexpr move_tracking_object(int v)
        : value(v)
    {
    }

    move_tracking_object(move_tracking_object&& o)
        : value(o.value)
        , moved_into(true)
        , moved_from(o.moved_from)
        
        , deleted(o.deleted)
    {
        o.moved_from = true;
    }

    move_tracking_object& operator=(move_tracking_object&& o)
    {
        moved_into = true;
        deleted = o.deleted;
        moved_from = o.moved_from;
        value = o.value;
        o.moved_from = true;

        return *this;
    }

    ~move_tracking_object() { deleted = true; }
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

    SECTION("erase_unsorted")
    {
        std::vector<move_tracking_object> vec;
        for (size_t i = 0; i < 10; ++i)
        {
            vec.emplace_back(move_tracking_object{(int)i});
        }

        for(auto& v : vec)
        {
            v.moved_into = false;
        }
        
        ien::erase_unsorted(vec, 5);

        REQUIRE(vec.size() == 9);
        REQUIRE(vec[5].value == 9);
        REQUIRE(vec[5].moved_into);
        for(size_t i = 6; i < vec.size(); ++i)
        {
            REQUIRE(!vec[i].moved_from);
            REQUIRE(!vec[i].moved_into);
        }
    }
};