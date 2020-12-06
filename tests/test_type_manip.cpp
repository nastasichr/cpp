#include "type_manip.h"
#include "testing.h"

using meta::make_unique_id;

TEST(make_type_uid_generates_same_id)
{
	constexpr auto v_int1 = make_unique_id<int>;
	constexpr auto v_int2 = make_unique_id<int>;
	static_assert(v_int1 == v_int2, "");
}

TEST(make_type_uid_gives_unique_ids)
{
	constexpr auto v_int     = make_unique_id<int>;
	constexpr auto v_double  = make_unique_id<double>;
	constexpr auto v_int_ref = make_unique_id<int&>;
	ASSERT(v_int != v_double);
	ASSERT(v_int != v_int_ref);
	ASSERT(v_int_ref != v_double);
}

TEST(max_returns_norm_of_largest_item_in_pack)
{
	using t_max = typename meta::max<meta::size_of, char, int, long long>::type;
	constexpr auto max = meta::max<meta::size_of, char, int, long long>::value;
	static_assert(max == sizeof(long long), "");
	static_assert(std::is_same<t_max, long long>::value, "");
	//meta::max<meta::size_of>::value;  // Will static assert
}

int main()
{
	return test::registry::run_all("type_manip") ? EXIT_SUCCESS : EXIT_FAILURE;
}
