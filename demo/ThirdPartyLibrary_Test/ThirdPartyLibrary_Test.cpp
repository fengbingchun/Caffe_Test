#include <iostream>
#include "boost_test.hpp"

int main()
{
	int ret = test_boost_shared_ptr();

	if (ret == 0) fprintf(stderr, "test success\n");
	else fprintf(stderr, "test fail\n");

	return 0;
}
