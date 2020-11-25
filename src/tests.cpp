#include "type_list.h"

#include <iostream>
#include <string>
#include <map>

struct TestRegistry {
	static std::map<std::string, void(*)()> reg;

	TestRegistry(const std::string& name, void(*test)())
	{
		reg[name] = test;
	}
};
std::map<std::string, void(*)()> TestRegistry::reg;

#define TEST(t) void t(); static TestRegistry ___reg_##t{#t, &t}; void t()

using List0 = meta::TypeList<>;
using List1 = meta::TypeList<double>;
using List2 = meta::TypeList<int, int>;
using List3 = meta::TypeList<int, TestRegistry, int>;

TEST(TypeList_size)
{
	static_assert(List0::size == 0, "Size should be 0");
	static_assert(List1::size == 1, "Size should be 1");
	static_assert(List2::size == 2, "Size should be 2");
	static_assert(List3::size == 3, "Size should be 3");
}

TEST(TypeList_has_type)
{
	static_assert(List0::has_type<int> == false, "");
	static_assert(List1::has_type<int> == false, "");
	static_assert(List1::has_type<double> == true, "");
	static_assert(List2::has_type<double> == false, "");
	static_assert(List2::has_type<int> == true, "");
	static_assert(List3::has_type<int> == true, "");
	static_assert(List3::has_type<double> == false, "");
	static_assert(List3::has_type<TestRegistry> == true, "");
}

int main()
{
	for (auto& item : TestRegistry::reg) {
		std::cout << "Running test " << item.first << std::endl;
		item.second();
	}

	return 0;
}
