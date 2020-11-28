#pragma once
#include <type_traits>

namespace meta {

namespace details { namespace function {

template<template<typename, typename...> class Action,typename R, typename... Args>
struct parsed {
	using ret_type = R;
	using apply = Action<R, Args...>;
};

template<template<typename, typename...> class Action, typename> struct parse;

template<template<typename, typename...> class Action, typename R, typename... Args>
struct parse<Action, R(Args...)> : parsed<Action, R, Args...> {};

template<template<typename, typename...> class Action, typename R, typename... Args>
struct parse<Action, R(*)(Args...)> : parsed<Action, R, Args...> {};

template<template<typename, typename...> class Action, class F, typename R, typename... Args>
struct parse<Action, R(F::*)(Args...)> : parsed<Action, R, Args...> {};

template<template<typename, typename...> class Action, class F, typename R, typename... Args>
struct parse<Action, R(F::*)(Args...) const> : parsed<Action, R, Args...> {};

template<typename, typename...> struct no_parse_action;

}}

template<typename T>
struct is_function_object {
private:
	struct True;
	struct False;
	template<typename U> static True  sfinae(decltype(&U::operator()));
	template<typename U> static False sfinae(...);
public:
	static constexpr bool value = std::is_same<decltype(sfinae<T>(nullptr)), True>::value;
};

template<class F,
	 template<typename, typename...> class Action = details::function::no_parse_action,
	 class Enable = void>
struct parse_function : details::function::parse<Action, F> {};

template<class F,
	 template<typename, typename...> class Action>
struct parse_function<
	F,
	Action,
	typename std::enable_if<is_function_object<F>::value>::type
	> : details::function::parse<Action, decltype(&F::operator())>{};

}
