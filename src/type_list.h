#pragma once

#include <cstddef>
#include <type_traits>

namespace meta {

namespace details {

template<typename T, typename... Ts> struct IndexOf;

template<typename T, typename... Ts>
struct IndexOf<T, T, Ts...> {
	static constexpr size_t value = 0;
};

template<typename T, typename U, typename... Ts>
struct IndexOf<T, U, Ts...> {

	static constexpr size_t value = 1 + IndexOf<T, Ts...>::value;
};
}

template<typename B> struct Base {};

template<typename... Ts>
struct TypeSet : Base<Ts>... {

	template<typename T>
	constexpr auto operator+(Base<T>&&)
	{
		if constexpr (std::is_base_of<Base<T>, TypeSet>::value) {
			return TypeSet{};
		} else {
			return TypeSet<T, Ts...>{};
		}
	}

	static constexpr size_t size = sizeof...(Ts);
};

template<typename... Ts>
struct TypeList {
private:
	template<typename T>
	static constexpr size_t SafeIndexOf()
	{
		static_assert(has_type<T>, "TypeList must contain T to get its position");
		return details::IndexOf<T, Ts...>::value;
	}

public:
	static constexpr size_t size = sizeof...(Ts);

	template<typename T>
	static constexpr bool has_type = (std::is_same<T, Ts>::value || ...);

	template<typename T>
	static constexpr size_t index_of = SafeIndexOf<T>();

	static constexpr bool is_unique = decltype((TypeSet{} + ... + Base<Ts>{}))::size == size;
};

}
