#include "snappy_test.hpp"
#include <iostream>
#include <string>
#include <snappy-c.h>

int test_snappy_1()
{
	/* reference:
		http://baike.baidu.com/item/snappy?fr=aladdin
		exaples:./snappy_unittest.cc、snappy-test.cc
		https://snappy.angeloflogic.com/cpp-tutorial/
	*/

	std::string file_name{ "Hello World!" };
	size_t file_size = file_name.length();

	size_t compressed_length = snappy_max_compressed_length(file_size);
	char* compressed = new char[compressed_length];
	snappy_status status = snappy_compress(file_name.c_str(), file_size, compressed, &compressed_length);
	if (status != SNAPPY_OK) {
		std::cout << "snappy compress fail!" << std::endl;
	}

	size_t uncompressed_length{0};
	status = snappy_uncompressed_length(compressed, compressed_length, &uncompressed_length);
	char* uncompressed = new char[uncompressed_length];
	status = snappy_uncompress(compressed, compressed_length, uncompressed, &uncompressed_length);
	if (status != SNAPPY_OK) {
		std::cout << "snappy uncompress fail!" << std::endl;
	}

	delete[] compressed;
	delete[] uncompressed;

	return 0;
}
