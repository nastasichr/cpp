#include "function_binder.h"
#include "type_debug.h"
#include "testing.h"

using namespace std::string_literals;
using container::sized_function;
using container::small_function;

double f1() { return 1.0; }
void   f2(int, double& d) { d = 2.0;}

struct A {
	//double m1()				{ return 1.1; }
	//void   m2(int, double& d)		{ d = 2.2; }
	//double c1() const			{ return 1.11; }
	//void   c2(int, double& d) const		{ d = 2.22; }
	static double s1()			{ return 1.111; }
	static void   s2(int, double& d)	{ d = 2.222; }
};

TEST(function_binds_to_normal_function)
{
	small_function<double()> f = &f1;
	ASSERT(f() == 1.0);

	small_function<void(int, double&)> g = &f2;
	double v = 0;
	g(22, v);
	ASSERT(v == 2.0);
}

TEST(function_binds_to_static_member_function)
{
	small_function<double()> f = &A::s1;
	ASSERT(f() == 1.111);

	small_function<void(int, double&)> g = &A::s2;
	double v = 0;
	g(22, v);
	ASSERT(v == 2.222);
}

TEST(function_binds_to_lambda)
{
	small_function<double()> f = [](){return 99.99;};
	ASSERT(f() == 99.99);

	small_function<void(int, double&)> g = [](int, double& d){ d = 12.21;};
	double v = 0;
	g(22, v);
	ASSERT(v == 12.21);
}

int main()
{
	return test::registry::run_all("container_function") ? EXIT_SUCCESS : EXIT_FAILURE;
}
