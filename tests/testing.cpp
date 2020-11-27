#include "testing.h"
#include <type_traits>

namespace test {

std::map<std::string, void(*)()>* registry::reg = nullptr;

registry::registry(const std::string& name, void(*test)())
{
	if (!reg) {
		reg = new std::remove_reference<decltype(*reg)>::type;
	}
	(*reg)[name] = test;
}

bool registry::run_all(const std::string& message)
{
	if (!reg) {
		return true;
	}
	std::cout << "========================================================" << std::endl;
	std::cout << "Running tests " << message << std::endl;
	std::cout << "========================================================" << std::endl;
	bool has_failed = false;
	for (auto& item : *registry::reg) {
		std::cout << "--> Test " << item.first << std::endl;
		try {
			item.second();
			std::cout << "<-- Success " << std::endl;
		} catch (...) {
			has_failed = true;
			std::cout << "<-- Failed " << item.first << std::endl;
		}
	}
	return !has_failed;
}

}
