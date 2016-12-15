#include "funset.hpp"
#include <string>
#include <assert.h>
#include <fstream>
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <gflags/gflags.h>
#include <glog/logging.h>
#include <levelDB/db.h>
#include <lmdb.h>
#include <hdf5.h>
#include <snappy-c.h>

#include <windows.h>
#include "ml.helloworld.pb.h"
#include "caffe_tmp.pb.h"

static const std::string data_path = "E:/GitCode/Caffe_Test/test_data/thrid_library/";

class implementation {
public:
	~implementation() { std::cout << "destroying implementation\n"; }
	void do_something() { std::cout << "do something\n"; }
};

int test_Boost()
{
	// reference: http://www.cnblogs.com/tianfang/archive/2008/09/19/1294521.html
	boost::shared_ptr<implementation> sp1(new implementation());
	std::cout << "The Sample now has " << sp1.use_count() << " references\n";

	boost::shared_ptr<implementation> sp2 = sp1;
	std::cout << "The Sample now has " << sp2.use_count() << " references\n";

	sp1.reset();
	std::cout << "After Reset sp1. The Sample now has " << sp2.use_count() << " references\n";

	sp2.reset();
	std::cout << "After Reset sp2.\n";

	return 0;
}

DEFINE_bool(big_menu, true, "Include 'advanced' options in the menu listing");
DEFINE_string(languages, "english,french,german", "comma-separated list of languages to offer in the 'lang' menu");

int test_GFlags()
{
	/* reference: http://dreamrunner.org/blog/2014/03/09/gflags-jian-ming-shi-yong/
                      http://www.leoox.com/?p=270
	*/
	int tmp_argc = 3;
	char** tmp_argv = NULL;
	tmp_argv = new char*[3];
	tmp_argv[0] = "";
	tmp_argv[1] = "--big_menu=false";
	tmp_argv[2] = "--languages=chinese";

	//google::ParseCommandLineFlags(&argc, &argv, true);
	google::ParseCommandLineFlags(&tmp_argc, &tmp_argv, true);

	std::cout << "argc=" << tmp_argc << std::endl;
	if (FLAGS_big_menu) {
		std::cout << "big menu is ture" << std::endl;
	}
	else {
		std::cout << "big menu is flase" << std::endl;
	}

	std::cout << "languages=" << FLAGS_languages << std::endl;

	return 0;
}

void thread1_test()
{
	std::string strTmp = "thread1_test";
	for (int i = 0; i< 1000; i++) {
		//LOG(INFO) << i;
		LOG_IF(INFO, i < 10) << i;
		//CHECK_EQ(i, 100) << "error!";
		//LOG(INFO) << strTmp;
		//Sleep(10);
	}
}

void thread2_test()
{
	std::string strTmp = "thread2_test";
	for (int i = 1000; i< 2000; i++) {
		//LOG(INFO) << i;
		LOG_IF(INFO, i < 1100) << i;
		//LOG(INFO) << strTmp;
		//Sleep(10);
	}
}

int test_GLog()
{
	/* reference: http://www.yeolar.com/note/2014/12/20/glog/
                      http://www.cppblog.com/pizzx/archive/2014/06/18/207320.aspx
	*/
#ifdef _DEBUG
	const char* exe = "E:/GitCode/Caffe_Test/lib/dbg/x64_vc12/testThridLibrary.exe";
#else
	const char* exe = "E:/GitCode/Caffe_Test/lib/rel/x64_vc12/testThridLibrary.exe";
#endif
	//Initialize Google's logging library.
	//google::InitGoogleLogging(argv[0]);
	google::InitGoogleLogging(exe);

	//为不同级别的日志设置不同的文件basename
	std::string file_info = data_path + "loginfo";
	std::string file_logwarn = data_path + "logwarn";
	std::string file_logerror = data_path + "logerror";
	google::SetLogDestination(google::INFO, file_info.c_str());
	google::SetLogDestination(google::WARNING, file_logwarn.c_str());
	google::SetLogDestination(google::GLOG_ERROR, file_logerror.c_str());

	//缓存的最大时长，超时会写入文件
	FLAGS_logbufsecs = 60;

	//单个日志文件最大，单位M
	FLAGS_max_log_size = 10;

	//设置为true，就不会写日志文件了
	FLAGS_logtostderr = false;
	boost::thread t1(boost::bind(&thread1_test));
	boost::thread t2(boost::bind(&thread2_test));

	t1.join();
	t2.join();

	//LOG(FATAL)<<"exit";
	google::ShutdownGoogleLogging();

	return 0;
}

