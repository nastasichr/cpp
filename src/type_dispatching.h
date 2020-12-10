#pragma once

#include "type_manip.h"
#include "type_list.h"
#include "type_map.h"
#include "function_binder.h"
#include "function_traits.h"

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

template<typename... Types>
struct static_any_of {
private:
	using size = uint16_t;
	using types = meta::type_list<Types...>;
	static_assert(types::is_unique, "List of types should be unique");
public:
	static_any_of() = default;

	template<typename T>
	static_any_of(T&& v)
	{
		static_assert(types::template has_type<T>, "Type not part of this any_of");
		auto target = reinterpret_cast<T*>(space);
		*target = v;
		id = types::template index_of<T>;
	}

	template<typename T, typename... Args>
	void store(Args&&... args)
	{
		static_assert(types::template has_type<T>, "Type not part of this any_of");
		new (space) T{std::forward<Args>(args)...};
		id = types::template index_of<T>;
	}

	template<class Visitor>
	void accept(Visitor&& v)
	{
		(try_dispatch<Types, Visitor>(v) || ...);
	}

private:
	union {
		char space[meta::max<meta::size_of, Types...>::value];
		typename meta::max<meta::align_of, Types...>::type alignment;
	};
	size id = types::size + 1;

	template<typename T, class Visitor>
	bool try_dispatch(Visitor&& v)
	{
		if (id == types::template index_of<T>) {
			auto t = reinterpret_cast<const T&>(*space);
			v.visit(t);
			return true;
		}
		return false;
	}
};

namespace details {
template<typename T>
struct queue_concept {
	T* acquire();
	void push(T*);
	T* pop();
	void release(T*);
};
}

#define CONCEPT_MEMFUNC_ASSERT(prototype, target, memfunc) \
	static_assert(meta::parse_function<decltype(&prototype::memfunc)>:: template equal< \
			meta::parse_function<decltype(&target::memfunc)>>, \
			"Given type " #target "::" #memfunc \
			" does not match concept " #prototype)

template<template<typename>class Q, typename... Types>
struct dispatch_queue {
	using element_type = any_of<Types...>;
	using subscriber = typename element_type::visitor;

	Q<element_type>& queue;

	dispatch_queue(Q<element_type>& q) : queue{q} {}

	template<typename T, typename... Args>
	void post(Args&&... args)
	{
		element_type* item = queue.acquire();
		item->template store<T>(std::forward<Args>(args)...);
		queue.push(item);
	}

	void dispatch(subscriber& s)
	{
		element_type* item = queue.pop();
		item->accept(s);
		queue.release(item);
	}
private:
	CONCEPT_MEMFUNC_ASSERT(details::queue_concept<element_type>, Q<element_type>, acquire);
	CONCEPT_MEMFUNC_ASSERT(details::queue_concept<element_type>, Q<element_type>, release);
	CONCEPT_MEMFUNC_ASSERT(details::queue_concept<element_type>, Q<element_type>, push);
	CONCEPT_MEMFUNC_ASSERT(details::queue_concept<element_type>, Q<element_type>, pop);

};

template<template<typename>class Q, typename... Types>
struct static_dispatch_queue {
	using element_type = static_any_of<Types...>;

	Q<element_type>& queue;

	static_dispatch_queue(Q<element_type>& q) : queue{q} {}

	template<typename T, typename... Args>
	void post(Args&&... args)
	{
		element_type* item = queue.acquire();
		item->template store<T>(std::forward<Args>(args)...);
		queue.push(item);
	}

	template<class... Visitors>
	void dispatch(Visitors&&... vs)
	{
		element_type* item = queue.pop();
		(item->accept(vs), ...);
		queue.release(item);
	}
private:
	CONCEPT_MEMFUNC_ASSERT(details::queue_concept<element_type>, Q<element_type>, acquire);
	CONCEPT_MEMFUNC_ASSERT(details::queue_concept<element_type>, Q<element_type>, release);
	CONCEPT_MEMFUNC_ASSERT(details::queue_concept<element_type>, Q<element_type>, push);
	CONCEPT_MEMFUNC_ASSERT(details::queue_concept<element_type>, Q<element_type>, pop);
};


template<class TypeMap>
struct dispatch_map {
private:
	template<size_t ID, class Visitor>
	static constexpr bool try_dispatch(size_t id, void* data, Visitor&& v)
	{
		if (id == ID) {
			v.visit(*(reinterpret_cast<typename TypeMap::template at<ID>*>(data)));
			return true;
		}
		return false;
	}
public:
	template<class Visitor>
	static void dispatch(size_t id, void* data, Visitor&& v)
	{
		const bool dispatched =
			TypeMap::keys::for_each_as_args([id, data, &v](auto...t) {
				return (try_dispatch<decltype(t)::type::value>(id, data, v) || ...);
			});
		if (!dispatched) {
			v.visit_uknown(id);
		}
	}
};

}
