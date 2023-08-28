#include <ien/str_utils.hpp>

#include <catch2/catch_test_macros.hpp>

#include <string>

#define STR(x) std::string(x)
#define STRV(v) std::vector<std::string>(v.begin(), v.end())

TEST_CASE("ien::str_split")
{
    GIVEN("An empty string")
    {
        std::string str_empty = "";
        WHEN("Splitting it with any character")
        {
            auto segments = ien::str_split(str_empty, ' ');
            THEN("An empty result is returned")
            {
                REQUIRE(segments.empty());
            };
        };
		WHEN("Splitting it with any C-String")
		{
			auto segments = ien::str_split(str_empty, ",.;");
			THEN("An empty result is returned")
			{
				REQUIRE(segments.empty());
			};
		};
		WHEN("Splitting it with any std::string")
		{
			auto segments = ien::str_split(str_empty, std::string("asdf"));
			THEN("An empty result is returned")
			{
				REQUIRE(segments.empty());
			};
		};
		WHEN("Splitting it with any std::string_view")
		{
			auto segments = ien::str_split(str_empty, std::string_view("asdf"));
			THEN("An empty result is returned")
			{
				REQUIRE(segments.empty());
			};
		};
    };

    GIVEN("A string with 3 characters separated by spaces (\"a b c\")")
    {
        std::string str = "a b c";
        WHEN("Trying to split it with a non present character")
		{
			auto segments = ien::str_split(str, '^');
			THEN("The entire string is returned as the first segment")
			{
				REQUIRE(segments.size() == 1);
				REQUIRE(segments[0] == str);
			};
		};
		WHEN("Trying to split it with spaces")
		{
			auto segments = ien::str_split(str, ' ');
			THEN("The individual characters are returned as segments")
			{
				REQUIRE(segments.size() == 3);
				REQUIRE(segments == decltype(segments){"a", "b", "c"});
			};
		};
		WHEN("Trying to split it with the last character")
		{
			auto segments = ien::str_split(str, 'c');
			THEN("The entire string, excluding the last character, is returned")
			{
				REQUIRE(segments.size() == 1);
				REQUIRE(segments == decltype(segments){"a b "});
			};
		};
    };

	GIVEN("A string with 5 consecutive 'x' characters")
	{
		std::string str = "xxxxx";
		WHEN("Trying to split it by an 'x' character")
		{
			auto segments = ien::str_split(str, 'x');
			THEN("An empty result is returned")
			{
				REQUIRE(segments.empty());
			};
		};
	}
};

TEST_CASE("ien::str_splitv")
{
    GIVEN("An empty string")
    {
        std::string str_empty = "";
        WHEN("Splitting it with any character")
        {
            auto segments = ien::str_splitv(str_empty, ' ');
            THEN("An empty result is returned")
            {
                REQUIRE(segments.empty());
            };
        };
		WHEN("Splitting it with any C-String")
		{
			auto segments = ien::str_splitv(str_empty, ",.;");
			THEN("An empty result is returned")
			{
				REQUIRE(segments.empty());
			};
		};
		WHEN("Splitting it with any std::string")
		{
			auto segments = ien::str_splitv(str_empty, std::string("asdf"));
			THEN("An empty result is returned")
			{
				REQUIRE(segments.empty());
			};
		};
		WHEN("Splitting it with any std::string_view")
		{
			auto segments = ien::str_splitv(str_empty, std::string_view("asdf"));
			THEN("An empty result is returned")
			{
				REQUIRE(segments.empty());
			};
		};
    };

    GIVEN("A string with 3 characters separated by spaces (\"a b c\")")
    {
        std::string str = "a b c";
        WHEN("Trying to split it with a non present character")
		{
			auto segments = ien::str_splitv(str, '^');
			THEN("The entire string is returned as the first segment")
			{
				REQUIRE(segments.size() == 1);
				REQUIRE(STR(segments[0]) == str);
			};
		};
		WHEN("Trying to split it with spaces")
		{
			auto segments = ien::str_splitv(str, ' ');
			THEN("The individual characters are returned as segments")
			{
				REQUIRE(segments.size() == 3);
				REQUIRE(STRV(segments) == std::vector<std::string>{"a", "b", "c"});
			};
		};
		WHEN("Trying to split it with the last character")
		{
			auto segments = ien::str_splitv(str, 'c');
			THEN("The entire string, excluding the last character, is returned")
			{
				REQUIRE(segments.size() == 1);
				REQUIRE(STRV(segments) == std::vector<std::string>{"a b "});
			};
		};
    };

	GIVEN("A string with 5 consecutive 'x' characters")
	{
		std::string str = "xxxxx";
		WHEN("Trying to split it by an 'x' character")
		{
			auto segments = ien::str_splitv(str, 'x');
			THEN("An empty result is returned")
			{
				REQUIRE(segments.empty());
			};
		};
	}
};

TEST_CASE("replace (char)")
{
    std::string str = " a b c ";
    std::string res = ien::str_replace(str, ' ', '-', 1, 5);
    REQUIRE(res == "a-b-c");
};

TEST_CASE("replace (str)")
{
    SECTION("Normal use case")
    {
        std::string str = "I have some pineapple candy left from yesterday. I love pineapples! I hate coconuts!";
        std::string res = ien::str_replace(str, "pineapple", "coconut", 0, 67);
        REQUIRE(res == "I have some coconut candy left from yesterday. I love coconuts!");
    };

    SECTION("Empty string")
    {
        std::string str;
        std::string res = ien::str_replace(str, " ", "asdf");
        REQUIRE(res == str);
    };

    SECTION("No replacement")
    {
        std::string str = "aaa bbb";
        std::string res = ien::str_replace(str, "c", "x");
        REQUIRE(res == str);
    };

    SECTION("Mix chars and strings")
    {
        std::string str = "aaa bbb";
        std::string res0 = ien::str_replace(str, ' ', "---");
        std::string res1 = ien::str_replace(str, "aaa", '*');
        REQUIRE(res0 == "aaa---bbb");
        REQUIRE(res1 == "* bbb");
    };

    SECTION("Replace chars with chars")
    {
        std::string str = "aaa bbb";
        std::string res = ien::str_replace(str, ' ', '-');
        REQUIRE(res == "aaa-bbb");
    };
};

TEST_CASE("trim (str)")
{
    SECTION("trim")
    {
        std::string str = "  hello  ";
        REQUIRE(ien::str_trim(str) == "hello");

        str = "hello   ";
        REQUIRE(ien::str_trim(str) == "hello");

        str = "    ";
        REQUIRE(ien::str_trim(str) == "");
    };
};