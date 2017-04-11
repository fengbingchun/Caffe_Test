#include <iostream>
#include "boost_test.hpp"
#include "protobuf_test.hpp"

int main()
{
	int ret = test_protobuf_2();

	if (ret == 0) fprintf(stderr, "=====  test success  =====\n");
	else fprintf(stderr, "*****  test fail  *****\n");

	return 0;
}
