#include "funset.hpp"
#include "common.hpp"

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
	// reference: ./examples/cifar10/convert_CIFAR10_data.cpp
	const std::vector<std::string> argv{
		"*.exe",
		"E:/GitCode/Caffe_Test/test_data/cifar10",
		"E:/GitCode/Caffe_Test/test_data/cifar10",
		"lmdb" };

	convert_dataset(argv[1], argv[2], argv[3]);

	fprintf(stderr, "cifar10 convert finish\n");
	return 0;
}
