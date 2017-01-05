#include "funset.hpp"
#include "common.hpp"

// gflags中的数据类型，c++ string，在这里指定转换到lmdb还是leveldb
DEFINE_string(backend, "lmdb", "The backend for storing the result");

static uint32_t swap_endian(uint32_t val) {
	val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
	return (val << 16) | (val >> 16);
}

static void convert_dataset(const char* image_filename, const char* label_filename,
	const char* db_path, const string& db_backend) {
	// Open files
	std::ifstream image_file(image_filename, std::ios::in | std::ios::binary);
	std::ifstream label_file(label_filename, std::ios::in | std::ios::binary);
	CHECK(image_file) << "Unable to open file " << image_filename;
	CHECK(label_file) << "Unable to open file " << label_filename;
	// Read the magic and the meta data
	uint32_t magic;
	uint32_t num_items;
	uint32_t num_labels;
	uint32_t rows;
	uint32_t cols;

	// 读取文件前n个字节，获取图像数量、图像宽、图像高
	image_file.read(reinterpret_cast<char*>(&magic), 4);
	magic = swap_endian(magic);
	CHECK_EQ(magic, 2051) << "Incorrect image file magic.";
	label_file.read(reinterpret_cast<char*>(&magic), 4);
	magic = swap_endian(magic);
	CHECK_EQ(magic, 2049) << "Incorrect label file magic.";
	image_file.read(reinterpret_cast<char*>(&num_items), 4);
	num_items = swap_endian(num_items);
	label_file.read(reinterpret_cast<char*>(&num_labels), 4);
	num_labels = swap_endian(num_labels);
	CHECK_EQ(num_items, num_labels);
	image_file.read(reinterpret_cast<char*>(&rows), 4);
	rows = swap_endian(rows);
	image_file.read(reinterpret_cast<char*>(&cols), 4);
	cols = swap_endian(cols);

	// lmdb
	MDB_env *mdb_env = NULL;
	MDB_dbi mdb_dbi;
	MDB_val mdb_key, mdb_data;
	MDB_txn *mdb_txn = NULL;
	// leveldb
	leveldb::DB* db = NULL;
	leveldb::Options options;
	options.error_if_exists = true;
	options.create_if_missing = true;
	options.write_buffer_size = 268435456;
	leveldb::WriteBatch* batch = NULL;

	// Open db
	if (db_backend == "leveldb") {  // leveldb
		LOG(INFO) << "Opening leveldb " << db_path;
		leveldb::Status status = leveldb::DB::Open(
			options, db_path, &db);
		CHECK(status.ok()) << "Failed to open leveldb " << db_path
			<< ". Is it already existing?";
		batch = new leveldb::WriteBatch();
	}
	else if (db_backend == "lmdb") {  // lmdb
		int rc;
		LOG(INFO) << "Opening lmdb " << db_path;
		// 创建指定的存放目录
		//CHECK_EQ(mkdir(db_path, 0744), 0)
		std::string strPath = std::string(db_path);
		std::string delPath = "rmdir /s/q " + strPath;
		system(delPath.c_str());
		strPath = "mkdir " + strPath;
		system(strPath.c_str());
		//CHECK_EQ(system(strPath.c_str()), 0) << "mkdir " << db_path << "failed";

		// 创建lmdb数据库
		CHECK_EQ(mdb_env_create(&mdb_env), MDB_SUCCESS) << "mdb_env_create failed";
		//CHECK_EQ(mdb_env_set_mapsize(mdb_env, 1099511627776), MDB_SUCCESS) << "mdb_env_set_mapsize failed";//1TB
		CHECK_EQ(mdb_env_set_mapsize(mdb_env, 107374182), MDB_SUCCESS) << "mdb_env_set_mapsize failed";//100MB
		CHECK_EQ(mdb_env_open(mdb_env, db_path, 0, 0664), MDB_SUCCESS) << "mdb_env_open failed";
		CHECK_EQ(mdb_txn_begin(mdb_env, NULL, 0, &mdb_txn), MDB_SUCCESS) << "mdb_txn_begin failed";
		CHECK_EQ(mdb_open(mdb_txn, NULL, 0, &mdb_dbi), MDB_SUCCESS) << "mdb_open failed. Does the lmdb already exist? ";
	}
	else {
		LOG(FATAL) << "Unknown db backend " << db_backend;
	}

	// Storing to db
	char label;
	char* pixels = new char[rows * cols];
	int count = 0;
	const int kMaxKeyLength = 10;
	char key_cstr[kMaxKeyLength];
	string value;

	caffe::Datum datum; // Caffe数据类
	datum.set_channels(1);
	datum.set_height(rows);
	datum.set_width(cols);
	LOG(INFO) << "A total of " << num_items << " items.";
	LOG(INFO) << "Rows: " << rows << " Cols: " << cols;
	// 将数据写入lmdb或leveldb数据库
	for (int item_id = 0; item_id < num_items; ++item_id) {
		image_file.read(pixels, rows * cols);
		label_file.read(&label, 1);
		datum.set_data(pixels, rows*cols);
		datum.set_label(label);
		//snprintf(key_cstr, kMaxKeyLength, "%08d", item_id);
		int ret = _snprintf(key_cstr, kMaxKeyLength, "%08d", item_id);
		if (ret == kMaxKeyLength || ret < 0) {
			printf("warning ");
			key_cstr[kMaxKeyLength - 1] = 0;
		}
		datum.SerializeToString(&value);
		string keystr(key_cstr);

		// Put in db
		if (db_backend == "leveldb") {  // leveldb
			batch->Put(keystr, value);
		}
		else if (db_backend == "lmdb") {  // lmdb
			mdb_data.mv_size = value.size();
			mdb_data.mv_data = reinterpret_cast<void*>(&value[0]);
			mdb_key.mv_size = keystr.size();
			mdb_key.mv_data = reinterpret_cast<void*>(&keystr[0]);
			CHECK_EQ(mdb_put(mdb_txn, mdb_dbi, &mdb_key, &mdb_data, 0), MDB_SUCCESS) << "mdb_put failed";
		}
		else {
			LOG(FATAL) << "Unknown db backend " << db_backend;
		}

		if (++count % 1000 == 0) {
			// Commit txn
			if (db_backend == "leveldb") {  // leveldb
				db->Write(leveldb::WriteOptions(), batch);
				delete batch;
				batch = new leveldb::WriteBatch();
			}
			else if (db_backend == "lmdb") {  // lmdb
				CHECK_EQ(mdb_txn_commit(mdb_txn), MDB_SUCCESS) << "mdb_txn_commit failed";
				CHECK_EQ(mdb_txn_begin(mdb_env, NULL, 0, &mdb_txn), MDB_SUCCESS) << "mdb_txn_begin failed";
			}
			else {
				LOG(FATAL) << "Unknown db backend " << db_backend;
			}
		}
	}
	// write the last batch
	if (count % 1000 != 0) {
		if (db_backend == "leveldb") {  // leveldb
			db->Write(leveldb::WriteOptions(), batch);
			delete batch;
			delete db;
		}
		else if (db_backend == "lmdb") {  // lmdb
			CHECK_EQ(mdb_txn_commit(mdb_txn), MDB_SUCCESS) << "mdb_txn_commit failed";
			mdb_close(mdb_env, mdb_dbi);
			mdb_env_close(mdb_env);
		}
		else {
			LOG(FATAL) << "Unknown db backend " << db_backend;
		}
		LOG(ERROR) << "Processed " << count << " files.";
	}
	delete[] pixels;
}

