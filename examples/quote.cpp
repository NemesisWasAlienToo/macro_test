#include <iostream>
#include <algorithm>

template <size_t N>
struct String
{
    constexpr static std::size_t size = N;
    char data[N] = {0};

    constexpr String() = default;
    constexpr String(String const &Other) { std::copy_n(Other.data, N, data); }
    constexpr String(char const (&str)[N + 1]) { std::copy_n(str, N, data); }
};

template <>
struct String<0>
{
    constexpr static std::size_t size = 0;

    constexpr String() = default;
    constexpr String(String const &) = default;
    constexpr String(char const (&)[1]) {}
};

template <std::size_t N>
String(char const (&)[N]) -> String<N - 1>;

template <String... Strings>
constexpr auto concat()
{
    constexpr size_t totalSize = (... + Strings.size) + sizeof...(Strings);
    char result[totalSize] = {0};

    size_t pos = 0;
    ((std::copy_n(Strings.data, Strings.size, result + pos), result[pos + Strings.size] = ' ', pos += Strings.size + 1), ...);

    return String(result);
}

template <String Text>
constexpr auto quotes()
{
    char result[Text.size + 3] = {0};
    result[0] = '\"';
    result[Text.size + 1] = '\"';
    std::copy_n(Text.data, Text.size, result + 1);
    return String(result);
}

template <String... Tokens>
struct quoteImpl
{
  static constexpr auto result = quotes<concat<Tokens...>()>();
};

namespace std
{
  template<String... Tokens>
  constexpr auto quote()
  {
    return quoteImpl<Tokens...>::result.data;
  }
}

int main()
{
    std::cout << macro std::quote{hi there};
   
    return 0;
}