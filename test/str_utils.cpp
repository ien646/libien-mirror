#include <ien/str_utils.hpp>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("split")
{
	std::string str0 = "a b c";
	std::vector<std::string> segments0 = ien::str_split(str0, ' ');
	REQUIRE(segments0.size() == 3);
	REQUIRE(segments0[0] == "a");
	REQUIRE(segments0[1] == "b");
	REQUIRE(segments0[2] == "c");

	std::string str1;
	auto segments1 = ien::str_split(str1, 'a');
	REQUIRE(segments1.empty());

	std::string str2 = "a b c";
	auto segments2 = ien::str_split(str0, 'c');
	REQUIRE(segments2.size() == 1);
	REQUIRE(segments2[0] == "a b ");

	auto segments3 = ien::str_split("a b c", ' ');
	REQUIRE(segments3.size() == 3);
	REQUIRE(segments3[0] == "a");
	REQUIRE(segments3[1] == "b");
	REQUIRE(segments3[2] == "c");

	auto segments3 = ien::str_split("a b c", " ");
	REQUIRE(segments3.size() == 3);
	REQUIRE(segments3[0] == "a");
	REQUIRE(segments3[1] == "b");
	REQUIRE(segments3[2] == "c");
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