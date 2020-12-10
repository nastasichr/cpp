#include "type_debug.h"
#include "type_map.h"
#include "testing.h"

class abstract {
	char fat[1024];
	virtual ~abstract() = 0;
};

using meta::type_map;
using meta::value_type_pair;
using meta::type_value_pair;
using meta::type_type_pair;

using map1 = type_map<value_type_pair<1, int>>;
using map2 = type_map<
		value_type_pair<1, int>,
		value_type_pair<2, abstract>
	     >;
// Will static_assert
//using map3 = type_map<value_type_pair<1, int>, value_type_pair<1, abstract>>;
using map4 = type_map<
		value_type_pair<7,   int>,
		value_type_pair<9,   abstract>,
		value_type_pair<10,  int>,
		value_type_pair<100, int>
	     >;

TEST(value_to_type_gives_different_types)
{
	using t1 = meta::value_to_type<1>;
	using t2 = meta::value_to_type<2>;
	using d1 = meta::value_to_type<1>;

	ASSERT_TYPE_NE(t1, t2);
	ASSERT_TYPE_EQ(t1, d1);
}

TEST(value_type_map_lookups_ok)
{
	ASSERT_TYPE_EQ(map1::at<1>, int);
	ASSERT_TYPE_EQ(map2::at<1>, int);
	ASSERT_TYPE_EQ(map2::at<2>, abstract);
	ASSERT_TYPE_EQ(map4::at<7>, int);
	ASSERT_TYPE_EQ(map4::at<9>, abstract);
	ASSERT_TYPE_EQ(map4::at<10>, int);
	ASSERT_TYPE_EQ(map4::at<100>, int);
}

TEST(value_type_map_lookups_fails)
{
	ASSERT_TYPE_EQ(map1::at<2>, meta::not_found);
}

TEST(value_type_map_reversible)
{
	using rev1 = map2::reverse;
	static_assert(rev1::at<int> == 1, "");
	static_assert(rev1::at<abstract> == 2, "");
	using map = type_map<
		type_value_pair<int, 1>,
		type_value_pair<abstract, 2>
	     >;
	ASSERT_TYPE_EQ(rev1, map);
	// Not reversible: will static assert
	//using rev4 = map4::reverse; rev4::keys;
}

using map11 = type_map<type_type_pair<int, double>>;
using map12 = type_map<
		type_type_pair<int, int>,
		type_type_pair<int*, abstract>
	     >;
// Will static_assert
using map13 = type_map<type_type_pair<int, int>, type_type_pair<int, abstract>>;
using map14 = type_map<
		type_type_pair<void,    int>,
		type_type_pair<void*,   abstract>,
		type_type_pair<void**,  int>,
		type_type_pair<void***, float>
	     >;

TEST(type_type_map_looksup_ok)
{
	//LOGGER << meta::debug::to_string<map13::values>() << std::endl;
	ASSERT_TYPE_EQ(map11::at<int>, double);
	ASSERT_TYPE_EQ(map12::at<int>, int);
	ASSERT_TYPE_EQ(map12::at<int*>, abstract);
	// Will static assert
	//ASSERT_TYPE_EQ(map13::at<int>, int);
	ASSERT_TYPE_EQ(map14::at<void>, int);
	ASSERT_TYPE_EQ(map14::at<void*>, abstract);
	ASSERT_TYPE_EQ(map14::at<void**>, int);
	ASSERT_TYPE_EQ(map14::at<void***>, float);
}

TEST(type_type_map_looksup_fails)
{
	ASSERT_TYPE_EQ(map11::at<double>, meta::not_found);
}

using map21 = type_map<type_value_pair<int, 1>>;
using map22 = type_map<
		type_value_pair<int, 2>,
		type_value_pair<int*, 3>
	     >;
// Will static_assert
using map23 = type_map<type_value_pair<int, 1>, type_value_pair<int, 2>>;
using map24 = type_map<
		type_value_pair<void,    1>,
		type_value_pair<void*,   22>,
		type_value_pair<void**,  3>,
		type_value_pair<void***, 99>
	     >;

TEST(type_value_map_looksup_ok)
{
	static_assert(map21::at<int> == 1, "");
	static_assert(map22::at<int> == 2, "");
	static_assert(map22::at<int*> == 3, "");
	static_assert(map24::at<void> == 1, "");
	static_assert(map24::at<void*> == 22, "");
	static_assert(map24::at<void**> == 3, "");
	static_assert(map24::at<void***> == 99, "");
	// Will static assert
	//map23::at<int>;
	//map21::at<double>;
}

int main()
{
	return test::registry::run_all("type_map") ? EXIT_SUCCESS : EXIT_FAILURE;
}
