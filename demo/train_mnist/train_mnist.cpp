#include <iostream>
#include <glog/logging.h>
#include <cstring>
#include <map>
#include <string>
#include <vector>

#include "caffe/common.hpp"
#include "boost/algorithm/string.hpp"
#include "caffe/caffe.hpp"
#include "caffe/util/io.hpp"
#include "caffe/blob.hpp"
#include "caffe/layer_factory.hpp"
#include "boost/smart_ptr/shared_ptr.hpp"

using caffe::Blob;
using caffe::Caffe;
using caffe::Net;
using caffe::Layer;
using caffe::Solver;
using caffe::shared_ptr;
using caffe::string;
using caffe::Timer;
using caffe::vector;
using std::ostringstream;

DEFINE_string(solver, "E:/GitCode/Caffe_Test/test_data/model/mnist/lenet_solver.prototxt",
	"The solver definition protocol buffer text file.");
DEFINE_string(snapshot, "E:/GitCode/Caffe_Test/test_data/model/mnist/lenet_iter_10000.solverstate",
	"Optional; the snapshot solver state to resume training.");
DEFINE_string(weights, "E:/GitCode/Caffe_Test/test_data/model/mnist/weights.caffemodel",
	"Optional; the pretrained weights to initialize finetuning, "
	"separated by ','. Cannot be set simultaneously with snapshot.");

// A simple registry for caffe commands.
typedef int(*BrewFunction)();
typedef std::map<caffe::string, BrewFunction> BrewMap;
BrewMap g_brew_map;

#define RegisterBrewFunction(func) \
namespace { \
class __Registerer_##func { \
 public: /* NOLINT */ \
  __Registerer_##func() { \
    g_brew_map[#func] = &func; \
  } \
}; \
__Registerer_##func g_registerer_##func; \
}

static BrewFunction GetBrewFunction(const caffe::string& name) {
	if (g_brew_map.count(name)) {
		return g_brew_map[name];
	}
	else {
		LOG(ERROR) << "Available caffe actions:";
		for (BrewMap::iterator it = g_brew_map.begin(); it != g_brew_map.end(); ++it) {
			LOG(ERROR) << "\t" << it->first;
		}
		LOG(FATAL) << "Unknown action: " << name;
		return NULL;  // not reachable, just to suppress old compiler warnings.
	}
}

// Load the weights from the specified caffemodel(s) into the train and test nets.
void CopyLayers(caffe::Solver<float>* solver, const std::string& model_list) {
	std::vector<std::string> model_names;
	boost::split(model_names, model_list, boost::is_any_of(","));
	for (int i = 0; i < model_names.size(); ++i) {
		LOG(INFO) << "Finetuning from " << model_names[i];
		solver->net()->CopyTrainedLayersFrom(model_names[i]);
		for (int j = 0; j < solver->test_nets().size(); ++j) {
			solver->test_nets()[j]->CopyTrainedLayersFrom(model_names[i]);
		}
	}
}

// Train / Finetune a model.
int train() {
	CHECK_GT(FLAGS_solver.size(), 0) << "Need a solver definition to train.";
	//CHECK(!FLAGS_snapshot.size() || !FLAGS_weights.size()) << "Give a snapshot to resume training or weights to finetune but not both.";

	caffe::SolverParameter solver_param;
	caffe::ReadProtoFromTextFileOrDie(FLAGS_solver, &solver_param);

	Caffe::set_mode(Caffe::CPU);

	shared_ptr<Solver<float> > solver(caffe::GetSolver<float>(solver_param));

	//if (FLAGS_snapshot.size()) { // resume training
	//	LOG(INFO) << "Resuming from " << FLAGS_snapshot;
	//	solver->Restore(FLAGS_snapshot.c_str());
	//}
	//else if (FLAGS_weights.size()) { // finetune
	//	CopyLayers(solver.get(), FLAGS_weights);
	//}

	LOG(INFO) << "Starting Optimization";
	solver->Solve();

	LOG(INFO) << "Optimization Done.";
	return 0;
}
RegisterBrewFunction(train);

int main(int argc, char* argv[])
{
	argc = 2;
#ifdef _DEBUG  
	argv[0] = "E:/GitCode/Caffe_Test/lib/dbg/x64_vc12/train_mnist.exe";
#else  
	argv[0] = "E:/GitCode/Caffe_Test/lib/rel/x64_vc12/train_mnist.exe";
#endif 
	argv[1] = "train";

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
		"  train           train or finetune a model\n");
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

