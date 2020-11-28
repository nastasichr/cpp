#pragma once

#include <iostream>
#include <string>
#include <map>

namespace test {

struct registry {
	registry(const std::string& name, void(*function)());
	static bool run_all(const std::string&);
private:
	static std::map<std::string, void(*)()>* reg;
};

}

#define TEST(t) void t(); static test::registry ___reg_##t{#t, &t}; void t()
#define LOGGER (std::cout << "    ")
#define PRINT(exp) #exp << " = " << (exp)
#define ASSERT(exp) do { \
	if (!(exp)) { \
		LOGGER << "Assertion failed: " #exp << std::endl;\
		throw "";\
	} \
} while (0)
#define ASSERT_TYPE_EQ(t1, t2) \
	static_assert(std::is_same<t1, t2>::value, "Types do not match")