int test_LevelDB()
{
	/* reference: http://www.cnblogs.com/haippy/archive/2011/12/04/2276064.html
                      http://www.bubuko.com/infodetail-411090.html
                      http://qiuqiang1985.iteye.com/blog/1255365
	*/
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

#define E(expr) CHECK((rc = (expr)) == MDB_SUCCESS, #expr)
#define RES(err, expr) ((rc = expr) == (err) || (CHECK(!rc, #expr), 0))
#define CHECK(test, msg) ((test) ? (void)0 : ((void)fprintf(stderr, \
	"%s:%d: %s: %s\n", __FILE__, __LINE__, msg, mdb_strerror(rc)), abort()))

int test_LMDB()
{
	/* reference: http://www.jianshu.com/p/yzFf8j
                      ./lmdb-mdb.master/libraries/liblmdb/mtest.c
	*/
	int i = 0, j = 0, rc;
	MDB_env *env;
	MDB_dbi dbi;
	MDB_val key, data;
	MDB_txn *txn;
	MDB_stat mst;
	MDB_cursor *cursor, *cur2;
	MDB_cursor_op op;
	int count;
	int *values;
	char sval[32] = "";

	srand(time(NULL));

	count = (rand() % 384) + 64;
	values = (int *)malloc(count*sizeof(int));

	for (i = 0; i<count; i++) {
		values[i] = rand() % 1024;
	}

	std::string file_LMDB = data_path + "testLMDB";//数据库文件隶属的文件夹，文件夹必须存在而且是可读的
	E(mdb_env_create(&env));
	E(mdb_env_set_maxreaders(env, 1));
	E(mdb_env_set_mapsize(env, 107374182));//100MB
	//E(mdb_env_open(env, "./testdb", MDB_FIXEDMAP /*|MDB_NOSYNC*/, 0664));
	E(mdb_env_open(env, file_LMDB.c_str(), 0/*MDB_FIXEDMAP*/ /*|MDB_NOSYNC*/, 0664));

	E(mdb_txn_begin(env, NULL, 0, &txn));
	E(mdb_dbi_open(txn, NULL, 0, &dbi));

	key.mv_size = sizeof(int);
	key.mv_data = sval;

	printf("Adding %d values\n", count);
	for (i = 0; i<count; i++) {
		sprintf(sval, "%03x %d foo bar", values[i], values[i]);
		/* Set <data> in each iteration, since MDB_NOOVERWRITE may modify it */
		data.mv_size = sizeof(sval);
		data.mv_data = sval;
		if (RES(MDB_KEYEXIST, mdb_put(txn, dbi, &key, &data, MDB_NOOVERWRITE))) {
			j++;
			data.mv_size = sizeof(sval);
			data.mv_data = sval;
		}
	}
	if (j) printf("%d duplicates skipped\n", j);
	E(mdb_txn_commit(txn));
	E(mdb_env_stat(env, &mst));

	E(mdb_txn_begin(env, NULL, MDB_RDONLY, &txn));
	E(mdb_cursor_open(txn, dbi, &cursor));
	while ((rc = mdb_cursor_get(cursor, &key, &data, MDB_NEXT)) == 0) {
		printf("key: %p %.*s, data: %p %.*s\n",
			key.mv_data, (int)key.mv_size, (char *)key.mv_data,
			data.mv_data, (int)data.mv_size, (char *)data.mv_data);
	}
	CHECK(rc == MDB_NOTFOUND, "mdb_cursor_get");
	mdb_cursor_close(cursor);
	mdb_txn_abort(txn);

	j = 0;
	key.mv_data = sval;
	for (i = count - 1; i > -1; i -= (rand() % 5)) {
		j++;
		txn = NULL;
		E(mdb_txn_begin(env, NULL, 0, &txn));
		sprintf(sval, "%03x ", values[i]);
		if (RES(MDB_NOTFOUND, mdb_del(txn, dbi, &key, NULL))) {
			j--;
			mdb_txn_abort(txn);
		}
		else {
			E(mdb_txn_commit(txn));
		}
	}
	free(values);
	printf("Deleted %d values\n", j);

	E(mdb_env_stat(env, &mst));
	E(mdb_txn_begin(env, NULL, MDB_RDONLY, &txn));
	E(mdb_cursor_open(txn, dbi, &cursor));
	printf("Cursor next\n");
	while ((rc = mdb_cursor_get(cursor, &key, &data, MDB_NEXT)) == 0) {
		printf("key: %.*s, data: %.*s\n",
			(int)key.mv_size, (char *)key.mv_data,
			(int)data.mv_size, (char *)data.mv_data);
	}
	CHECK(rc == MDB_NOTFOUND, "mdb_cursor_get");
	printf("Cursor last\n");
	E(mdb_cursor_get(cursor, &key, &data, MDB_LAST));
	printf("key: %.*s, data: %.*s\n",
		(int)key.mv_size, (char *)key.mv_data,
		(int)data.mv_size, (char *)data.mv_data);
	printf("Cursor prev\n");
	while ((rc = mdb_cursor_get(cursor, &key, &data, MDB_PREV)) == 0) {
		printf("key: %.*s, data: %.*s\n",
			(int)key.mv_size, (char *)key.mv_data,
			(int)data.mv_size, (char *)data.mv_data);
	}
	CHECK(rc == MDB_NOTFOUND, "mdb_cursor_get");
	printf("Cursor last/prev\n");
	E(mdb_cursor_get(cursor, &key, &data, MDB_LAST));
	printf("key: %.*s, data: %.*s\n",
		(int)key.mv_size, (char *)key.mv_data,
		(int)data.mv_size, (char *)data.mv_data);
	E(mdb_cursor_get(cursor, &key, &data, MDB_PREV));
	printf("key: %.*s, data: %.*s\n",
		(int)key.mv_size, (char *)key.mv_data,
		(int)data.mv_size, (char *)data.mv_data);

	mdb_cursor_close(cursor);
	mdb_txn_abort(txn);

	printf("Deleting with cursor\n");
	E(mdb_txn_begin(env, NULL, 0, &txn));
	E(mdb_cursor_open(txn, dbi, &cur2));
	for (i = 0; i<50; i++) {
		if (RES(MDB_NOTFOUND, mdb_cursor_get(cur2, &key, &data, MDB_NEXT)))
			break;
		printf("key: %p %.*s, data: %p %.*s\n",
			key.mv_data, (int)key.mv_size, (char *)key.mv_data,
			data.mv_data, (int)data.mv_size, (char *)data.mv_data);
		E(mdb_del(txn, dbi, &key, NULL));
	}

	printf("Restarting cursor in txn\n");
	for (op = MDB_FIRST, i = 0; i <= 32; op = MDB_NEXT, i++) {
		if (RES(MDB_NOTFOUND, mdb_cursor_get(cur2, &key, &data, op)))
			break;
		printf("key: %p %.*s, data: %p %.*s\n",
			key.mv_data, (int)key.mv_size, (char *)key.mv_data,
			data.mv_data, (int)data.mv_size, (char *)data.mv_data);
	}
	mdb_cursor_close(cur2);
	E(mdb_txn_commit(txn));

	printf("Restarting cursor outside txn\n");
	E(mdb_txn_begin(env, NULL, 0, &txn));
	E(mdb_cursor_open(txn, dbi, &cursor));
	for (op = MDB_FIRST, i = 0; i <= 32; op = MDB_NEXT, i++) {
		if (RES(MDB_NOTFOUND, mdb_cursor_get(cursor, &key, &data, op)))
			break;
		printf("key: %p %.*s, data: %p %.*s\n",
			key.mv_data, (int)key.mv_size, (char *)key.mv_data,
			data.mv_data, (int)data.mv_size, (char *)data.mv_data);
	}
	mdb_cursor_close(cursor);
	mdb_txn_abort(txn);

	mdb_dbi_close(env, dbi);
	mdb_env_close(env);

	return 0;
}

