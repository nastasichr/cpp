#pragma once

#include <cstddef>
#include <type_traits>
#include "type_manip.h"

namespace meta {

struct not_found;

namespace details {

template<template<typename> class Condition, typename... Ts>
struct find_if {
	using type = not_found;
};

template<template<typename> class Condition, typename T, typename... Ts>
struct find_if<Condition, T, Ts...> {
	using type = typename std::conditional<
		Condition<T>::value,
		T,
		typename find_if<Condition, Ts...>::type>::type;
};

template<typename T, typename... Ts> struct index_of;

template<typename T, typename... Ts>
struct index_of<T, T, Ts...> {
	static constexpr size_t value = 0;
};

template<typename T, typename U, typename... Ts>
struct index_of<T, U, Ts...> {
	static constexpr size_t value = 1 + index_of<T, Ts...>::value;
};

template<size_t pos, typename... Ts> struct type_at;

template<typename T, typename... Ts>
struct type_at<0, T, Ts...> {
	using type = T;
};

template<size_t pos, typename T, typename... Ts>
struct type_at<pos, T, Ts...> {
	using type = typename type_at<pos - 1, Ts...>::type;
};

}

template<size_t N>
struct value_to_type {
	static constexpr size_t value = N;
};

template<typename B> struct placeholder {
	using type = B;
};

template<typename B> struct base {};

template<typename... Ts>
struct type_set : base<Ts>... {

	template<typename T>
	constexpr auto operator+(base<T>&&)
	{
		if constexpr (std::is_base_of<base<T>, type_set>::value) {
			return type_set{};
		} else {
			return type_set<T, Ts...>{};
		}
	}

	static constexpr size_t size = sizeof...(Ts);
};

template<typename... Ts>
struct type_list {
private:
	template<typename T>
	static constexpr size_t safe_index_of()
	{
		static_assert(has_type<T>, "type_list must contain T to get its position");
		return details::index_of<T, Ts...>::value;
	}

public:
	template<size_t pos>
	static constexpr auto safe_type_at()
	{
		static_assert(pos < sizeof...(Ts), "requested type at index out of bounds");
		return placeholder<typename details::type_at<pos, Ts...>::type>{};
	}

	static constexpr size_t size = sizeof...(Ts);

	template<typename T>
	static constexpr bool has_type = (std::is_same<T, Ts>::value || ...);

	template<typename T>
	static constexpr size_t index_of = safe_index_of<T>();

	template<template<typename> class Condition>
	using find_if = typename details::find_if<Condition, Ts...>::type;

	static constexpr bool is_unique = decltype((type_set{} + ... + base<Ts>{}))::size == size;

	template<size_t pos>
	using at = typename decltype(safe_type_at<pos>())::type;

	template<template<typename>class Norm>
	using max = meta::max<Norm, Ts...>;

	template<template<typename...> class F>
	static constexpr auto for_each()
	{
		 return F<Ts...>::process();
	}

	template<class F>
	static constexpr auto for_each(F&& f)
	{
		 return f.template operator()<Ts...>();
	}

	template<class F>
	static constexpr auto for_each_as_args(F&& f)
	{
		return f(placeholder<Ts>{}...);
	}
};

}
