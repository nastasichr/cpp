#include "type_debug.h"
#include "type_list.h"
#include "testing.h"

using namespace std::string_literals;

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
	LOGGER << PRINT(sizeof(list0)) << std::endl;
	LOGGER << PRINT(sizeof(list1)) << std::endl;
	LOGGER << PRINT(sizeof(list2)) << std::endl;
	LOGGER << PRINT(sizeof(list3)) << std::endl;
	LOGGER << PRINT(sizeof(list4)) << std::endl;
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

TEST(type_list_at)
{
	static_assert(std::is_same<list1::at<0>, double>::value, "");
	static_assert(std::is_same<list2::at<0>, int>::value, "");
	static_assert(std::is_same<list3::at<1>, abstract>::value, "");
	static_assert(std::is_same<list4::at<3>, long long>::value, "");
}

enum {
	GENERIC_MATCH,
	LIST0_MATCH,
	LIST1_MATCH,
	LIST2_MATCH,
	LIST3_MATCH,
	LIST4_MATCH,
};

template<typename...T>
struct static_operation {
	static /*constexpr*/ int process() { return GENERIC_MATCH; }
};

template<>
struct static_operation<> {
	static constexpr int process() { return LIST0_MATCH; }
};

template<>
struct static_operation<double> {
	static constexpr int process() { return LIST1_MATCH; }
};

template<>
struct static_operation<int, int> {
	static constexpr int process() { return LIST2_MATCH; }
};

template<>
struct static_operation<int, abstract, int> {
	static constexpr int process() { return LIST3_MATCH; }
};

TEST(type_list_passes_types_to_static_operation_with_for_each)
{
	static_assert(list0::for_each<static_operation>() == LIST0_MATCH, "");
	static_assert(list1::for_each<static_operation>() == LIST1_MATCH, "");
	static_assert(list2::for_each<static_operation>() == LIST2_MATCH, "");
	static_assert(list3::for_each<static_operation>() == LIST3_MATCH, "");
	// This is not static_assert because the generic version is not constexpr
	ASSERT(list4::for_each<static_operation>() == GENERIC_MATCH);
}

struct operation {
	template<typename...T>
	constexpr int operator()() { return GENERIC_MATCH; }

	template<typename...T>
	constexpr auto some_function() { return nullptr; }
};

struct binder {
	operation& op;
	constexpr binder(operation& op) : op{op} {}

	template<typename...T>
	constexpr auto operator()() { return op.some_function<T...>(); }
};

struct printer {
	template<typename...T>
	constexpr void operator()()
	{
		LOGGER << "Number of types = " << sizeof...(T) << std::endl;
		if constexpr (sizeof...(T) > 0) {
		  LOGGER << "Sizes = " << ((" "s + std::to_string(sizeof(T))) + ...) << std::endl;
		}
	}
};

TEST(type_list_passes_types_to_function_object_with_for_each)
{
	operation op{};
	// Dispatch to operator() -> function object
	static_assert(list4::for_each(op) == GENERIC_MATCH, "");
	// Dispatch to some_function() -> binder acts as a pseudo template lambda
	static_assert(list4::for_each(binder{op}) == nullptr, "");

	list0::for_each(printer{});
	list1::for_each(printer{});
	list2::for_each(printer{});
	list3::for_each(printer{});
	list4::for_each(printer{});
}

TEST(type_list_passes_types_to_lambda_with_for_each_as_args)
{
	static_assert(list0::for_each_as_args([](auto...t) { return sizeof...(t) == 0; }), "");
	static_assert(list1::for_each_as_args([](auto...t) { return sizeof...(t) == 1; }), "");
	static_assert(list2::for_each_as_args([](auto...t) { return sizeof...(t) == 2; }), "");
	static_assert(list3::for_each_as_args([](auto...t) { return sizeof...(t) == 3; }), "");
	static_assert(list4::for_each_as_args([](auto...t) { return sizeof...(t) == 4; }), "");

	list3::for_each_as_args([](auto...t) {
		LOGGER << "Number of types = " << sizeof...(t) << std::endl;
		LOGGER << "Positions = " << ((" "s + std::to_string(list3::index_of<typename decltype(t)::type>)) + ...) << std::endl;
		LOGGER << "Sizes = " << ((" "s + std::to_string(sizeof(typename decltype(t)::type))) + ...) << std::endl;
	});
}


template<typename T> struct is_fat { static constexpr bool value = sizeof(T) > 512; };

TEST(type_list_find_if_finds_type_with_meta_codition)
{
	ASSERT_TYPE_EQ(list3::find_if<is_fat>, abstract);
	ASSERT_TYPE_EQ(list2::find_if<is_fat>, meta::not_found);
}

using meta::type_map;
using meta::value_type_pair;

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

int main()
{
	return test::registry::run_all("type_list") ? EXIT_SUCCESS : EXIT_FAILURE;
}
