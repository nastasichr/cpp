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

	LOGGER << PRINT((int)++idx) << std::endl;
	LOGGER << PRINT((int)++idx) << std::endl;
	LOGGER << PRINT((int)++idx) << std::endl;
	LOGGER << PRINT((int)++idx) << std::endl;
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

	LOGGER << PRINT((int)idx++) << std::endl;
	LOGGER << PRINT((int)idx++) << std::endl;
	LOGGER << PRINT((int)idx++) << std::endl;
	LOGGER << PRINT((int)idx++) << std::endl;
}

int main()
{
	return test::registry::run_all("container_ring") ? EXIT_SUCCESS : EXIT_FAILURE;
}
