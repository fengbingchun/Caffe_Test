#include <iostream>
#include "funset.hpp"

int main()
{
	int ret = cifar10_compute_image_mean();

	if (0 == ret) fprintf(stderr, "===== test success =====\n");
	else fprintf(stderr, "***** test fail *****\n");

	return 0;
}
