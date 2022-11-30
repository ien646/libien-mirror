#include <ien/net_utils.hpp>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("is_ip_address")
{
    SECTION("ipv4")
    {
        REQUIRE(true == ien::is_ipv4_address("192.168.1.1"));
        REQUIRE(true == ien::is_ipv4_address("0.0.0.0"));
        REQUIRE(false == ien::is_ipv4_address("0.0.0"));
        REQUIRE(false == ien::is_ipv4_address("0.0.0.257"));
        REQUIRE(false == ien::is_ipv4_address(""));
    };

    SECTION("ipv6")
    {
        REQUIRE(true == ien::is_ipv6_address("2001:db8:3333:4444:5555:6666:7777:8888"));
        REQUIRE(true == ien::is_ipv6_address("2001:db8:1::ab9:C0A8:102"));
        REQUIRE(true == ien::is_ipv6_address("::1234:5678"));
        REQUIRE(true == ien::is_ipv6_address("::"));
        REQUIRE(false == ien::is_ipv6_address("0.0.0.0"));
        REQUIRE(false == ien::is_ipv6_address(""));
    };
};