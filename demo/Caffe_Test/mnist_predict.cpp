#include "funset.hpp"
#include "common.hpp"

int MNIST_predict()
{
	Caffe::set_mode(Caffe::CPU);

	const std::string param_file{ "E:/GitCode/Caffe_Test/test_data/model/mnist/lenet_train_test_.prototxt" };
	const std::string trained_filename{ "E:/GitCode/Caffe_Test/test_data/model/mnist/lenet_iter_10000.caffemodel" };
	const std::string image_path{ "E:/GitCode/Caffe_Test/test_data/images/" };

	// Instantiate the caffe net.
	Net<float> caffe_net(param_file, caffe::TEST);
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
		vector<cv::Mat> patches;
		patches.push_back(mat);

		// push vector<Mat> to data layer
		float loss = 0.0;
		boost::shared_ptr<caffe::MemoryDataLayer<float> > memory_data_layer;
		memory_data_layer = boost::static_pointer_cast<caffe::MemoryDataLayer<float>>(caffe_net.layer_by_name("data"));

		vector<int> labels(patches.size());
		memory_data_layer->AddMatVector(patches, labels);

		// Net forward
		const vector<Blob<float>*>& results = caffe_net.ForwardPrefilled(&loss);
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
