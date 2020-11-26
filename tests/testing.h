#pragma once

#include <iostream>
#include <string>
#include <map>

namespace test {

struct registry {
	registry(const std::string& name, void(*function)());
	static bool run_all();
private:
	static std::map<std::string, void(*)()>* reg;
};

}

#define TEST(t) void t(); static test::registry ___reg_##t{#t, &t}; void t()

#define PRINT(exp) #exp << " = " << (exp)
