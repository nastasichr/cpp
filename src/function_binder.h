#pragma once
#include <cstddef>
#include <cstdint>
#include <utility>

namespace container {

namespace details {

template<typename R, typename... Args>
struct callable {
	virtual R operator()(Args&&...) = 0;
};

template<typename F, typename R, typename... Args>
struct binder : callable<R, Args...> {
	F target;
	binder(F f) : target{f} {}

	R operator()(Args&&... args) override
	{
		return target(std::forward<Args>(args)...);
	}
};

}

template<size_t Size>
struct fixed_size_storage {
	static constexpr size_t max_size = Size;
	constexpr void* get_write_space(size_t) { return space; }
	constexpr void* get_read_space(size_t) { return space; }
private:
	char space[max_size];
};

template<typename...> struct function;

template<class Storage, typename R, typename... Args>
struct function<Storage, R(Args...)> {

	Storage storage;

	function()
	{
		static_assert(smallest_binder <= Storage::max_size,
			      "Not enough space to store the function");
		static_assert(sizeof(uintptr_t) <= smallest_binder);
		void* space = storage.get_write_space(sizeof(uintptr_t));
		*(reinterpret_cast<uintptr_t*>(space)) = 0;
	}

	template<class F>
	function(F&& f)
	{
		using binder = details::binder<F, R, Args...>;
		static_assert(sizeof(binder) <= Storage::max_size,
			      "Not enough space to store the function");
		void* space = storage.get_write_space(sizeof(binder));
		new (space) binder{f};
	}

	R operator()(Args&&... args)
	{
		using callable = details::callable<R, Args...>;
		void* space = storage.get_read_space(sizeof(callable));
		auto f = reinterpret_cast<callable*>(space);
		return (*f)(std::forward<Args>(args)...);
	}

	bool valid()
	{
		void* space = storage.get_read_space(sizeof(uintptr_t));
		return *(reinterpret_cast<uintptr_t*>(space)) != 0;
	}

	operator bool()
	{
		return valid();
	}

private:
	static R nop(Args...);
	static constexpr size_t smallest_binder = sizeof(details::binder<decltype(&nop), R, Args...>);
};

template<size_t Size, typename R, typename... Args>
using sized_function = function<fixed_size_storage<Size>, R, Args...>;

template<typename R, typename... Args>
using small_function = sized_function<sizeof(void*) * 2, R, Args...>;

}
