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

TEST(a)
{
}

int main()
{
	for (auto& item : TestRegistry::reg) {
		std::cout << "Running test " << item.first << std::endl;
		item.second();
	}

	return 0;
}
