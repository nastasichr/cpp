#include "function_traits.h"
#include "testing.h"

using namespace std::string_literals;
using meta::parse_function;

using R1 = void;
using R2 = int;
using R3 = double;
using R4 = std::string;
using R5 = std::string&;

R1 f1();
R2 f2(int, int);
R3 f3(int, int, int);
R4 f4(int, int, int, int);
R5 f5();

struct A {
	static R1 s1();
	static R2 s2();
	static R3 s3();
	static R4 s4();
	static R5 s5();
	
	R1 m1();
	R2 m2();
	R3 m3();
	R4 m4();
	R5 m5();
	
	R1 c1() const;
	R2 c2() const;
	R3 c3() const;
	R4 c4() const;
	R5 c5() const;
};

struct F1 { R1 operator()(); };
struct F2 { R2 operator()(); };
struct F3 { R3 operator()(); };
struct F4 { R4 operator()(); };
struct F5 { R5 operator()(); };

auto lambda1 = []()->R1{};
auto lambda2 = [](int, int)->R2{return 1;};
auto lambda3 = [](int, int, int)->R3{return 2.0;};
auto lambda4 = [](float, int, double, char)->R4{return "";};

TEST(is_function_object_recognises_function_objects)
{
	static_assert(meta::is_function_object<F1>::value, "");
	static_assert(meta::is_function_object<decltype(lambda1)>::value, "");
	(void)lambda1;
	(void)lambda2;
	(void)lambda3;
	(void)lambda4;
}

TEST(parse_function_gets_return_type_from_function)
{
	ASSERT_TYPE_EQ(parse_function<decltype(f1)>::ret_type, R1);
	ASSERT_TYPE_EQ(parse_function<decltype(f2)>::ret_type, R2);
	ASSERT_TYPE_EQ(parse_function<decltype(f3)>::ret_type, R3);
	ASSERT_TYPE_EQ(parse_function<decltype(f4)>::ret_type, R4);
	ASSERT_TYPE_EQ(parse_function<decltype(f5)>::ret_type, R5);
}	

TEST(parse_function_gets_return_type_from_function_pointer)
{
	ASSERT_TYPE_EQ(parse_function<decltype(&f1)>::ret_type, R1);
	ASSERT_TYPE_EQ(parse_function<decltype(&f2)>::ret_type, R2);
	ASSERT_TYPE_EQ(parse_function<decltype(&f3)>::ret_type, R3);
	ASSERT_TYPE_EQ(parse_function<decltype(&f4)>::ret_type, R4);
	ASSERT_TYPE_EQ(parse_function<decltype(&f5)>::ret_type, R5);
}

TEST(parse_function_gets_return_type_from_static_member_function)
{
	ASSERT_TYPE_EQ(parse_function<decltype(A::s1)>::ret_type, R1);
	ASSERT_TYPE_EQ(parse_function<decltype(A::s2)>::ret_type, R2);
	ASSERT_TYPE_EQ(parse_function<decltype(A::s3)>::ret_type, R3);
	ASSERT_TYPE_EQ(parse_function<decltype(A::s4)>::ret_type, R4);
	ASSERT_TYPE_EQ(parse_function<decltype(A::s5)>::ret_type, R5);
}

TEST(parse_function_gets_return_type_from_static_member_function_pointer)
{
	ASSERT_TYPE_EQ(parse_function<decltype(&A::s1)>::ret_type, R1);
	ASSERT_TYPE_EQ(parse_function<decltype(&A::s2)>::ret_type, R2);
	ASSERT_TYPE_EQ(parse_function<decltype(&A::s3)>::ret_type, R3);
	ASSERT_TYPE_EQ(parse_function<decltype(&A::s4)>::ret_type, R4);
	ASSERT_TYPE_EQ(parse_function<decltype(&A::s5)>::ret_type, R5);
}

TEST(parse_function_gets_return_type_from_member_function_pointer)
{
	ASSERT_TYPE_EQ(parse_function<decltype(&A::m1)>::ret_type, R1);
	ASSERT_TYPE_EQ(parse_function<decltype(&A::m2)>::ret_type, R2);
	ASSERT_TYPE_EQ(parse_function<decltype(&A::m3)>::ret_type, R3);
	ASSERT_TYPE_EQ(parse_function<decltype(&A::m4)>::ret_type, R4);
	ASSERT_TYPE_EQ(parse_function<decltype(&A::m5)>::ret_type, R5);
}

TEST(parse_function_gets_return_type_from_function_object)
{
	ASSERT_TYPE_EQ(parse_function<F1>::ret_type, R1);
	ASSERT_TYPE_EQ(parse_function<F2>::ret_type, R2);
	ASSERT_TYPE_EQ(parse_function<F3>::ret_type, R3);
	ASSERT_TYPE_EQ(parse_function<F4>::ret_type, R4);
	ASSERT_TYPE_EQ(parse_function<F5>::ret_type, R5);
}

TEST(parse_function_gets_return_type_from_lambda_closure)
{
	ASSERT_TYPE_EQ(parse_function<decltype(lambda1)>::ret_type, R1);
	ASSERT_TYPE_EQ(parse_function<decltype(lambda2)>::ret_type, R2);
	ASSERT_TYPE_EQ(parse_function<decltype(lambda3)>::ret_type, R3);
	ASSERT_TYPE_EQ(parse_function<decltype(lambda4)>::ret_type, R4);
}

template<typename R, typename... Args>
struct inspect_args {
	static constexpr size_t count = sizeof...(Args);
};

TEST(parse_function_applies_given_metafunction)
{
	static_assert(parse_function<decltype(f1), inspect_args>::apply::count == 0, "");
	static_assert(parse_function<decltype(f2), inspect_args>::apply::count == 2, "");
	static_assert(parse_function<decltype(f3), inspect_args>::apply::count == 3, "");
	static_assert(parse_function<decltype(f4), inspect_args>::apply::count == 4, "");

	static_assert(parse_function<decltype(lambda1), inspect_args>::apply::count == 0, "");
	static_assert(parse_function<decltype(lambda2), inspect_args>::apply::count == 2, "");
	static_assert(parse_function<decltype(lambda3), inspect_args>::apply::count == 3, "");
	static_assert(parse_function<decltype(lambda4), inspect_args>::apply::count == 4, "");
}

int main()
{
	return test::registry::run_all("function_traits") ? EXIT_SUCCESS : EXIT_FAILURE;
}
