#include <iostream>
#include "funset.hpp"

int main()
{
	int ret = test_caffe_layer_pooling();

	if (0 == ret) fprintf(stderr, "test success\n");
	else fprintf(stderr, "test fail\n");

	return 0;
}
