#pragma once

#include "type_manip.h"
#include "type_list.h"
#include "function_binder.h"

namespace container {

template<typename... Types>
struct any_of {
private:
	template<typename T>
	struct visit_interface { virtual void visit(const T&) = 0; };

	using types = meta::type_list<Types...>;
	static_assert(types::is_unique, "List of types should be unique");
public:
	struct visitor : visit_interface<Types>... {};

	any_of() = default;

	template<typename T>
	any_of(T&& v)
	{
		static_assert(types::template has_type<T>, "Type not part of this any_of");
		auto target = reinterpret_cast<T*>(space);
		*target = v;
		capture(target);
	}

	template<typename T, typename... Args>
	void store(Args&&... args)
	{
		static_assert(types::template has_type<T>, "Type not part of this any_of");
		auto target = new (space) T{std::forward<Args>(args)...};
		capture(target);
	}

	void accept(visitor& v)
	{
		if (dispatch) {
			dispatch(v);
		}
	}

private:
	union {
		char space[meta::max<meta::size_of, Types...>::value];
		typename meta::max<meta::align_of, Types...>::type alignment;
	};
	sized_function<4 * sizeof(uintptr_t), void(visitor&)> dispatch;

	template<typename T>
	void capture(T* target)
	{
		dispatch = [target](visitor& v) {
			auto mfunc = &visit_interface<T>::visit;
			(v.*mfunc)(*target);
		};
	}
};

template<template<typename>class Q, size_t N, typename... Types>
struct type_dispatcher {
	using element_type = any_of<Types...>;
	using subscriber = typename element_type::visitor;
	
	Q<element_type>& queue;
	subscriber* subscribers[N] = {nullptr};
	size_t count = 0;

	type_dispatcher(Q<element_type>& q) : queue{q} {}

	void subscribe(subscriber& s)
	{
		if (count == N) {
			return;
		}
		subscribers[count++] = &s;
	}

	template<typename T, typename... Args>
	void post(Args&&... args)
	{
		queue.emplace_back();
		queue.back().template store<T>(std::forward<Args>(args)...);
	}

	void dispatch()
	{
		auto& item = queue.front();
		for (size_t i = 0; i < count; ++i) {
			item.accept(*subscribers[i]);
		}
		queue.pop_front();
	}
};

}
