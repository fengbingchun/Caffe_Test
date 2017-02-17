#include "funset.hpp"
#include <string>
#include "common.hpp"

int test_caffe_proto()
{


	return 0;
}

int get_layer_type_list()
{
	caffe::LayerRegistry<double>::CreatorRegistry& registry = caffe::LayerRegistry<double>::Registry();

	std::vector<std::string> layers_list;
	for (caffe::LayerRegistry<double>::CreatorRegistry::iterator iter = registry.begin(); iter != registry.end(); ++iter) {
		layers_list.push_back(iter->first);
	}

	fprintf(stdout, "layer count: %d\n", layers_list.size());
	for (int i = 0; i < layers_list.size(); i++) {
		fprintf(stdout, "%d:    %s\n", i+1, layers_list[i].c_str());
	}

	return 0;
}

int test_caffe_common()
{
	// 1. test macro NOT_IMPLEMENTED
	//NOT_IMPLEMENTED; // error, fatal log

	// 2. test global initialization function GlobalInit
	int argc = 2;
	char** argv = nullptr;
	argv = new char*[2];
#ifdef _DEBUG
	argv[0] = "E:/GitCode/Caffe_Test/lib/dbg/x64_vc12/Caffe_Test.exe";
#else
	argv[0] = "E:/GitCode/Caffe_Test/lib/rel/x64_vc12/Caffe_Test.exe";
#endif
	argv[1] = "caffe_test";

	caffe::GlobalInit(&argc, &argv);

	delete[] argv;

	// 3. test caffe class
	// caffe::Caffe caffe_; // error, can't create Caffe object directly

	// verify Caffe is a singleton class
	caffe::Caffe& caffe1 = caffe::Caffe::Get();
	caffe::Caffe& caffe2 = caffe::Caffe::Get();
	fprintf(stderr, "caffe1 addr: %p\n", &caffe1);
	fprintf(stderr, "caffe2 addr: %p\n", &caffe2);
	auto addr_caffe1 = std::addressof(caffe1);
	auto addr_caffe2 = std::addressof(caffe2);
	if (addr_caffe1 != addr_caffe2) {
		fprintf(stderr, "caffe1 and caffe2 addr are different: caffe1 addr: %p, caffe2 addr: %p\n", &caffe1, &caffe2);
		return -1;
	}

	// get run mode: CPU or GPU
	caffe::Caffe::Brew run_mode = caffe1.mode();
	fprintf(stderr, "0: CPU, 1: GPU, run_mode: %d\n", run_mode);

	// set solver_count
	caffe::Caffe::set_solver_count(5);
	// get solver_count
	int solver_count = caffe::Caffe::solver_count();
	fprintf(stderr, "solver count: %d\n", solver_count);

	// set root_solver
	caffe::Caffe::set_root_solver(false);
	// get root_solver
	bool root_solver = caffe::Caffe::root_solver();
	fprintf(stderr, "root solver: %d\n", root_solver);

	// set device
	// caffe::Caffe::SetDevice(2); // error, fatal log: Cannot use GPU in CPU-only Caffe: check mode.
	// device query
	// caffe::Caffe::DeviceQuery(); // error, fatal log: Cannot use GPU in CPU-only Caffe: check mode.

	// RNG: generate random number
	caffe::SyncedMemory data_a(10 * sizeof(int));
	caffe::Caffe::set_random_seed(8888);
	caffe::caffe_rng_bernoulli(10, 0.5, static_cast<int*>(data_a.mutable_cpu_data()));

	caffe::SyncedMemory data_b(10 * sizeof(int));
	caffe::Caffe::set_random_seed(8888);
	caffe::caffe_rng_bernoulli(10, 0.5, static_cast<int*>(data_b.mutable_cpu_data()));

	for (int i = 0; i < 10; ++i) {
		fprintf(stderr, "%d, %d\n", static_cast<const int*>(data_a.cpu_data())[i], static_cast<const int*>(data_b.cpu_data())[i]);
		if (static_cast<const int*>(data_a.cpu_data())[i] != static_cast<const int*>(data_b.cpu_data())[i]) {
			fprintf(stderr, "same seed should be generate same random number\n");
			return - 1;
		}
	}

	// set run mode
	caffe::Caffe::set_mode(caffe::Caffe::GPU);
	run_mode = caffe1.mode();
	fprintf(stderr, "0: CPU, 1: GPU, run_mode: %d\n", run_mode);

	return 0;
}

