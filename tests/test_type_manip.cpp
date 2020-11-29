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
	static_assert(v_int != v_double, "");
	static_assert(v_int != v_int_ref, "");
	static_assert(v_int_ref != v_double, "");
}

int main()
{
	return test::registry::run_all("type_manip") ? EXIT_SUCCESS : EXIT_FAILURE;
}
