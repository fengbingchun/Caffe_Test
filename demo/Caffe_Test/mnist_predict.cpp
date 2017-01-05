#include "funset.hpp"
#include "common.hpp"

DEFINE_string(model, "E:/GitCode/Caffe_Test/test_data/model/mnist/lenet_train_test_.prototxt",
	"The model definition protocol buffer text file..");
DEFINE_string(weights_predict, "E:/GitCode/Caffe_Test/test_data/model/mnist/lenet_iter_10000.caffemodel",
	"Optional; the pretrained weights to initialize finetuning, "
	"separated by ','. Cannot be set simultaneously with snapshot.");

// A simple registry for caffe commands.
typedef int(*BrewFunction)();
typedef std::map<caffe::string, BrewFunction> BrewMap_predict;
BrewMap_predict g_brew_map_predict;

#define RegisterBrewFunction_predict(func) \
namespace { \
class __Registerer_##func{ \
public: /* NOLINT */ \
	__Registerer_##func() { \
	g_brew_map_predict[#func] = &func; \
	} \
}; \
	__Registerer_##func g_registerer_##func; \
}

static BrewFunction GetBrewFunction(const caffe::string& name) {
	if (g_brew_map_predict.count(name)) {
		return g_brew_map_predict[name];
	}
	else {
		LOG(ERROR) << "Available caffe actions:";
		for (BrewMap_predict::iterator it = g_brew_map_predict.begin();
			it != g_brew_map_predict.end(); ++it) {
			LOG(ERROR) << "\t" << it->first;
		}
		LOG(FATAL) << "Unknown action: " << name;
		return NULL;  // not reachable, just to suppress old compiler warnings.
	}
}

// Test: score a model.
static int test() {
	CHECK_GT(FLAGS_model.size(), 0) << "Need a model definition to score.";
	CHECK_GT(FLAGS_weights_predict.size(), 0) << "Need model weights to score.";

	LOG(INFO) << "Use CPU.";
	Caffe::set_mode(Caffe::CPU);

	// Instantiate the caffe net.
	Net<float> caffe_net(FLAGS_model, caffe::TEST);
	caffe_net.CopyTrainedLayersFrom(FLAGS_weights_predict);

	int target[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	int result[10] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

	std::string image_path = "E:/GitCode/Caffe_Test/test_data/images/";
	for (int i = 0; i < 10; i++) {
		char ch[15];
		sprintf(ch, "%d", i);
		std::string str;
		str = std::string(ch);
		str += ".png";
		str = image_path + str;

		cv::Mat mat = cv::imread(str.c_str(), 1);
		if (!mat.data) {
			std::cout << "load image error" << std::endl;
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
		const vector<Blob<float>*> & results = caffe_net.ForwardPrefilled(&loss);
		float *output = results[1]->mutable_cpu_data();

		float tmp = -1;
		int pos = -1;

		// Display the output
		std::cout << "actuarl digit is: " << i << std::endl;
		for (int j = 0; j < 10; j++) {
			printf("Probability to be Number %d is %.3f\n", j, output[j]);
			if (tmp < output[j]) {
				pos = j;
				tmp = output[j];
			}
		}

		result[i] = pos;
	}

	for (int i = 0; i < 10; i++) {
		std::cout << "actual digit is : " << target[i] << ", result digit is: " << result[i] << std::endl;
	}

	return 0;
}
RegisterBrewFunction_predict(test);

int MNIST_predict()
{
	/* reference:
		https://initialneil.wordpress.com/2015/07/16/caffe-vs2013-opencv-in-windows-tutorial-ii/
		https://initialneil.wordpress.com/2015/01/11/build-caffe-in-windows-with-visual-studio-2013-cuda-6-5-opencv-2-4-9/
		https://github.com/BVLC/caffe/issues/2499
		http://ju.outofmemory.cn/entry/139417
		http://pz124578126.lofter.com/tag/caffe
		https://github.com/BVLC/caffe/pull/1907
	*/
	int argc = 2;
	char* tmp[2] = { "", "" };
	char** argv = &tmp[0];

#ifdef _DEBUG
	argv[0] = "E:/GitCode/Caffe_Test/lib/dbg/x64_vc12/Caffe_Test.exe";
#else  
	argv[0] = "E:/GitCode/Caffe_Test/lib/rel/x64_vc12/Caffe_Test.exe";
#endif 
	argv[1] = "test";

	// 每个进程中至少要执行1次InitGoogleLogging(),否则不产生日志文件
	google::InitGoogleLogging(argv[0]);
	// 设置日志文件保存目录，此目录必须是已经存在的
	FLAGS_log_dir = "E:\\GitCode\\Caffe_Test\\test_data";
	FLAGS_max_log_size = 1024;//MB

	// Print output to stderr (while still logging).
	FLAGS_alsologtostderr = 1;
	// Usage message.
	gflags::SetUsageMessage("command line brew\n"
		"usage: caffe <command> <args>\n\n"
		"commands:\n"
		"  test            score a model");
	// Run tool or show usage.
	//caffe::GlobalInit(&argc, &argv);
	// 解析命令行参数  
	gflags::ParseCommandLineFlags(&argc, &argv, true);

	if (argc == 2) {
		return GetBrewFunction(caffe::string(argv[1]))();
	}
	else {
		gflags::ShowUsageWithFlagsRestrict(argv[0], "tools/caffe");
	}

	std::cout << "OK!!!" << std::endl;

	return 0;
}
