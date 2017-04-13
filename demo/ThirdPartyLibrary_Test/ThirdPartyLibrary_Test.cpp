#include <iostream>
#include "boost_test.hpp"
#include "protobuf_test.hpp"
#include "glog_test.hpp"
#include "gflags_test.hpp"
#include "leveldb_test.hpp"
#include "lmdb_test.hpp"

int main()
{
	int ret = test_lmdb_1();

	if (ret == 0) fprintf(stderr, "=====  test success  =====\n");
	else fprintf(stderr, "*****  test fail  *****\n");

	return 0;
}
