#pragma once

#include <type_traits>

namespace meta {

namespace details {

template<typename T>
struct make_unique_id {
    static constexpr void identity(){};
};

}

template<typename T>
constexpr auto make_unique_id = (&details::make_unique_id<T>::identity);

template<typename T, T... Values>
struct max_value {
	static_assert(sizeof...(Values) > 0, "max_value requies a list of values");
};

template<typename T, T Found>
struct max_value<T, Found> {
	static constexpr T value = Found;
};

template<typename T, T V1, T V2, T... Others>
struct max_value<T, V1, V2, Others...> {
	static constexpr auto largest = (V1 > V2) ? V1 : V2;
	static constexpr auto value = max_value<T, largest, Others...>::value;
};

template<template<typename> class Norm, typename... Ts>
struct max {
	static_assert(sizeof...(Ts) > 0, "max requies a list of types");
};

template<template<typename> class Norm, typename T>
struct max<Norm, T> {
	static constexpr auto value = Norm<T>::value;
	using type = T;
};

template<template<typename> class Norm, typename T1, typename T2, typename... Ts>
struct max<Norm, T1, T2, Ts...> {
	static constexpr auto n1 = Norm<T1>::value;
	static constexpr auto n2 = Norm<T2>::value;
	using largest = typename std::conditional<(n1 > n2), T1, T2>::type;
	using type = typename max<Norm, largest, Ts...>::type;
	static constexpr auto value = Norm<type>::value;
};

template<typename T> struct size_of { static constexpr auto value = sizeof(T); };
template<typename T> struct align_of { static constexpr auto value = alignof(T); };


}
