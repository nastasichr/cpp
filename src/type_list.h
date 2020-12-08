#pragma once

#include <cstddef>
#include <type_traits>

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

template<size_t N> struct value_to_type;

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

template<size_t K, typename T>
struct value_type_pair {
	static constexpr size_t key = K;
	using type = T;
};

template<typename K, typename T>
struct type_type_pair {
	using key = K;
	using type = T;
};

template<typename K, size_t V>
struct type_value_pair {
	using key = K;
	static constexpr size_t value = V;
};

template<class... Entries> struct type_map;

template<size_t... Ks, typename... Ts>
struct type_map<value_type_pair<Ks, Ts>...> {
private:
	using keys = type_list<value_to_type<Ks>...>;
	using items = type_list<value_type_pair<Ks, Ts>...>;
	static_assert(keys::is_unique, "List of keys should be unique");
	template<size_t K>
	struct key {
		template<class Item>
		struct matches {
			static constexpr bool value = (Item::key == K);
		};
	};

public:
	template<size_t K>
	static constexpr auto safe_at()
	{
		using result = typename items::template find_if<key<K>::template matches>;
		if constexpr (std::is_same<result, not_found>::value) {
			return (not_found *)nullptr;
		} else {
			return (typename result::type *)nullptr;
		}
	}

	template<size_t K>
	using at = typename std::remove_pointer<decltype(safe_at<K>())>::type;
};

template<typename... Ks, typename... Ts>
struct type_map<type_type_pair<Ks, Ts>...> {
private:
	using keys = type_list<Ks...>;
	using items = type_list<type_type_pair<Ks, Ts>...>;
	static_assert(keys::is_unique, "List of keys should be unique");
	template<typename K>
	struct key {
		template<class Item>
		struct matches {
			static constexpr bool value = std::is_same_v<typename Item::key,K>;
		};
	};

public:
	template<typename K>
	static constexpr auto safe_at()
	{
		using result = typename items::template find_if<key<K>::template matches>;
		if constexpr (std::is_same<result, not_found>::value) {
			return (not_found *)nullptr;
		} else {
			return (typename result::type *)nullptr;
		}
	}

	template<typename K>
	using at = typename std::remove_pointer<decltype(safe_at<K>())>::type;
};

template<typename... Ks, size_t... Vs>
struct type_map<type_value_pair<Ks, Vs>...> {
private:
	using keys = type_list<Ks...>;
	using items = type_list<type_value_pair<Ks, Vs>...>;
	static_assert(keys::is_unique, "List of keys should be unique");
	template<typename K>
	struct key {
		template<class Item>
		struct matches {
			static constexpr bool value = std::is_same_v<typename Item::key,K>;
		};
	};

public:
	template<typename K>
	static constexpr auto safe_at()
	{
		using result = typename items::template find_if<key<K>::template matches>;
		static_assert(!std::is_same<result, not_found>::value, "Key not found");
		return result::value;
	}

	template<typename K>
	static constexpr size_t at = safe_at<K>();
};

}
