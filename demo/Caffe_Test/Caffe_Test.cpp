#include <iostream>
#include "funset.hpp"

int main()
{
	int ret = test_caffe_net1();

	if (0 == ret) fprintf(stderr, "test success\n");
	else fprintf(stderr, "test fail\n");

	return 0;
}
