#include <ien/lang_utils.hpp>

#include <string>

static_assert(std::is_same_v<char, ien::underlying_char_t<const char*>>);
static_assert(std::is_same_v<wchar_t, ien::underlying_char_t<std::wstring>>);
static_assert(std::is_same_v<char16_t, ien::underlying_char_t<std::u16string_view>>);
static_assert(std::is_same_v<char32_t, ien::underlying_char_t<char32_t[77]>>);