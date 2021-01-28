#include "testing.h"
#include "ring.h"

using namespace container;

TEST(index_pre_increment)
{
	ring::index idx{3};

	ASSERT((int)++idx == 1);
	ASSERT((int)++idx == 2);
	ASSERT((int)++idx == 0);
	ASSERT((int)++idx == 1);
	ASSERT((int)++idx == 2);
	ASSERT((int)++idx == 0);
}

TEST(index_post_increment)
{
	ring::index idx{3};

	ASSERT((int)idx++ == 0);
	ASSERT((int)idx++ == 1);
	ASSERT((int)idx++ == 2);
	ASSERT((int)idx++ == 0);
	ASSERT((int)idx++ == 1);
	ASSERT((int)idx++ == 2);
}

TEST(index_plus)
{
	ring::index idx{10};

	ASSERT((int)(idx + 4) == 4);
	ASSERT((int)(idx + 8) == 8);
	ASSERT((int)(idx + 12) == 2);
}

TEST(vector_at_init)
{
	int data[3] = {22, 33, 198};
	ring::vector<int> v{data, 3};

	ASSERT(v[0] == data[0]);
	ASSERT(v[1] == data[1]);
	ASSERT(v[2] == data[2]);
	ASSERT(v[3] == data[0]);
	ASSERT(v[4] == data[1]);
	ASSERT(v[5] == data[2]);
}

TEST(vector_right_rotate)
{
	int data[3] = {22, 33, 198};
	ring::vector<int> v{data, 3};

	ASSERT(v[0] == data[0]);
	v >> 1;
	ASSERT(v[0] == data[1]);
	v >> 1;
	ASSERT(v[0] == data[2]);
	v >> 1;
	ASSERT(v[0] == data[0]);
	v >> 2;
	ASSERT(v[0] == data[2]);
}

int main()
{
	return test::registry::run_all("container_ring") ? EXIT_SUCCESS : EXIT_FAILURE;
}
