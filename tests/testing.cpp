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

bool registry::run_all()
{
	if (!reg) {
		return true;
	}
	bool has_failed = false;
	for (auto& item : *registry::reg) {
		std::cout << "--> Running test " << item.first << std::endl;
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
