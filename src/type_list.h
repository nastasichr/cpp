#pragma once

#include <cstddef>
#include <type_traits>

namespace meta {

template<typename... Ts>
struct TypeList {
	static constexpr size_t size = sizeof...(Ts);

	template<typename T>
	static constexpr bool has_type = (std::is_same<T, Ts>::value || ...);
};

}
