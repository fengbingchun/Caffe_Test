#include "funset.hpp"
#include "common.hpp"

// Blog: https://blog.csdn.net/fengbingchun/article/details/88379067

int lenet_5_mnist_train()
{	
#ifdef CPU_ONLY
	caffe::Caffe::set_mode(caffe::Caffe::CPU);
#else
	caffe::Caffe::set_mode(caffe::Caffe::GPU);
#endif

#ifdef _MSC_VER
	const std::string filename{ "E:/GitCode/Caffe_Test/test_data/Net/lenet-5_mnist_windows_solver.prototxt" };
#else
	const std::string filename{ "test_data/Net/lenet-5_mnist_linux_solver.prototxt" };
#endif
	caffe::SolverParameter solver_param;
	if (!caffe::ReadProtoFromTextFile(filename.c_str(), &solver_param)) {
		fprintf(stderr, "parse solver.prototxt fail\n");
		return -1;
	}

	mnist_convert(); // convert MNIST to LMDB

	caffe::SGDSolver<float> solver(solver_param);
	solver.Solve();

	fprintf(stdout, "train finish\n");

	return 0;
}

int lenet_5_mnist_test()
{
#ifdef CPU_ONLY
	caffe::Caffe::set_mode(caffe::Caffe::CPU);
#else
	caffe::Caffe::set_mode(caffe::Caffe::GPU);
#endif

#ifdef _MSC_VER
	const std::string param_file{ "E:/GitCode/Caffe_Test/test_data/Net/lenet-5_mnist_windows_test.prototxt" };
	const std::string trained_filename{ "E:/GitCode/Caffe_Test/test_data/Net/lenet-5_mnist_iter_10000.caffemodel" };
	const std::string image_path{ "E:/GitCode/Caffe_Test/test_data/images/handwritten_digits/" };
#else
	const std::string param_file{ "test_data/Net/lenet-5_mnist_linux_test.prototxt" };
	const std::string trained_filename{ "test_data/Net/lenet-5_mnist_iter_10000.caffemodel" };
	const std::string image_path{ "test_data/images/handwritten_digits/" };
#endif

	caffe::Net<float> caffe_net(param_file, caffe::TEST);
	caffe_net.CopyTrainedLayersFrom(trained_filename);

	const boost::shared_ptr<caffe::Blob<float> > blob_data_layer = caffe_net.blob_by_name("data");
	int image_channel_data_layer = blob_data_layer->channels();
	int image_height_data_layer = blob_data_layer->height();
	int image_width_data_layer = blob_data_layer->width();

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

		if (image_channel_data_layer == 1)
			cv::cvtColor(mat, mat, CV_BGR2GRAY);
		else if (image_channel_data_layer == 4)
			cv::cvtColor(mat, mat, CV_BGR2BGRA);

		cv::resize(mat, mat, cv::Size(image_width_data_layer, image_height_data_layer));
		cv::bitwise_not(mat, mat);

		boost::shared_ptr<caffe::MemoryDataLayer<float> > memory_data_layer =
			boost::static_pointer_cast<caffe::MemoryDataLayer<float>>(caffe_net.layer_by_name("data"));
		mat.convertTo(mat, CV_32FC1, 0.00390625);
		float dummy_label[1] {0};
		memory_data_layer->Reset((float*)(mat.data), dummy_label, 1);

		float loss{ 0.0 };
		const std::vector<caffe::Blob<float>*>& results = caffe_net.ForwardPrefilled(&loss);
		const float* output = results[require_blob_index]->cpu_data();

		float tmp{ -1 };
		int pos{ -1 };

		for (int j = 0; j < 10; j++) {
			//fprintf(stdout, "Probability to be Number %d is: %.3f\n", j, output[j]);
			if (tmp < output[j]) {
				pos = j;
				tmp = output[j];
			}
		}

		result.push_back(pos);
	}

	for (auto i = 0; i < 10; i++)
		fprintf(stdout, "actual digit is: %d, result digit is: %d\n", target[i], result[i]);

	fprintf(stdout, "predict finish\n");
	return 0;
}

