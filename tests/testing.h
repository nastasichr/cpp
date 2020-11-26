#pragma once

#include <iostream>
#include <string>
#include <map>

struct TestRegistry {
	TestRegistry(const std::string& name, void(*function)());
	static bool run_all();
private:
	static std::map<std::string, void(*)()>* reg;
};

#define TEST(t) void t(); static TestRegistry ___reg_##t{#t, &t}; void t()

#define PRINT(exp) #exp << " = " << (exp)
