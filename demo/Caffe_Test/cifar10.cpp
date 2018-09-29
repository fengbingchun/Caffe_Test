#include "funset.hpp"
#include "common.hpp"

int cifar10_train()
{
	// Blog: http://blog.csdn.net/fengbingchun/article/details/72953284
#ifdef CPU_ONLY
	caffe::Caffe::set_mode(caffe::Caffe::CPU);
#else
	caffe::Caffe::set_mode(caffe::Caffe::GPU);
#endif

#ifdef _MSC_VER
	const std::string filename{ "E:/GitCode/Caffe_Test/test_data/model/cifar10/cifar10_quick_solver.prototxt" };
#else
	const std::string filename{ "test_data/model/cifar10/cifar10_quick_solver_linux.prototxt" };
#endif
	caffe::SolverParameter solver_param;
	if (!caffe::ReadProtoFromTextFile(filename.c_str(), &solver_param)) {
		fprintf(stderr, "parse solver.prototxt fail\n");
		return -1;
	}

	cifar10_convert(); // convert cifar10 to LMDB
	if (cifar10_compute_image_mean() != 0) { // compute cifar10 image mean, generate mean.binaryproto
		fprintf(stderr, "compute cifar10 image mean fail\n");
		return -1;
	}

	caffe::SGDSolver<float> solver(solver_param);
	solver.Solve();

	fprintf(stderr, "cifar10 train finish\n");
	return 0;
}