int MNIST_convert()
{
#ifndef GFLAGS_GFLAGS_H_
	namespace gflags = google;
#endif
	int argc = 4;
	char* tmp[4] = { "", "", "", "" };
	char** argv = &tmp[0];

#ifdef _DEBUG
	argv[0] = "E:/GitCode/Caffe_Test/lib/dbg/x64_vc12/Caffe_Test.exe";
#else
	argv[0] = "E:/GitCode/Caffe_Test/lib/rel/x64_vc12/Caffe_Test.exe";
#endif
	//mnist test images
	//argv[1] = "E:/GitCode/Caffe_Test/test_data/MNIST/t10k-images.idx3-ubyte";
	//argv[2] = "E:/GitCode/Caffe_Test/test_data/MNIST/t10k-labels.idx1-ubyte";
	//argv[3] = "E:\\GitCode\\Caffe_Test\\test_data\\MNIST\\test";
	//mnist train images
	argv[1] = "E:/GitCode/Caffe_Test/test_data/MNIST/train-images.idx3-ubyte";
	argv[2] = "E:/GitCode/Caffe_Test/test_data/MNIST/train-labels.idx1-ubyte";
	argv[3] = "E:\\GitCode\\Caffe_Test\\test_data\\MNIST\\train";

	// 用来设定usage说明
	gflags::SetUsageMessage("This script converts the MNIST dataset to\n"
		"the lmdb/leveldb format used by Caffe to load data.\n"
		"Usage:\n"
		"    convert_mnist_data [FLAGS] input_image_file input_label_file "
		"output_db_file\n"
		"The MNIST dataset could be downloaded at\n"
		"    http://yann.lecun.com/exdb/mnist/\n"
		"You should gunzip them after downloading,"
		"or directly use data/mnist/get_mnist.sh\n");
	// 解析命令行参数
	gflags::ParseCommandLineFlags(&argc, &argv, true);

	// 获取标志参数backend的值
	const string& db_backend = FLAGS_backend;

	if (argc != 4) {
		// 输出usage说明
		gflags::ShowUsageWithFlagsRestrict(argv[0],
			"examples/mnist/convert_mnist_data");
	}
	else {
		// 设置日志文件名中"文件名"字段
		// 每个进程中至少要执行一次InitGoogleLogging，否则不产生日志文件
		google::InitGoogleLogging(argv[0]);
		convert_dataset(argv[1], argv[2], argv[3], db_backend);
	}

	std::cout << "ok!" << std::endl;

	return 0;
}
