#pragma once

#include <cstddef>
#include <utility>
#include <string>

namespace meta { namespace string {

namespace details {

template<template<typename U, U...>class Derived, typename T, T... Values>
struct value_list {
private:
	template<size_t P>
	static constexpr T safe_at()
	{
		static_assert(P < length, "Index out of bunds");
		constexpr T values[length] = {Values...};
		return values[P];
	}

	template<size_t Offset, size_t... Indexes>
	static constexpr auto safe_inner_slice(std::index_sequence<Indexes...>)
	{
		return Derived<T, at<Offset + Indexes>...>{};
	}
public:
	template<size_t From, size_t To>
	static constexpr auto safe_slice()
	{
		static_assert(To <= length, "Upper-bound higher than lengh");
		static_assert(From <= To, "Extremes must be ordered");
		// The if-constexpr below avoids (potential) long compiler
		// recursions despite the static_assert above
		// This gives better compile-time error
		if constexpr (From > To || To > length) {
			return Derived<T>{};
		} else {
			constexpr auto indexes = std::make_index_sequence<To - From>{};
			return safe_inner_slice<From>(indexes);
		}
	}
public:
	static constexpr size_t length = sizeof...(Values);
	static constexpr size_t last_index = length - 1;
	static constexpr bool empty = (length == 0);

	template<class U>
	static constexpr bool equal = std::is_same<U, Derived<T, Values...>>::value;

	static const T* data()
	{
		static const T values[length] = {Values...};
		return values;
	}

	static inline std::initializer_list<T> initializer_list = {Values...};

	template<size_t P>
	static constexpr T at = safe_at<P>();

	template<size_t From, size_t To>
	using slice = decltype(safe_slice<From, To>());

	template<template<T...> class Action>
	using apply = Action<Values...>;

	template<T... Others>
	using append = Derived<T, Values..., Others...>;

	template<T... Others>
	using prepend = Derived<T, Others..., Values...>;

	template<class List>
	using merge = typename List::template prepend<Values...>;
};

template<typename C, C... S>
struct base_type_string : details::value_list<base_type_string, char, S...> {
private:
	using base = details::value_list<base_type_string, char, S...>;
public:
	static const char* c_str()
	{
		static char values[base::length + 1] = {S..., '\0'};
		return values;
	}
};

}

template<typename T, T... Values>
struct value_list : details::value_list<value_list, T, Values...> {};

template<char... S>
using type_string = details::base_type_string<char, S...>;

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