int cifar10_predict()
{
	// Blog: http://blog.csdn.net/fengbingchun/article/details/72999346
#ifdef CPU_ONLY
	caffe::Caffe::set_mode(caffe::Caffe::CPU);
#else
	caffe::Caffe::set_mode(caffe::Caffe::GPU);
#endif

#ifdef _MSC_VER
	const std::string param_file{ "E:/GitCode/Caffe_Test/test_data/model/cifar10/cifar10_quick_train_test_.prototxt" };
	const std::string trained_filename{ "E:/GitCode/Caffe_Test/test_data/model/cifar10/cifar10_quick_iter_4000.caffemodel.h5" };
	const std::string image_path{ "E:/GitCode/Caffe_Test/test_data/images/object_recognition/" };
	const std::string mean_file{"E:/GitCode/Caffe_Test/test_data/model/cifar10/mean.binaryproto"};
#else
	const std::string param_file{ "test_data/model/cifar10/cifar10_quick_train_test_.prototxt" };
	const std::string trained_filename{ "test_data/model/cifar10/cifar10_quick_iter_4000.caffemodel.h5" };
	const std::string image_path{ "test_data/images/object_recognition/" };
	const std::string mean_file{"test_data/model/cifar10/mean.binaryproto"};
#endif

	caffe::Net<float> caffe_net(param_file, caffe::TEST);
	caffe_net.CopyTrainedLayersFromHDF5(trained_filename);

	const boost::shared_ptr<caffe::Blob<float> > blob_by_name = caffe_net.blob_by_name("data");
	int image_channel = blob_by_name->channels();
	int image_height = blob_by_name->height();
	int image_width = blob_by_name->width();

	int num_outputs = caffe_net.num_outputs();
	const std::vector<caffe::Blob<float>*>& output_blobs = caffe_net.output_blobs();
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

	// read mean data
	caffe::BlobProto image_mean; // storage order: rr..rrgg..ggbb..bb
	if (!caffe::ReadProtoFromBinaryFile(mean_file, &image_mean)) {
		fprintf(stderr, "parse mean file fail\n");
		return -1;
	}

	if (image_channel != image_mean.channels() || image_height != image_mean.height() || image_width != image_mean.width() ||
		image_channel != 3) {
		fprintf(stderr, "their dimension dismatch\n");
		return -1;
	}

	cv::Mat mat_mean(image_height, image_width, CV_32FC3, const_cast<float*>(image_mean.data().data()));

	for (auto num : target) {
		std::string str = std::to_string(num);
		str += ".jpg";
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
		mat.convertTo(mat, CV_32FC3);

		// Note: need to subtract mean
		std::vector<cv::Mat> mat_tmp2; //b,g,r
		cv::split(mat, mat_tmp2);
		cv::Mat mat_tmp3(image_height, image_width, CV_32FC3);
		float* p = (float*)mat_tmp3.data;
		memcpy(p, mat_tmp2[2].data, image_height * image_width * sizeof(float));
		memcpy(p + image_height * image_width, mat_tmp2[1].data, image_height * image_width * sizeof(float));
		memcpy(p + image_height * image_width * 2, mat_tmp2[0].data, image_height * image_width * sizeof(float));
		cv::subtract(mat_tmp3, mat_mean, mat_tmp3);

		boost::shared_ptr<caffe::MemoryDataLayer<float> > memory_data_layer =
			boost::static_pointer_cast<caffe::MemoryDataLayer<float>>(caffe_net.layer_by_name("data"));
		float dummy_label[1] {0};
		memory_data_layer->Reset((float*)(mat_tmp3.data), dummy_label, 1); // rr..rrgg..ggbb..bb

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

const int CIFAR10_IMAGE_BTYES_SIZE = 3072; // 32 * 32 * 3
const int CIFAR10_IMAGE_WIDTH = 32;
const int CIFAR10_IMAGE_HEIGHT = 32;
const int CIFAR10_TRAIN_BATCHES = 5;
const int CIFAR10_BATCH_SIZE = 10000;

static void read_image(std::ifstream* file, int* label, char* buffer)
{
	char label_char;
	file->read(&label_char, 1);
	*label = label_char;
	file->read(buffer, CIFAR10_IMAGE_BTYES_SIZE);
	return;
}

static void convert_dataset(const std::string& input_folder, const std::string& output_folder, const std::string& db_type)
{
	// prevent crash when multiple call convert_dataset function
	std::vector<std::string> output_folder_name{ output_folder + "/cifar10_train_" + db_type,
		output_folder + "/cifar10_test_" + db_type };
	for (const auto& name : output_folder_name) {
		//const
		boost::filesystem::remove_all(name);
	}

	boost::scoped_ptr<caffe::db::DB> train_db(caffe::db::GetDB(db_type));
	train_db->Open(output_folder + "/cifar10_train_" + db_type, caffe::db::NEW);
	boost::scoped_ptr<caffe::db::Transaction> txn(train_db->NewTransaction());
	// Data buffer
	int label;
	char str_buffer[CIFAR10_IMAGE_BTYES_SIZE];
	caffe::Datum datum;
	datum.set_channels(3);
	datum.set_height(CIFAR10_IMAGE_HEIGHT);
	datum.set_width(CIFAR10_IMAGE_WIDTH);

	LOG(INFO) << "Writing Training data";
	for (int fileid = 0; fileid < CIFAR10_TRAIN_BATCHES; ++fileid) {
		// Open files
		LOG(INFO) << "Training Batch " << fileid + 1;
		std::string batchFileName = input_folder + "/data_batch_" + caffe::format_int(fileid + 1) + ".bin";
		std::ifstream data_file(batchFileName.c_str(), std::ios::in | std::ios::binary);
		CHECK(data_file) << "Unable to open train file #" << fileid + 1;
		for (int itemid = 0; itemid < CIFAR10_BATCH_SIZE; ++itemid) {
			read_image(&data_file, &label, str_buffer);
			datum.set_label(label);
			datum.set_data(str_buffer, CIFAR10_IMAGE_BTYES_SIZE);
			std::string out;
			CHECK(datum.SerializeToString(&out));
			txn->Put(caffe::format_int(fileid * CIFAR10_BATCH_SIZE + itemid, 5), out);
		}
	}
	txn->Commit();
	train_db->Close();

	LOG(INFO) << "Writing Testing data";
	boost::scoped_ptr<caffe::db::DB> test_db(caffe::db::GetDB(db_type));
	test_db->Open(output_folder + "/cifar10_test_" + db_type, caffe::db::NEW);
	txn.reset(test_db->NewTransaction());
	// Open files
	std::ifstream data_file((input_folder + "/test_batch.bin").c_str(), std::ios::in | std::ios::binary);
	CHECK(data_file) << "Unable to open test file.";
	for (int itemid = 0; itemid < CIFAR10_BATCH_SIZE; ++itemid) {
		read_image(&data_file, &label, str_buffer);
		datum.set_label(label);
		datum.set_data(str_buffer, CIFAR10_IMAGE_BTYES_SIZE);
		std::string out;
		CHECK(datum.SerializeToString(&out));
		txn->Put(caffe::format_int(itemid, 5), out);
	}
	txn->Commit();
	test_db->Close();
}

int cifar10_convert()
{
	// Blog: http://blog.csdn.net/fengbingchun/article/details/71540852
	// reference: ./examples/cifar10/convert_CIFAR10_data.cpp
	//FLAGS_minloglevel = 2; // Fix: WARNING: Logging before InitGoogleLogging() is written to STDERR
	const std::vector<std::string> argv{
		"*.exe",
#ifdef _MSC_VER
		"E:/GitCode/Caffe_Test/test_data/cifar10",
		"E:/GitCode/Caffe_Test/test_data/cifar10",
#else
		"test_data/cifar10",
		"test_data/cifar10",
#endif
		"lmdb" };

	convert_dataset(argv[1], argv[2], argv[3]);

	fprintf(stderr, "cifar10 convert finish\n");
	return 0;
}
