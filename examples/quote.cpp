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
    constexpr auto operator[](std::size_t i) const { return data[i]; }
};

template <>
struct String<0>
{
    constexpr static std::size_t size = 0;

    constexpr String() = default;
    constexpr String(String const &) = default;
    constexpr String(char const (&)[1]) {}
};

template <String Original, String Text>
constexpr auto Find(size_t Index)
{
    for (size_t i = Index; i < Original.size - Text.size + 1; i++)
    {
        if (Original[i] != Text[0])
            continue;

        bool found = true;

        for (size_t j = 1; j < Text.size; j++)
        {
            if (Original[i + j] != Text[j])
            {
                found = false;
                break;
            }
        }

        if (found)
        {
            return i;
        }
    }

    return static_cast<size_t>(-1);
}

template <String Original, String Phrase>
constexpr size_t FindCount()
{
    if (!Phrase.size || !Original.size)
        return 0;

    size_t Count = 0;
    size_t Index = 0;

    while ((Index = Find<Original, Phrase>(Index)) != static_cast<size_t>(-1))
    {
        Count++;
        Index += Phrase.size;
    }

    return Count;
}

template <String Original, String Phrase, String Text>
constexpr auto Replace(size_t Index = 0)
{
    constexpr size_t DataSize = []()
    {
        if (Phrase.size == Text.size)
            return Original.size;
        
        size_t Count = FindCount<Original, Phrase>();

        if (Text.size > Phrase.size)
        {
            return Original.size + Count * (Text.size - Phrase.size);
        }
        else
        {
            return Original.size - Count * (Phrase.size - Text.size);
        }
    }();

    char Data[DataSize + 1];

    if (!Original.size || !Phrase.size)
        return String(Data);

    size_t OCounter = 0, RCounter = 0;

    while ((Index = Find<Original, Phrase>(RCounter)) != static_cast<size_t>(-1))
    {
        for (; RCounter < Index; RCounter++)
        {
            Data[OCounter++] = Original[RCounter];
        }

        for (size_t i = 0; i < Text.size; i++)
        {
            Data[OCounter++] = Text[i];
        }

        RCounter += Phrase.size;
    }

    for (; RCounter < Original.size; RCounter++)
    {
        Data[OCounter++] = Original[RCounter];
    }

    return String(Data);
}

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
  static constexpr auto result = quotes<Replace<concat<Tokens...>(), "\"", "\\\"">()>();
};

namespace std 
{
  template<String... Tokens>
  constexpr auto quote()
  {
    return quoteImpl<Tokens...>::result.data;
  }
}

template<String... Tokens>
constexpr auto rec()
{
  //return macro std::quote{std::cout << \"hi\" << std::endl};
  return macro std::quote{std::cout << "hi" << std::endl};
}

int main()
{
  macro rec{};
  //std::cout << macro std::macros::quote{hi there} << std::endl;
  return 0;
}
