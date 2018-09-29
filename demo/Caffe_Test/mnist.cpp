#include "funset.hpp"
#include <stdio.h>
#include <memory>
#include <fstream>
#include <tuple>
#include "common.hpp"

int mnist_tensorrt_predict()
{
	// Blog: http://blog.csdn.net/fengbingchun/article/details/78606228
#ifdef CPU_ONLY
	caffe::Caffe::set_mode(caffe::Caffe::CPU);
#else
	caffe::Caffe::set_mode(caffe::Caffe::GPU);
#endif

#ifdef _MSC_VER
	const std::string deploy_file{ "E:/GitCode/Caffe_Test/test_data/model/mnist/mnist_tensorrt.prototxt" };
	const std::string model_filename{ "E:/GitCode/Caffe_Test/test_data/model/mnist/mnist_tensorrt.caffemodel" };
	const std::string mean_file{ "E:/GitCode/Caffe_Test/test_data/model/mnist/mnist_tensorrt_mean.binary" };
	const std::string image_path{ "E:/GitCode/Caffe_Test/test_data/images/handwritten_digits/" };
#else
	const std::string deploy_file{ "test_data/model/mnist/mnist_tensorrt.prototxt" };
	const std::string model_filename{ "test_data/model/mnist/mnist_tensorrt.caffemodel" };
	const std::string mean_file{ "test_data/model/mnist/mnist_tensorrt_mean.binary" };
	const std::string image_path{ "test_data/images/handwritten_digits/" };
#endif

	caffe::Net<float> caffe_net(deploy_file, caffe::TEST);
	caffe_net.CopyTrainedLayersFrom(model_filename);

	// print net info
	fprintf(stdout, "input blob num: %d, output blob num: %d\n", caffe_net.num_inputs(), caffe_net.num_outputs());
	const boost::shared_ptr<caffe::Blob<float> > blob_by_name = caffe_net.blob_by_name("data");
	int image_num = blob_by_name->num();
	int image_channel = blob_by_name->channels();
	int image_height = blob_by_name->height();
	int image_width = blob_by_name->width();
	fprintf(stdout, "inpub blob shape(num, channels, height, width): %d, %d, %d, %d\n",
		image_num, image_channel, image_height, image_width);

	fprintf(stdout, "layer names: ");
	for (int i = 0; i < caffe_net.layer_names().size(); ++i) {
		fprintf(stdout, "  %s  ", caffe_net.layer_names()[i].c_str());
	}
	fprintf(stdout, "\nblob names: ");
	for (int i = 0; i < caffe_net.blob_names().size(); ++i) {
		fprintf(stdout, "  %s  ", caffe_net.blob_names()[i].c_str());
	}
	fprintf(stdout, "\nlayer types: ");
	for (int i = 0; i < caffe_net.layers().size(); ++i) {
		fprintf(stdout, "  %s  ", caffe_net.layers()[i]->type());
	}
	const std::vector<caffe::Blob<float>*> output_blobs = caffe_net.output_blobs();
	fprintf(stdout, "\noutput blobs num: %d, blob(num, channel, heihgt, width): %d, %d, %d, %d\n",
		output_blobs.size(), output_blobs[0]->num(), output_blobs[0]->channels(), output_blobs[0]->height(), output_blobs[0]->width());

	const int image_size{ image_num * image_channel * image_height * image_width };
	std::unique_ptr<float[]> mean_values(new float[image_size]);
	std::ifstream in(mean_file.c_str(), std::ios::in | std::ios::binary);
	if (!in.is_open()) {
		fprintf(stderr, "read mean file fail: %s\n", mean_file.c_str());
		return -1;
	}
	in.read((char*)mean_values.get(), image_size * sizeof(float));
	in.close();

	const std::vector<int> target{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	typedef std::tuple<int, float> result;
	std::vector<result> results;

	for (const auto& num : target) {
		std::string str = std::to_string(num);
		str += ".png";
		str = image_path + str;

		cv::Mat mat = cv::imread(str.c_str(), 0);
		if (!mat.data) {
			fprintf(stderr, "load image error: %s\n", str.c_str());
			return -1;
		}

		cv::resize(mat, mat, cv::Size(image_width, image_height));
		mat.convertTo(mat, CV_32FC1);

		float* p = (float*)mat.data;
		for (int i = 0; i < image_size; ++i) {
			p[i] -= mean_values.get()[i];
		}

		const std::vector<caffe::Blob<float>*>& blob_input = caffe_net.input_blobs();
		blob_input[0]->set_cpu_data((float*)mat.data);

		const std::vector<caffe::Blob<float>*>& output_blob_ = caffe_net.Forward(nullptr);
		const float* output = output_blob_[0]->cpu_data();

		float tmp{ -1.f };
		int pos{ -1 };

		for (int j = 0; j < output_blobs[0]->count(); j++) {
			if (tmp < output[j]) {
				pos = j;
				tmp = output[j];
			}
		}

		result ret = std::make_tuple(pos, tmp);
		results.push_back(ret);
	}

	for (auto i = 0; i < target.size(); i++)
		fprintf(stdout, "actual digit is: %d, result digit is: %d, probability: %f\n",
			target[i], std::get<0>(results[i]), std::get<1>(results[i]));

	fprintf(stdout, "predict finish\n");

	return 0;
}

int mnist_train()
{
	// Blog: http://blog.csdn.net/fengbingchun/article/details/49849225
	//       http://blog.csdn.net/fengbingchun/article/details/68065338
	
#ifdef CPU_ONLY
	caffe::Caffe::set_mode(caffe::Caffe::CPU);
#else
	caffe::Caffe::set_mode(caffe::Caffe::GPU);
#endif

#ifdef _MSC_VER
	const std::string filename{ "E:/GitCode/Caffe_Test/test_data/model/mnist/lenet_solver.prototxt" };
#else
	const std::string filename{ "test_data/model/mnist/lenet_solver_linux.prototxt" };
#endif
	caffe::SolverParameter solver_param;
	if (!caffe::ReadProtoFromTextFile(filename.c_str(), &solver_param)) {
		fprintf(stderr, "parse solver.prototxt fail\n");
		return -1;
	}

	mnist_convert(); // convert MNIST to LMDB

	caffe::SGDSolver<float> solver(solver_param);
	solver.Solve();

	fprintf(stderr, "train finish\n");
	return 0;
}

int mnist_predict()
{
	// Blog: http://blog.csdn.net/fengbingchun/article/details/50987185
	//       http://blog.csdn.net/fengbingchun/article/details/69001433
#ifdef CPU_ONLY
	caffe::Caffe::set_mode(caffe::Caffe::CPU);
#else
	caffe::Caffe::set_mode(caffe::Caffe::GPU);
#endif

#ifdef _MSC_VER
	const std::string param_file{ "E:/GitCode/Caffe_Test/test_data/model/mnist/lenet_train_test_.prototxt" };
	const std::string trained_filename{ "E:/GitCode/Caffe_Test/test_data/model/mnist/lenet_iter_10000.caffemodel" };
	const std::string image_path{ "E:/GitCode/Caffe_Test/test_data/images/handwritten_digits/" };
#else
	const std::string param_file{ "test_data/model/mnist/lenet_train_test_.prototxt" };
	const std::string trained_filename{ "test_data/model/mnist/lenet_iter_10000.caffemodel" };
	const std::string image_path{ "test_data/images/handwritten_digits/" };
#endif

	// 有两种方法可以实例化net
	// 1. 通过传入参数类型为std::string
	caffe::Net<float> caffe_net(param_file, caffe::TEST);
	caffe_net.CopyTrainedLayersFrom(trained_filename);
	// 2. 通过传入参数类型为caffe::NetParameter
	//caffe::NetParameter net_param1, net_param2;
	//caffe::ReadNetParamsFromTextFileOrDie(param_file, &net_param1);
	//net_param1.mutable_state()->set_phase(caffe::TEST);
	//caffe::Net<float> caffe_net(net_param1);
	//caffe::ReadNetParamsFromBinaryFileOrDie(trained_filename, &net_param2);
	//caffe_net.CopyTrainedLayersFrom(net_param2);

	int num_inputs = caffe_net.input_blobs().size(); // 0 ??
	const boost::shared_ptr<caffe::Blob<float> > blob_by_name = caffe_net.blob_by_name("data");
	int image_channel = blob_by_name->channels();
	int image_height = blob_by_name->height();
	int image_width = blob_by_name->width();

	int num_outputs = caffe_net.num_outputs();
	const std::vector<caffe::Blob<float>*> output_blobs = caffe_net.output_blobs();
	int require_blob_index{ -1 };
	const int digit_category_num{ 10 };
	for (int i = 0; i < output_blobs.size(); ++i) {
		if (output_blobs[i]->count() == digit_category_num)
			require_blob_index = i;
	}
	if (require_blob_index == -1) {
		fprintf(stderr, "ouput blob don't match\n");
		return -1;
	}

	std::vector<int> target{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	std::vector<int> result;

	for (auto num : target) {
		std::string str = std::to_string(num);
		str += ".png";
		str = image_path + str;

		cv::Mat mat = cv::imread(str.c_str(), 1);
		if (!mat.data) {
			fprintf(stderr, "load image error: %s\n", str.c_str());
			return -1;
		}

		if (image_channel == 1)
			cv::cvtColor(mat, mat, CV_BGR2GRAY);
		else if (image_channel == 4)
			cv::cvtColor(mat, mat, CV_BGR2BGRA);

		cv::resize(mat, mat, cv::Size(image_width, image_height));
		cv::bitwise_not(mat, mat);

		// 将图像数据载入Net网络，有2种方法
		boost::shared_ptr<caffe::MemoryDataLayer<float> > memory_data_layer =
			boost::static_pointer_cast<caffe::MemoryDataLayer<float>>(caffe_net.layer_by_name("data"));
		// 1. 通过MemoryDataLayer类的Reset函数
		mat.convertTo(mat, CV_32FC1, 0.00390625);
		float dummy_label[1] {0};
		memory_data_layer->Reset((float*)(mat.data), dummy_label, 1);

		// 2. 通过MemoryDataLayer类的AddMatVector函数
		//std::vector<cv::Mat> patches{mat}; // set the patch for testing
		//std::vector<int> labels(patches.size());
		//memory_data_layer->AddMatVector(patches, labels); // push vector<Mat> to data layer

		float loss{ 0.0 };
		const std::vector<caffe::Blob<float>*>& results = caffe_net.ForwardPrefilled(&loss); // Net forward
		const float* output = results[require_blob_index]->cpu_data();

		float tmp{ -1 };
		int pos{ -1 };

		fprintf(stderr, "actual digit is: %d\n", target[num]);
		for (int j = 0; j < 10; j++) {
			printf("Probability to be Number %d is: %.3f\n", j, output[j]);
			if (tmp < output[j]) {
				pos = j;
				tmp = output[j];
			}
		}

		result.push_back(pos);
	}

	for (auto i = 0; i < 10; i++)
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
		boost::filesystem::create_directory(db_path);

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
#ifdef __linux__
		int ret = snprintf(key_cstr, kMaxKeyLength, "%08d", item_id);
#else
		int ret = _snprintf(key_cstr, kMaxKeyLength, "%08d", item_id);
#endif
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
	// Blog: http://blog.csdn.net/fengbingchun/article/details/49794453
	//mnist test images	
	//FLAGS_minloglevel = 2; // Fix: WARNING: Logging before InitGoogleLogging() is written to STDERR
#ifdef _MSC_VER
	const std::string argv_test[] {"E:/GitCode/Caffe_Test/test_data/MNIST/t10k-images.idx3-ubyte",
		"E:/GitCode/Caffe_Test/test_data/MNIST/t10k-labels.idx1-ubyte",
		"E:\\GitCode\\Caffe_Test\\test_data\\MNIST\\test"};
#else
	const std::string argv_test[] {"test_data/MNIST/t10k-images.idx3-ubyte",
		"test_data/MNIST/t10k-labels.idx1-ubyte",
		"test_data/MNIST/test"};
#endif
	//mnist train images
#ifdef _MSC_VER
	const std::string argv_train[] { "E:/GitCode/Caffe_Test/test_data/MNIST/train-images.idx3-ubyte",
		"E:/GitCode/Caffe_Test/test_data/MNIST/train-labels.idx1-ubyte",
		"E:\\GitCode\\Caffe_Test\\test_data\\MNIST\\train" };
#else
	const std::string argv_train[] { "test_data/MNIST/train-images.idx3-ubyte",
		"test_data/MNIST/train-labels.idx1-ubyte",
		"test_data/MNIST/train" };
#endif

	convert_dataset(argv_train[0].c_str(), argv_train[1].c_str(), argv_train[2].c_str(), "lmdb");
	convert_dataset(argv_test[0].c_str(), argv_test[1].c_str(), argv_test[2].c_str(), "lmdb");

	fprintf(stderr, "mnist convert finish\n");
	return 0;
}
