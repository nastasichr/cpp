#pragma once

#include "type_list.h"

namespace meta {

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
	using reverse = type_map<type_value_pair<Ts, Ks>...>;
	using keys = type_list<value_to_type<Ks>...>;
	using types = type_list<Ts...>;
private:
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
	using reverse = type_map<type_type_pair<Ts, Ks>...>;
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
	using reverse = type_map<value_type_pair<Vs, Ks>...>;
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
