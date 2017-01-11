#include "funset.hpp"
#include "common.hpp"

int MNIST_train()
{
	Caffe::set_mode(Caffe::CPU);

	const std::string filename{ "E:/GitCode/Caffe_Test/test_data/model/mnist/lenet_solver.prototxt" };

	caffe::SolverParameter solver_param;
	caffe::ReadProtoFromTextFileOrDie(filename, &solver_param);

	shared_ptr<Solver<float> > solver(caffe::GetSolver<float>(solver_param));

	LOG(INFO) << "Starting Optimization";
	solver->Solve();

	LOG(INFO) << "Optimization Done.";

	fprintf(stderr, "train finish\n");
	return 0;
}