#define H5FILE_NAME		"E:/GitCode/Caffe_Test/test_data/thrid_library/SDS.h5"
#define DATASETNAME		"IntArray"
#define NX			5 /* dataset dimensions */
#define NY			6
#define RANK			2

static int test_HDF5_write_HDF5_Data()
{
	hid_t       file, dataset;         /* file and dataset handles */
	hid_t       datatype, dataspace;   /* handles */
	hsize_t     dimsf[2];              /* dataset dimensions */
	herr_t      status;
	int         data[NX][NY];          /* data to write */
	int         i, j;

	//Data  and output buffer initialization.
	for (j = 0; j < NX; j++)
		for (i = 0; i < NY; i++)
			data[j][i] = i + j + 100;//changed
	/*
	* 0 1 2 3 4 5
	* 1 2 3 4 5 6
	* 2 3 4 5 6 7
	* 3 4 5 6 7 8
	* 4 5 6 7 8 9
	*/

	/*
	* Create a new file using H5F_ACC_TRUNC access,
	* default file creation properties, and default file
	* access properties.
	*/
	file = H5Fcreate(H5FILE_NAME, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

	/*
	* Describe the size of the array and create the data space for fixed
	* size dataset.
	*/
	dimsf[0] = NX;
	dimsf[1] = NY;
	dataspace = H5Screate_simple(RANK, dimsf, NULL);

	/*
	* Define datatype for the data in the file.
	* We will store little endian INT numbers.
	*/
	datatype = H5Tcopy(H5T_NATIVE_INT);
	status = H5Tset_order(datatype, H5T_ORDER_LE);

	/*
	* Create a new dataset within the file using defined dataspace and
	* datatype and default dataset creation properties.
	*/
	dataset = H5Dcreate2(file, DATASETNAME, datatype, dataspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

	//Write the data to the dataset using default transfer properties.
	status = H5Dwrite(dataset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);

	//Close/release resources.
	H5Sclose(dataspace);
	H5Tclose(datatype);
	H5Dclose(dataset);
	H5Fclose(file);

	return 0;
}

#define NX_SUB			3 /* hyperslab dimensions */
#define NY_SUB			4
#define NX			7 /* output buffer dimensions */
#define NY			7
#define NZ			3
#define RANK			2
#define RANK_OUT		3

static int test_HDF5_read_HDF5_data()
{
	hid_t       file, dataset;         /* handles */
	hid_t       datatype, dataspace;
	hid_t       memspace;
	H5T_class_t t_class;                 /* data type class */
	H5T_order_t order;                 /* data order */
	size_t      size;                  /*
					   * size of the data element
					   * stored in file
					   */
	hsize_t     dimsm[3];              /* memory space dimensions */
	hsize_t     dims_out[2];           /* dataset dimensions */
	herr_t      status;

	int         data_out[NX][NY][NZ]; /* output buffer */

	hsize_t      count[2];              /* size of the hyperslab in the file */
	hsize_t      offset[2];             /* hyperslab offset in the file */
	hsize_t      count_out[3];          /* size of the hyperslab in memory */
	hsize_t      offset_out[3];         /* hyperslab offset in memory */
	int          i, j, k, status_n, rank;

	for (j = 0; j < NX; j++) {
		for (i = 0; i < NY; i++) {
			for (k = 0; k < NZ; k++)
				data_out[j][i][k] = 0 - 1000;//changed
		}
	}

	/*
	* Open the file and the dataset.
	*/
	file = H5Fopen(H5FILE_NAME, H5F_ACC_RDONLY, H5P_DEFAULT);
	dataset = H5Dopen2(file, DATASETNAME, H5P_DEFAULT);

	/*
	* Get datatype and dataspace handles and then query
	* dataset class, order, size, rank and dimensions.
	*/
	datatype = H5Dget_type(dataset);     /* datatype handle */
	t_class = H5Tget_class(datatype);
	if (t_class == H5T_INTEGER) printf("Data set has INTEGER type \n");
	order = H5Tget_order(datatype);
	if (order == H5T_ORDER_LE) printf("Little endian order \n");

	size = H5Tget_size(datatype);
	printf("Data size is %d \n", (int)size);

	dataspace = H5Dget_space(dataset);    /* dataspace handle */
	rank = H5Sget_simple_extent_ndims(dataspace);
	status_n = H5Sget_simple_extent_dims(dataspace, dims_out, NULL);
	printf("rank %d, dimensions %lu x %lu \n", rank,
		(unsigned long)(dims_out[0]), (unsigned long)(dims_out[1]));

	/*
	* Define hyperslab in the dataset.
	*/
	offset[0] = 1;
	offset[1] = 2;
	count[0] = NX_SUB;
	count[1] = NY_SUB;
	status = H5Sselect_hyperslab(dataspace, H5S_SELECT_SET, offset, NULL,
		count, NULL);

	/*
	* Define the memory dataspace.
	*/
	dimsm[0] = NX;
	dimsm[1] = NY;
	dimsm[2] = NZ;
	memspace = H5Screate_simple(RANK_OUT, dimsm, NULL);

	/*
	* Define memory hyperslab.
	*/
	offset_out[0] = 3;
	offset_out[1] = 0;
	offset_out[2] = 0;
	count_out[0] = NX_SUB;
	count_out[1] = NY_SUB;
	count_out[2] = 1;
	status = H5Sselect_hyperslab(memspace, H5S_SELECT_SET, offset_out, NULL,
		count_out, NULL);

	/*
	* Read data from hyperslab in the file into the hyperslab in
	* memory and display.
	*/
	status = H5Dread(dataset, H5T_NATIVE_INT, memspace, dataspace,
		H5P_DEFAULT, data_out);
	for (j = 0; j < NX; j++) {
		for (i = 0; i < NY; i++) printf("%d ", data_out[j][i][0]);
		printf("\n");
	}
	/*
	* 0 0 0 0 0 0 0
	* 0 0 0 0 0 0 0
	* 0 0 0 0 0 0 0
	* 3 4 5 6 0 0 0
	* 4 5 6 7 0 0 0
	* 5 6 7 8 0 0 0
	* 0 0 0 0 0 0 0
	*/

	/*
	* Close/release resources.
	*/
	H5Tclose(datatype);
	H5Dclose(dataset);
	H5Sclose(dataspace);
	H5Sclose(memspace);
	H5Fclose(file);

	return 0;
}

int test_HDF5()
{
	/* reference: http://wenku.baidu.com/link?url=HDnqbrqqJ27GSvVGTcCbfM-bn5K2QCYxSlqTEtY_jwFvBVi3B97DK6s9qBUwXDjVgMHFQq-MLGSKcMKeGJkq87GF_8vchhsleRWISq9PwO3
                      http://baike.baidu.com/link?url=TqYZDUzu_XFMYa9XswMS1OVSyboWzu3RtK6L-DiOFZT6zugtXjBUIFa4QHerxZcSbPNuTO84BomEGgxpchWojK
                      http://www.docin.com/p-608918978.html
	*/
	// 1-->./examples/h5_write.c:This example writes data to the HDF5 file
	test_HDF5_write_HDF5_Data();
	// 2-->./examples/h5_read.c:This example reads hyperslab from the SDS.h5 file
	test_HDF5_read_HDF5_data();

	return 0;
}

int test_Snappy()
{
	/* reference: http://baike.baidu.com/link?url=X8PCUvwS0MFJF5xS2DdzMrVDj9hNV8VsXL40W_jgiI1DeGNW5q5PsfEbL9RwUSrIilseenbFiulT1ceONYL5E_
                      exaples:./snappy_unittest.cc、snappy-test.cc
                      https://snappy.angeloflogic.com/cpp-tutorial/
	*/

	std::string filename = data_path + "fireworks.jpeg";
	size_t input_length = 200;
	snappy_status status;
	size_t output_length = snappy_max_compressed_length(input_length);
	char* output = (char*)malloc(output_length);
	status = snappy_compress(filename.c_str(), input_length, output, &output_length);
	if (status != SNAPPY_OK) {
		std::cout << "snappy compress fail!" << std::endl;
	}
	free(output);

	size_t output_length1;
	snappy_status status1;
	status1 = snappy_uncompressed_length(filename.c_str(), input_length, &output_length1);
	if (status != SNAPPY_OK) {
		std::cout << "get snappy uncompress length fail!" << std::endl;
	}
	char* output1 = (char*)malloc(output_length1);
	status1 = snappy_uncompress(filename.c_str(), input_length, output1, &output_length1);
	if (status != SNAPPY_OK) {
		std::cout << "snappy uncompress fail!" << std::endl;
	}
	free(output1);

	return 0;
}

void ListMsg(const lm::helloworld& msg) {
	std::cout << msg.id() << std::endl;
	std::cout << msg.str() << std::endl;
}

int test_ProtoBuf()
{
	/* reference: http://www.ibm.com/developerworks/cn/linux/l-cn-gpb/
                      http://blog.163.com/jiang_tao_2010/blog/static/12112689020114305013458/
                      http://www.cnblogs.com/dkblog/archive/2012/03/27/2419010.html
	*/
	// 1-->首先编写一个ml.helloworld.proto文件，内容如下：
	/*
	syntax = "proto2";
	package lm;

	message helloworld
	{
	required int32     id = 1;  // ID
	required string    str = 2;  // str
	optional int32     opt = 3;  //optional field
	}
	*/

	// 2-->利用protoc.exe生成ml.helloworld.pb.h和ml.hellowrold.ph.cc

	// 3-->Writer,将把一个结构化数据写入磁盘，以便其他人来读取
	lm::helloworld msg1;
	msg1.set_id(101);
	msg1.set_str("hello");

	// Write the new address book back to disk. 
	std::string file_proto = data_path + "log";
	std::fstream output(file_proto.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
	if (!msg1.SerializeToOstream(&output)) {
		std::cerr << "Failed to write msg." << std::endl;
		return -1;
	}
	output.close();

	// 4-->Reader,读取结构化数据,log文件
	lm::helloworld msg2;

	std::fstream input(file_proto.c_str(), std::ios::in | std::ios::binary);
	if (!msg2.ParseFromIstream(&input)) {
		std::cerr << "Failed to parse address book." << std::endl;
		return -1;
	}
	input.close();

	ListMsg(msg2);

	return 0;
}

int test_ProtoBuf2()
{
	// 序列化，将数据存入文件
	caffe_tmp::BlobShape blobShape;
	blobShape.add_dim(4);
	blobShape.add_dim(8);
	int size_blobShape = blobShape.ByteSize();

	caffe_tmp::BlobProto blobProto;
	blobProto.add_data(1.5);
	blobProto.add_diff(3.0);
	blobProto.set_channels(3);
	blobProto.set_height(100);
	blobProto.set_width(200);
	blobProto.set_num(5);
	blobProto.set_data(0, -1.0);
	blobProto.add_data(3.3);
	caffe_tmp::BlobShape* blobShape1 = blobProto.mutable_shape();
	blobShape1->add_dim(10);
	int size_blobProto = blobProto.ByteSize();

	caffe_tmp::Datum datum;
	byte tmp1[5] = { 1, 2, 3, 4, 5 };
	datum.set_data(tmp1, 5);
	int size_datum = datum.ByteSize();

	caffe_tmp::NetState netState;
	std::string str[3] = { "hello", "protobuf", "caffe" };
	netState.add_stage(str[0]);
	netState.add_stage(str[1]);
	netState.add_stage(str[2]);
	caffe_tmp::Phase phase = caffe_tmp::Phase::TRAIN;
	netState.set_phase(phase);
	int size_netState = netState.ByteSize();

	caffe_tmp::LossParameter lossParameter;
	lossParameter.set_normalize(false);
	int size_lossParameter = lossParameter.ByteSize();

	caffe_tmp::ConvolutionParameter convolutionParameter;
	convolutionParameter.set_num_output(2);
	convolutionParameter.set_engine(caffe_tmp::ConvolutionParameter::Engine::ConvolutionParameter_Engine_CAFFE);
	int size_convolutionParameter = convolutionParameter.ByteSize();

	caffe_tmp::PoolingParameter poolingParameter;
	poolingParameter.set_pool(caffe_tmp::PoolingParameter::PoolMethod::PoolingParameter_PoolMethod_AVE);
	poolingParameter.set_engine(caffe_tmp::PoolingParameter::Engine::PoolingParameter_Engine_CUDNN);
	int size_poolingParameter = poolingParameter.ByteSize();

	std::fstream output;
	std::string file_out = data_path + "caffe.bin";
	output.open(file_out.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
	if (!output.is_open()) {
		std::cout << "write, open file fail" << std::endl;
		return -1;
	}

	if (!blobShape.SerializeToOstream(&output) || !blobProto.SerializeToOstream(&output) || !datum.SerializeToOstream(&output) ||
		!netState.SerializeToOstream(&output) || !lossParameter.SerializeToOstream(&output) ||
		!convolutionParameter.SerializeToOstream(&output) || !poolingParameter.SerializeToOstream(&output)) {
		std::cout << "failed to write" << std::endl;
		return -1;
	}
	output.close();

	// 解析(反序列化)
	std::fstream input;
	std::string file_in = data_path + "caffe.bin";
	input.open(file_in.c_str(), std::ios::in | std::ios::binary);
	if (!input.is_open()) {
		std::cout << "read, open file fail" << std::endl;
		return -1;
	}

	char* buf = new char[1024];
	input.read((char*)buf, size_blobShape);
	caffe_tmp::BlobShape blobShape_;
	blobShape_.ParseFromString((char*)buf);
	assert(blobShape_.ByteSize() == size_blobShape);
	assert(blobShape_.dim_size() == 2);
	assert(blobShape_.dim(0) == 4);
	assert(blobShape_.dim(1) == 8);

	input.read(buf, size_blobProto);
	caffe_tmp::BlobProto blobProto_;
	blobProto_.ParseFromArray(buf, size_blobProto);
	assert(blobProto_.ByteSize() == size_blobProto);
	assert(blobProto_.has_shape() == true);
	blobShape_ = blobProto_.shape();
	assert(blobShape_.dim(0) == 10);
	assert(blobProto_.data_size() == 2);
	assert(blobProto_.data(0) == -1.0);
	assert(blobProto_.diff(0) == 3.0);
	assert(blobProto_.has_num() == true);

	input.read(buf, size_datum);
	caffe_tmp::Datum datum_;
	datum_.ParseFromArray(buf, size_datum);
	assert(datum_.ByteSize() == size_datum);
	assert(datum_.has_channels() == false);
	assert(datum_.float_data_size() == 0);
	assert(datum_.has_data() == true);
	std::string str1 = datum_.data();
	assert(str1.size() == 5);
	std::vector<byte> bytes(str1.begin(), str1.end());
	assert(bytes[4] == 5);

	input.read(buf, size_netState);
	caffe_tmp::NetState netState_;
	netState_.ParseFromArray(buf, size_netState);
	assert(netState_.ByteSize() == size_netState);
	assert(netState_.has_phase() == true);
	caffe_tmp::Phase phase_ = netState_.phase();
	assert(phase_ == 0);
	assert(netState_.has_level() == false);
	assert(netState_.stage_size() == 3);
	assert(netState_.stage(1) == "protobuf");

	input.read(buf, size_lossParameter);
	caffe_tmp::LossParameter lossParameter_;
	lossParameter_.ParseFromArray(buf, size_lossParameter);
	assert(lossParameter_.ByteSize() == size_lossParameter);
	assert(lossParameter_.has_ignore_label() == false);
	assert(lossParameter_.normalize() == false);

	input.read(buf, size_convolutionParameter);
	caffe_tmp::ConvolutionParameter convolutionParameter_;
	convolutionParameter_.ParseFromArray(buf, size_convolutionParameter);
	assert(convolutionParameter_.ByteSize() == size_convolutionParameter);
	assert(convolutionParameter_.has_kernel_size() == false);
	assert(convolutionParameter_.num_output() == 2);
	assert(convolutionParameter_.has_engine() == true);
	caffe_tmp::ConvolutionParameter_Engine engine_ = convolutionParameter_.engine();
	assert(engine_ == caffe_tmp::ConvolutionParameter::Engine::ConvolutionParameter_Engine_CAFFE);

	input.read(buf, size_poolingParameter);
	caffe_tmp::PoolingParameter poolingParameter_;
	poolingParameter_.ParseFromArray(buf, size_poolingParameter);
	assert(poolingParameter_.ByteSize() == size_poolingParameter);
	assert(poolingParameter_.has_pool() == true);
	caffe_tmp::PoolingParameter_PoolMethod poolMethod = poolingParameter_.pool();
	assert(poolMethod == caffe_tmp::PoolingParameter::PoolMethod::PoolingParameter_PoolMethod_AVE);
	caffe_tmp::PoolingParameter_Engine pooling_engine_ = poolingParameter_.engine();
	assert(pooling_engine_ == 2);

	delete[] buf;
	input.close();

	return 0;
}

