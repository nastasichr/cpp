#include "type_list.h"
#include "testing.h"

class abstract {
	char fat[1024];
	virtual ~abstract() = 0;
};

using list0 = meta::type_list<>;
using list1 = meta::type_list<double>;
using list2 = meta::type_list<int, int>;
using list3 = meta::type_list<int, abstract, int>;
using list4 = meta::type_list<char, int, long, long long>;

TEST(type_list_runtime_occupancy)
{
	std::cout << PRINT(sizeof(list0)) << std::endl;
	std::cout << PRINT(sizeof(list1)) << std::endl;
	std::cout << PRINT(sizeof(list2)) << std::endl;
	std::cout << PRINT(sizeof(list3)) << std::endl;
	std::cout << PRINT(sizeof(list4)) << std::endl;
}

TEST(type_list_size)
{
	static_assert(list0::size == 0, "Size should be 0");
	static_assert(list1::size == 1, "Size should be 1");
	static_assert(list2::size == 2, "Size should be 2");
	static_assert(list3::size == 3, "Size should be 3");
}

TEST(type_list_has_type)
{
	static_assert(list0::has_type<int> == false, "");
	static_assert(list1::has_type<int> == false, "");
	static_assert(list1::has_type<double> == true, "");
	static_assert(list2::has_type<double> == false, "");
	static_assert(list2::has_type<int> == true, "");
	static_assert(list3::has_type<int> == true, "");
	static_assert(list3::has_type<double> == false, "");
	static_assert(list3::has_type<abstract> == true, "");
}

TEST(type_list_index_of)
{
	// Should fail compilation: static_assert(list0::index_of<int> == 0, "");
	static_assert(list1::index_of<double> == 0, "");
	static_assert(list2::index_of<int> == 0, "");
	static_assert(list3::index_of<int> == 0, "");
	static_assert(list3::index_of<abstract> == 1, "");
}

TEST(type_list_is_unique)
{
	static_assert(list0::is_unique == true, "");
	static_assert(list1::is_unique == true, "");
	static_assert(list2::is_unique == false, "");
	static_assert(list3::is_unique == false, "");
	static_assert(list4::is_unique == true, "");
}

int main()
{
	std::cout << "===>> " __FILE__ << std::endl;
	return test::registry::run_all() ? EXIT_SUCCESS : EXIT_FAILURE;
}
