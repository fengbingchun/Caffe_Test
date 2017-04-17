#include "openblas_test.hpp"
#include <iostream>
#include <cblas.h>

int test_openblas_1()
{
	int th_model = openblas_get_parallel();
	switch (th_model) {
	case OPENBLAS_SEQUENTIAL:
		printf("OpenBLAS is compiled sequentially.\n");
		break;
	case OPENBLAS_THREAD:
		printf("OpenBLAS is compiled using the normal threading model\n");
		break;
	case OPENBLAS_OPENMP:
		printf("OpenBLAS is compiled using OpenMP\n");
		break;
	}

	int n = 2;
	double* x = (double*)malloc(n*sizeof(double));
	double* upperTriangleResult = (double*)malloc(n*(n + 1)*sizeof(double) / 2);

	for (int j = 0; j<n*(n + 1) / 2; j++)
		upperTriangleResult[j] = 0;

	x[0] = 1; x[1] = 3;

	cblas_dspr(CblasRowMajor, CblasUpper, n, 1, x, 1, upperTriangleResult);
	double*& A = upperTriangleResult;
	std::cout << A[0] << "\t" << A[1] << std::endl << "*\t" << A[2] << std::endl;

	free(upperTriangleResult);
	free(x);

	return 0;
}
