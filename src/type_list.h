#pragma once

#include <cstddef>
#include <type_traits>

namespace meta {

namespace details {

template<typename T, typename... Ts> struct index_of;

template<typename T, typename... Ts>
struct index_of<T, T, Ts...> {
	static constexpr size_t value = 0;
};

template<typename T, typename U, typename... Ts>
struct index_of<T, U, Ts...> {

	static constexpr size_t value = 1 + index_of<T, Ts...>::value;
};
}

template<typename B> struct TypeHolder {
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
	static constexpr size_t size = sizeof...(Ts);

	template<typename T>
	static constexpr bool has_type = (std::is_same<T, Ts>::value || ...);

	template<typename T>
	static constexpr size_t index_of = safe_index_of<T>();

	static constexpr bool is_unique = decltype((type_set{} + ... + base<Ts>{}))::size == size;

	template<template<typename...> class F>
	static auto for_each()
	{
		 return F<Ts...>::f();
	}

	template<class F>
	static auto for_each_alt(F&& f)
	{
		return f(TypeHolder<Ts>{}...);
	}
};

}
