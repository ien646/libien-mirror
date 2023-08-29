#include <ien/lang_utils.hpp>

#include <catch2/catch_test_macros.hpp>

#include <string>

static_assert(std::is_same_v<char, ien::underlying_char_t<const char*>>);
static_assert(std::is_same_v<char, ien::underlying_char_t<char[16]>>);
static_assert(std::is_same_v<char, ien::underlying_char_t<std::string>>);
static_assert(std::is_same_v<char, ien::underlying_char_t<std::string_view>>);

static_assert(std::is_same_v<wchar_t, ien::underlying_char_t<const wchar_t*>>);
static_assert(std::is_same_v<wchar_t, ien::underlying_char_t<wchar_t[16]>>);
static_assert(std::is_same_v<wchar_t, ien::underlying_char_t<std::wstring>>);
static_assert(std::is_same_v<wchar_t, ien::underlying_char_t<std::wstring_view>>);

static_assert(std::is_same_v<char8_t, ien::underlying_char_t<const char8_t*>>);
static_assert(std::is_same_v<char8_t, ien::underlying_char_t<char8_t[16]>>);
static_assert(std::is_same_v<char8_t, ien::underlying_char_t<std::u8string>>);
static_assert(std::is_same_v<char8_t, ien::underlying_char_t<std::u8string_view>>);

static_assert(std::is_same_v<char16_t, ien::underlying_char_t<const char16_t*>>);
static_assert(std::is_same_v<char16_t, ien::underlying_char_t<char16_t[16]>>);
static_assert(std::is_same_v<char16_t, ien::underlying_char_t<std::u16string>>);
static_assert(std::is_same_v<char16_t, ien::underlying_char_t<std::u16string_view>>);

static_assert(std::is_same_v<char32_t, ien::underlying_char_t<const char32_t*>>);
static_assert(std::is_same_v<char32_t, ien::underlying_char_t<char32_t[16]>>);
static_assert(std::is_same_v<char32_t, ien::underlying_char_t<std::u32string>>);
static_assert(std::is_same_v<char32_t, ien::underlying_char_t<std::u32string_view>>);

TEST_CASE("dummy")
{
    SECTION("dummy")
    {
        REQUIRE(true);
    };
};