#include "type_list.h"
#include "testing.h"

using List0 = meta::TypeList<>;
using List1 = meta::TypeList<double>;
using List2 = meta::TypeList<int, int>;
using List3 = meta::TypeList<int, TestRegistry, int>;
using List4 = meta::TypeList<char, int, long, long long>;

TEST(TypeList_runtime_occupancy)
{
	std::cout << PRINT(sizeof(List0)) << std::endl;
	std::cout << PRINT(sizeof(List1)) << std::endl;
	std::cout << PRINT(sizeof(List2)) << std::endl;
	std::cout << PRINT(sizeof(List3)) << std::endl;
	std::cout << PRINT(sizeof(List4)) << std::endl;
}

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

TEST(TypeList_index_of)
{
	// Should fail compilation: static_assert(List0::index_of<int> == 0, "");
	static_assert(List1::index_of<double> == 0, "");
	static_assert(List2::index_of<int> == 0, "");
	static_assert(List3::index_of<int> == 0, "");
	static_assert(List3::index_of<TestRegistry> == 1, "");
}

TEST(TypeList_is_unique)
{
	static_assert(List0::is_unique == true, "");
	static_assert(List1::is_unique == true, "");
	static_assert(List2::is_unique == false, "");
	static_assert(List3::is_unique == false, "");
	static_assert(List4::is_unique == true, "");
}

int main()
{
	return TestRegistry::run_all() ? EXIT_SUCCESS : EXIT_FAILURE;
}
