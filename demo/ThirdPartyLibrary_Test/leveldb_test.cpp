#include "leveldb_test.hpp"
#include <iostream>
#include <leveldb/db.h>

int test_leveldb_1()
{
	/* reference:
		http://www.cnblogs.com/haippy/archive/2011/12/04/2276064.html
		http://www.bubuko.com/infodetail-411090.html
		http://qiuqiang1985.iteye.com/blog/1255365
	*/
#ifdef _MSC_VER
	const std::string data_path = "E:/GitCode/Caffe_Test/test_data/third_party_library/";
#else
	const std::string data_path = "test_data/third_party_library/";
#endif

	leveldb::DB* db;
	leveldb::Options options;
	options.create_if_missing = true;
	std::string file_LevelDB = data_path + "testLevelDB";
	leveldb::Status status = leveldb::DB::Open(options, file_LevelDB.c_str(), &db);
	assert(status.ok());

	//write key1,value1
	std::string key = "key";
	std::string value = "value";

	//write
	status = db->Put(leveldb::WriteOptions(), key, value);
	assert(status.ok());

	//read
	status = db->Get(leveldb::ReadOptions(), key, &value);
	assert(status.ok());
	std::cout << value << std::endl;
	std::string key2 = "key2";

	//move the value under key to key2
	status = db->Put(leveldb::WriteOptions(), key2, value);
	assert(status.ok());
	//delete
	status = db->Delete(leveldb::WriteOptions(), key);
	assert(status.ok());

	status = db->Get(leveldb::ReadOptions(), key2, &value);
	assert(status.ok());
	std::cout << key2 << "===" << value << std::endl;

	status = db->Get(leveldb::ReadOptions(), key, &value);
	if (!status.ok())
		std::cerr << key << "  " << status.ToString() << std::endl;
	else
		std::cout << key << "===" << value << std::endl;

	delete db;

	return 0;
}

