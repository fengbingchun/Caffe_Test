#include "funset.hpp"
#include "common.hpp"

int mnist_train()
{
	caffe::Caffe::set_mode(caffe::Caffe::CPU);
	const std::string filename{ "E:/GitCode/Caffe_Test/test_data/model/mnist/lenet_solver.prototxt" };

	caffe::SolverParameter solver_param;
	if (!caffe::ReadProtoFromTextFile(filename.c_str(), &solver_param)) {
		fprintf(stderr, "parse solver.prototxt fail\n");
		return -1;
	}

	boost::shared_ptr<caffe::Solver<float> > solver(caffe::GetSolver<float>(solver_param));

	fprintf(stderr, "Starting Optimization\n");
	solver->Solve();
	fprintf(stderr, "Optimization Done\n");

	fprintf(stderr, "train finish\n");
	return 0;
}

int mnist_predict()
{
	caffe::Caffe::set_mode(caffe::Caffe::CPU);

	const std::string param_file{ "E:/GitCode/Caffe_Test/test_data/model/mnist/lenet_train_test_.prototxt" };
	const std::string trained_filename{ "E:/GitCode/Caffe_Test/test_data/model/mnist/lenet_iter_10000.caffemodel" };
	const std::string image_path{ "E:/GitCode/Caffe_Test/test_data/images/" };

	// Instantiate the caffe net.
	caffe::Net<float> caffe_net(param_file, caffe::TEST);
	caffe_net.CopyTrainedLayersFrom(trained_filename);

	std::vector<int> target{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	std::vector<int> result;

	for (int i = 0; i < target.size(); i++) {
		std::string str = std::to_string(i);
		str += ".png";
		str = image_path + str;

		cv::Mat mat = cv::imread(str.c_str(), 1);
		if (!mat.data) {
			fprintf(stderr, "load image error: %s\n", str.c_str());
			return -1;
		}

		cv::cvtColor(mat, mat, CV_BGR2GRAY);
		cv::resize(mat, mat, cv::Size(28, 28));
		cv::bitwise_not(mat, mat);

		// set the patch for testing
		std::vector<cv::Mat> patches;
		patches.push_back(mat);

		// push vector<Mat> to data layer
		float loss = 0.0;
		boost::shared_ptr<caffe::MemoryDataLayer<float> > memory_data_layer;
		memory_data_layer = boost::static_pointer_cast<caffe::MemoryDataLayer<float>>(caffe_net.layer_by_name("data"));

		std::vector<int> labels(patches.size());
		memory_data_layer->AddMatVector(patches, labels);

		// Net forward
		const std::vector<caffe::Blob<float>*>& results = caffe_net.ForwardPrefilled(&loss);
		float* output = results[1]->mutable_cpu_data();

		float tmp = -1;
		int pos = -1;

		// Display the output
		fprintf(stderr, "actual digit is: %d\n", target[i]);
		for (int j = 0; j < 10; j++) {
			printf("Probability to be Number %d is: %.3f\n", j, output[j]);
			if (tmp < output[j]) {
				pos = j;
				tmp = output[j];
			}
		}

		result.push_back(pos);
	}

	for (int i = 0; i < 10; i++)
		fprintf(stderr, "actual digit is: %d, result digit is: %d\n", target[i], result[i]);

	fprintf(stderr, "predict finish\n");
	return 0;
}

static uint32_t swap_endian(uint32_t val) {
	val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
	return (val << 16) | (val >> 16);
}

static void convert_dataset(const char* image_filename, const char* label_filename,
	const char* db_path, const std::string& db_backend) {
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
	std::string value;

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
		std::string keystr(key_cstr);

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

// mnist convert to lmdb or leveldb
int mnist_convert()
{
	//mnist test images
	const std::string argv_test[] {"E:/GitCode/Caffe_Test/test_data/MNIST/t10k-images.idx3-ubyte",
		"E:/GitCode/Caffe_Test/test_data/MNIST/t10k-labels.idx1-ubyte",
		"E:\\GitCode\\Caffe_Test\\test_data\\MNIST\\test"};
	//mnist train images
	const std::string argv_train[] { "E:/GitCode/Caffe_Test/test_data/MNIST/train-images.idx3-ubyte",
		"E:/GitCode/Caffe_Test/test_data/MNIST/train-labels.idx1-ubyte",
		"E:\\GitCode\\Caffe_Test\\test_data\\MNIST\\train" };

	//convert_dataset(argv_train[0].c_str(), argv_train[1].c_str(), argv_train[2].c_str(), "lmdb");
	convert_dataset(argv_test[0].c_str(), argv_test[1].c_str(), argv_test[2].c_str(), "lmdb");

	fprintf(stderr, "mnist convert finish\n");
	return 0;
}
