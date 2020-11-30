#pragma once

#include <cstddef>
#include <utility>
#include <string>

namespace meta { namespace string {

template<typename T, T... Values>
struct value_list {
	static constexpr size_t length = sizeof...(Values);

	template<class U>
	static constexpr bool equal = std::is_same<U, value_list<T, Values...>>::value;

	static const T* data()
	{
		static T values[length] = {Values...};
		return values;
	}

	static inline std::initializer_list<T> initializer_list = {Values...};
};

template<char... S>
using type_string = value_list<char, S...>;

namespace details {

template <typename StringHolder, size_t...Is>
constexpr auto make_string(StringHolder holder, std::index_sequence<Is...>)
{
    constexpr std::string_view text = holder();
    return type_string<text[Is]...>{};
}

template <typename StringHolder, size_t...Is>
constexpr auto make_string(std::index_sequence<Is...>)
{
    constexpr std::string_view text = StringHolder::yield();
    return type_string<text[Is]...>{};
}

}

}}

#define MAKE_STRING(text) \
	meta::string::details::make_string([](){return text;},\
					   std::make_index_sequence<sizeof(text) - 1>())

#define MAKE_STRING_TYPE(name, text) \
struct _a_##name { static constexpr auto yield() { return text; } }; \
using name = decltype(meta::string::details::make_string<_a_##name>(std::make_index_sequence<sizeof(text) - 1>()))
