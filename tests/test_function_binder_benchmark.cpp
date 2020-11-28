#include "function_binder.h"
#include "testing.h"

#include <chrono>
#include <vector>
#include <functional>

using container::small_function;

template<class F, class T>
void benchmark_base(const std::string& name, size_t n, T&& t)
{
	using clock = std::chrono::high_resolution_clock;

	std::vector<F> functions(n);

	auto lambda = []{};
	const auto then = clock::now();

	for (size_t i = 0; i < n; ++i) {
		t(lambda, functions[i]);
	}

	const auto now = clock::now();
	std::chrono::duration<double, std::milli> elapsed_ms = now - then;
	LOGGER	<< name << " -> "
		<< "Lambda-closure size = " << sizeof(lambda) << ", "
		<< PRINT(n) << ", "
		<< PRINT(elapsed_ms.count()) << ", "
		<< " microseconds per iteration = " << (elapsed_ms.count() * 1000) / n
		<< std::endl;
}

template<class F>
void benchmark0(const std::string& name, size_t n)
{
	benchmark_base<F>(name, n, [](auto& closure, auto& func) {
		func = closure;
	});
}

template<class F>
void benchmark1(const std::string& name, size_t n)
{
	benchmark_base<F>(name, n, [](auto& closure, auto& func) {
		func = closure;
		func();
	});
}

constexpr size_t _1M = 1 * 1024 * 1024;
constexpr size_t _10M =  10 * _1M;

TEST(benchmark_bind_only)
{
	benchmark0<std::function<void()>> ("std::function",  _1M);
	benchmark0<std::function<void()>> ("std::function",  _10M);
	benchmark0<small_function<void()>>("small_function", _1M);
	benchmark0<small_function<void()>>("small_function", _10M);
}

TEST(benchmark_bind_and_call)
{
	benchmark1<std::function<void()>> ("std::function",  _1M);
	benchmark1<std::function<void()>> ("std::function",  _10M);
	//benchmark1<std::function<void()>> ("std::function",  _100M);
	benchmark1<small_function<void()>>("small_function", _1M);
	benchmark1<small_function<void()>>("small_function", _10M);
	//benchmark1<small_function<void()>>("small_function", _100M);
}

int main()
{
	return test::registry::run_all("benchmark function binder") ? EXIT_SUCCESS : EXIT_FAILURE;
}
