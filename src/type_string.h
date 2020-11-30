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
struct type_string : value_list<char, S...> {
private:
	using base = value_list<char, S...>;
public:

	template<class U>
	static constexpr bool equal = std::is_same<U, type_string<S...>>::value;

	static const char* c_str()
	{
		static char values[base::length + 1] = {S..., '\0'};
		return values;
	}
};

namespace details {

template <typename Holder, size_t...Is>
constexpr auto make_string(Holder h, std::index_sequence<Is...>)
{
    constexpr std::string_view text = h();
    (void)text;
    return type_string<text[Is]...>{};
}

template <typename Holder, size_t...Is>
constexpr auto make_string(std::index_sequence<Is...>)
{
    constexpr std::string_view text = Holder::yield();
    (void)text;
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
