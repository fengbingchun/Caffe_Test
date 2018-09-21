#include <iostream>
#include "boost_test.hpp"
#include "protobuf_test.hpp"
#include "glog_test.hpp"
#include "gflags_test.hpp"
#include "leveldb_test.hpp"
#include "lmdb_test.hpp"
#include "hdf5_test.hpp"
#include "snappy_test.hpp"
#include "openblas_test.hpp"

int main()
{
	// Blog: http://blog.csdn.net/fengbingchun/article/details/48768039
	int ret = test_openblas_1();

	if (ret == 0) fprintf(stdout, "====== test success ======\n");
	else fprintf(stderr, "###### test fail ######\n");

	return 0;
}
