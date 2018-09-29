#include "funset.hpp"
#include <string>
#include <vector>
#include <map>
#include "common.hpp"

int image_joint()
{
#ifdef _MSC_VER
	const std::string image_path{ "E:/GitCode/Caffe_Test/test_data/images/object_recognition/" };
#else
	const std::string image_path{ "test_data/images/object_recognition/" };
#endif
	const int width{ 160 }, height{ 160 };

	cv::Mat matDst(height * 2, width * 5, CV_8UC3);
	for (int i = 0; i < 10; ++i) {
		std::string str = std::to_string(i) + ".jpg";
		cv::Mat matSrc = cv::imread(image_path + str);
		if (!matSrc.data) {
			fprintf(stderr, "image does not exist: %s\n", str.c_str());
			return -1;
		}

		cv::resize(matSrc, matSrc, cv::Size(width, height), 0.f, 0.f, 3);
		int x = (i * width) % (width * 5);
		int y = (i / 5) * height;
		cv::Mat part = matDst(cv::Rect(x, y, width, height));
		matSrc.copyTo(part);
	}

	const std::string output_image = image_path + "result.jpg";
	cv::imwrite(output_image, matDst);

	return 0;
}

int campute_image_mean(const std::string& db_type, const std::string& db_path, std::vector<float>& image_mean, const std::string& binaryproto)
{
#ifdef CPU_ONLY
	caffe::Caffe::set_mode(caffe::Caffe::CPU);
#else
	caffe::Caffe::set_mode(caffe::Caffe::GPU);
#endif

	// reference: caffe/tools/compute_image_mean.cpp
	boost::scoped_ptr<caffe::db::DB> db(caffe::db::GetDB(db_type));
	db->Open(db_path, caffe::db::READ);
	boost::scoped_ptr<caffe::db::Cursor> cursor(db->NewCursor());

	caffe::BlobProto sum_blob;
	int count = 0;
	// load first datum
	caffe::Datum datum;
	datum.ParseFromString(cursor->value());

	caffe::DecodeDatumNative(&datum);

	sum_blob.set_num(1);
	sum_blob.set_channels(datum.channels());
	sum_blob.set_height(datum.height());
	sum_blob.set_width(datum.width());
	const int data_size = datum.channels() * datum.height() * datum.width();
	int size_in_datum = std::max<int>(datum.data().size(), datum.float_data_size());
	for (int i = 0; i < size_in_datum; ++i) {
		sum_blob.add_data(0.);
	}

	// Starting Iteration
	while (cursor->valid()) {
		caffe::Datum datum2;
		datum2.ParseFromString(cursor->value());
		caffe::DecodeDatumNative(&datum2);

		const std::string& data = datum2.data();
		size_in_datum = std::max<int>(datum2.data().size(), datum2.float_data_size());
		if (size_in_datum != data_size) {
			fprintf(stderr, "incorrect data field size: size_in_datum: %d, data_size: %d\n",
				size_in_datum, data_size);
			return -1;
		}
		if (data.size() != 0) {
			if (data.size() != size_in_datum) {
				fprintf(stderr, "data.size() != size_in_datum: data.size: %d, size_in_datum: %d\n",
					data.size(), size_in_datum);
				return -1;
			}
			for (int i = 0; i < size_in_datum; ++i) {
				sum_blob.set_data(i, sum_blob.data(i) + (uint8_t)data[i]);
			}
		} else {
			if (datum2.float_data_size() != size_in_datum) {
				fprintf(stderr, "datum.float_data_size() != size_in_datum: datum.float_data_size: %d, size_in_datum: %d",
					datum2.float_data_size(), size_in_datum);
				return -1;
			}
			for (int i = 0; i < size_in_datum; ++i) {
				sum_blob.set_data(i, sum_blob.data(i) + static_cast<float>(datum2.float_data(i)));
			}
		}
		++count;
		if (count % 10000 == 0) {
			fprintf(stderr, "Processed: %d files\n", count);
		}
		cursor->Next();
	}

	if (count % 10000 != 0) {
		fprintf(stderr, "Processed: %d files\n", count);
	}
	for (int i = 0; i < sum_blob.data_size(); ++i) {
		sum_blob.set_data(i, sum_blob.data(i) / count);
	}

	// Write to disk
	if (binaryproto != "") {
		caffe::WriteProtoToBinaryFile(sum_blob, binaryproto);
	}

	const int channels = sum_blob.channels();
	const int dim = sum_blob.height() * sum_blob.width();
	image_mean.resize(channels, 0.0);
	fprintf(stderr, "Number of channels: %d", channels);
	for (int c = 0; c < channels; ++c) {
		for (int i = 0; i < dim; ++i) {
			image_mean[c] += sum_blob.data(dim * c + i);
		}
		image_mean[c] /= dim;
	}
	fprintf(stderr, "\n");

	return 0;
}

int cifar10_compute_image_mean()
{
	// Blog: http://blog.csdn.net/fengbingchun/article/details/72416951
	const std::string db_type{ "lmdb" };
	std::vector<float> image_mean;
#ifdef _MSC_VER
	const std::string db_path{ "E:/GitCode/Caffe_Test/test_data/cifar10/cifar10_train_lmdb" };
	std::string binaryproto{ "E:/GitCode/Caffe_Test/test_data/model/cifar10/mean.binaryproto" };
#else
	const std::string db_path{ "test_data/cifar10/cifar10_train_lmdb" };
	std::string binaryproto{ "test_data/model/cifar10/mean.binaryproto" };
#endif
	if (campute_image_mean(db_type, db_path, image_mean, binaryproto) != 0) {
		fprintf(stderr, "compute image mean fail\n");
		return -1;
	}

	fprintf(stderr, "image mean:");
	for (const auto& value : image_mean) {
		fprintf(stderr, "  %f  ", value);
	}
	fprintf(stderr, "\n");

	return 0;
}

int test_caffe_solver()
{
	// Blog: http://blog.csdn.net/fengbingchun/article/details/64444599
#ifdef CPU_ONLY
	caffe::Caffe::set_mode(caffe::Caffe::CPU);
#else
	caffe::Caffe::set_mode(caffe::Caffe::GPU);
#endif

#ifdef _MSC_VER
	const std::string solver_prototxt{ "E:/GitCode/Caffe_Test/test_data/model/mnist/lenet_solver.prototxt" };
#else
	const std::string solver_prototxt{ "test_data/model/mnist/lenet_solver_linux.prototxt" };
#endif

	caffe::SolverParameter solver_param;
	if (!caffe::ReadProtoFromTextFile(solver_prototxt.c_str(), &solver_param)) {
		fprintf(stderr, "parse solver.prototxt fail\n");
		return -1;
	}

	caffe::SGDSolver<float> solver(solver_param);
	caffe::SolverParameter param = solver.param();

	if (param.has_net())
		fprintf(stderr, "net: %s\n", param.net().c_str());
	if (param.has_net_param()) {
		fprintf(stderr, "has net param\n");
		caffe::NetParameter net_param = param.net_param();
		if (net_param.has_name())
			fprintf(stderr, "net param name: %s\n", net_param.name().c_str());
	}
	if (param.has_train_net())
		fprintf(stderr, "train_net: %s\n", param.train_net().c_str());
	if (param.test_net().size() > 0) {
		for (auto test_net : param.test_net())
			fprintf(stderr, "test_net: %s\n", test_net.c_str());
	}
	if (param.has_train_net_param()) {
		fprintf(stderr, "has train net param\n");
		caffe::NetParameter train_net_param = param.train_net_param();
	}
	if (param.test_net_param().size() > 0) {
		fprintf(stderr, "has test net param\n");
		std::vector<caffe::NetParameter> test_net_param;
		for (auto net_param : param.test_net_param())
			test_net_param.push_back(net_param);
	}

	if (param.has_train_state()) {
		fprintf(stderr, "has train state\n");
		caffe::NetState state = param.train_state();
	}
	if (param.test_state().size()) {
		fprintf(stderr, "has test state\n");
	}

	if (param.test_iter_size() > 0) {
		fprintf(stderr, "has test iter\n");
		for (auto iter : param.test_iter())
			fprintf(stderr, "  %d  ", iter);
		fprintf(stderr, "\n");
	}

	if (param.has_test_interval())
		fprintf(stderr, "test interval: %d\n", param.test_interval());
	bool test_compute_loss = param.test_compute_loss();
	fprintf(stderr, "test compute loss: %d\n", test_compute_loss);
	bool test_initialization = param.test_initialization();
	fprintf(stderr, "test initializtion: %d\n", test_initialization);
	if (param.has_base_lr()) {
		float base_lr = param.base_lr();
		fprintf(stderr, "base lr: %f\n", base_lr);
	}
	if (param.has_display()) {
		int display = param.display();
		fprintf(stderr, "display: %d\n", display);
	}
	int average_loss = param.average_loss();
	fprintf(stderr, "average loss: %d\n", average_loss);
	if (param.has_max_iter()) {
		int max_iter = param.max_iter();
		fprintf(stderr, "max iter: %d\n", max_iter);
	}
	int iter_size = param.iter_size();
	fprintf(stderr, "iter size: %d\n", iter_size);

	if (param.has_lr_policy())
		fprintf(stderr, "lr policy: %s\n", param.lr_policy().c_str());
	if (param.has_gamma())
		fprintf(stderr, "gamma: %f\n", param.gamma());
	if (param.has_power())
		fprintf(stderr, "power: %f\n", param.power());
	if (param.has_momentum())
		fprintf(stderr, "momentum: %f\n", param.momentum());
	if (param.has_weight_decay())
		fprintf(stderr, "weight decay: %f\n", param.weight_decay());
	std::string regularization_type = param.regularization_type();
	fprintf(stderr, "regularization type: %s\n", param.regularization_type().c_str());
	if (param.has_stepsize())
		fprintf(stderr, "stepsize: %d\n", param.stepsize());
	if (param.stepvalue_size() > 0) {
		fprintf(stderr, "has stepvalue\n");
		for (auto value : param.stepvalue())
			fprintf(stderr, "  %d  ", value);
		fprintf(stderr, "\n");
	}

	fprintf(stderr, "clip gradients: %f\n", param.clip_gradients());

	fprintf(stderr, "snapshot: %d\n", param.snapshot());
	if (param.has_snapshot_prefix())
		fprintf(stderr, "snapshot prefix: %s\n", param.snapshot_prefix().c_str());
	fprintf(stderr, "snapshot diff: %d\n", param.snapshot_diff());
	caffe::SolverParameter_SnapshotFormat snapshot_format = param.snapshot_format();
	fprintf(stderr, "snapshot format: %s\n", snapshot_format == 0 ? "HDF5" : "BINARYPROTO");
	caffe::SolverParameter_SolverMode solver_mode = param.solver_mode();
	fprintf(stderr, "solver mode: %s\n", solver_mode == 0 ? "CPU" : "GPU");
	if (param.has_device_id())
		fprintf(stderr, "device id: %d\n", param.device_id());
	fprintf(stderr, "random seed: %d\n", param.random_seed());

	caffe::SolverParameter_SolverType solver_type = param.solver_type();
	std::string solver_method[] {"SGD", "NESTEROV", "ADAGRAD", "RMSPROP", "ADADELTA", "ADAM"};
	fprintf(stderr, "solver type: %s\n", solver_method[solver_type].c_str());
	fprintf(stderr, "delta: %f\n", param.delta());
	fprintf(stderr, "momentum2: %f\n", param.momentum2());

	if (param.has_rms_decay())
		fprintf(stderr, "rms decy: %f\n", param.rms_decay());

	fprintf(stderr, "debug info: %d\n", param.debug_info());
	fprintf(stderr, "snapshot after train: %d\n", param.snapshot_after_train());

	boost::shared_ptr<caffe::Net<float>> net = solver.net();
	std::vector<boost::shared_ptr<caffe::Net<float>>> test_nets = solver.test_nets();
	fprintf(stderr, "test nets size: %d\n", test_nets.size());
	fprintf(stderr, "iter: %d\n", solver.iter());

	return 0;
}

int test_caffe_net2()
{
	// Blog: http://blog.csdn.net/fengbingchun/article/details/62423060
#ifdef CPU_ONLY
	caffe::Caffe::set_mode(caffe::Caffe::CPU);
#else
	caffe::Caffe::set_mode(caffe::Caffe::GPU);
#endif

	// reference: caffe/src/caffe/test/test_net.cpp
#ifdef _MSC_VER
	std::string prototxt{ "E:/GitCode/Caffe_Test/test_data/model/test_net_8.prototxt" };
#else
	std::string prototxt{ "test_data/model/test_net_8.prototxt" };
#endif
	caffe::Phase phase = caffe::Phase::TRAIN;

	// 1. Net(const string& param_file, Phase phase)
	boost::shared_ptr<caffe::Net<float>> net(new caffe::Net<float>(prototxt, phase));

	//caffe::Caffe::set_random_seed(1701);

	{
		// 2. Dtype ForwardBackward()
		float loss = net->ForwardBackward();
		fprintf(stderr, "loss: %f\n", loss);
	}

	{
		// 3. Dtype ForwardFromTo(int start, int end)
		float loss = net->ForwardFromTo(0, net->layers().size() - 1);
		// 4. void BackwardFromTo(int start, int end)
		net->BackwardFromTo(net->layers().size() - 1, 0);
		fprintf(stderr, "loss: %f\n", loss);
	}

	{
		// 5.  Dtype ForwardTo(int end)
		float loss = net->ForwardTo(net->layers().size() - 2);
		// 6. void BackwardFrom(int start)
		net->BackwardFrom(net->layers().size() - 2);
		fprintf(stderr, "loss: %f\n", loss);
	}

	{
		// 7. Dtype ForwardFrom(int start)
		float loss = net->ForwardFrom(1);
		// 8. void BackwardTo(int end)
		net->BackwardTo(1);
		fprintf(stderr, "loss: %f\n", loss);
	}

	{
		// 9. vector<Blob<Dtype>*>& ForwardPrefilled(Dtype* loss = NULL)
		float loss;
		std::vector<caffe::Blob<float>*> net_output_blobs = net->ForwardPrefilled(&loss);
		// 10. void Backward()
		net->Backward();
		fprintf(stderr, "net output blobs size: %d; loss: %f\n", net_output_blobs.size(), loss);
	}

	{
		// 11. string Forward(const vector<Blob<Dtype>* > & bottom, Dtype* loss = NULL)
		const std::vector<caffe::Blob<float>* > bottom;
		float loss;
		const std::vector<caffe::Blob<float>*>& blob = net->Forward(bottom, &loss);
		net->Backward();
		fprintf(stderr, "output blob size: %d; loss: %f\n", blob.size(), loss);
	}

	{
		// 12. vector<Blob<Dtype>*>& Forward(const vector<Blob<Dtype>* > & bottom, Dtype* loss = NULL)
		std::vector<caffe::Blob<float>*> bottom;
		float loss;
		std::vector<caffe::Blob<float>*> net_output_blobs = net->Forward(bottom, &loss);
		net->Backward();
		fprintf(stderr, "net output blobs size: %d; loss: %f\n", net_output_blobs.size(), loss);
	}

	// 13. void ShareWeights()
	net->ShareWeights();
	// 14. void Update()
	net->Update();
	// 15. void Reshape()
	net->Reshape();
	// 16. void ClearParamDiffs()
	net->ClearParamDiffs();

	// 17. void CopyTrainedLayersFrom(const NetParameter& param)
	caffe::NetParameter net_param;
	net->ToProto(&net_param, false);
	net->CopyTrainedLayersFrom(net_param);

	// 加载已训练好的模型
	// 18. void CopyTrainedLayersFrom(const string trained_filename)
	std::string trained_filename{ " " };
	//net->CopyTrainedLayersFrom(trained_filename);
	// 19. void CopyTrainedLayersFromBinaryProto(const string trained_filename)
	//net->CopyTrainedLayersFromBinaryProto(trained_filename);
	// 20. void CopyTrainedLayersFromHDF5(const string trained_filename)
	//net->CopyTrainedLayersFromHDF5(trained_filename);

	// 21. void ShareTrainedLayersWith(const Net* other)
	caffe::Net<float> net1(prototxt, phase);
	net->ShareTrainedLayersWith(&net1);

	// 22. static void FilterNet(const NetParameter& param, NetParameter* param_filtered)
	caffe::NetParameter param1, param2;
	net->FilterNet(param1, &param2);

	// 23. static bool StateMeetsRule(const NetState& state, const NetStateRule& rule, const string& layer_name)
	const caffe::NetState state;
	const caffe::NetStateRule rule;
	const std::string layer_name;
	bool ret = net->StateMeetsRule(state, rule, layer_name);
	fprintf(stderr, "state meet rule: %d\n", ret);

	return 0;
}

int test_caffe_net1()
{
	// Blog: http://blog.csdn.net/fengbingchun/article/details/62423060
#ifdef CPU_ONLY
	caffe::Caffe::set_mode(caffe::Caffe::CPU);
#else
	caffe::Caffe::set_mode(caffe::Caffe::GPU);
#endif

	// reference: caffe/src/caffe/test/test_net.cpp
#ifdef _MSC_VER
	std::string prototxt{"E:/GitCode/Caffe_Test/test_data/model/test_net_8.prototxt"}; // 1~8
#else
	std::string prototxt{"test_data/model/test_net_8.prototxt"}; // 1~8
#endif
	caffe::NetParameter param;
	caffe::ReadNetParamsFromTextFileOrDie(prototxt, &param);

	// 1. Net(const NetParameter& param)
	boost::shared_ptr<caffe::Net<float>> net(new caffe::Net<float>(param));

	// 2. const string& name()
	std::string name = net->name();
	fprintf(stderr, "Net name: %s\n", name.c_str());

	// 3. const vector<string>& layer_names()
	std::vector<std::string> layer_names = net->layer_names();
	fprintf(stderr, "print all layer names: layer size: %d\n", layer_names.size());
	for (auto layer_name : layer_names) {
		fprintf(stderr, "    %s\n", layer_name.c_str());
	}

	// 4. const vector<string>& blob_names()
	std::vector<std::string> blob_names = net->blob_names();
	fprintf(stderr, "print all blob names: blob size:  %d\n", blob_names.size());
	for (auto blob_name : blob_names) {
		fprintf(stderr, "    %s\n", blob_name.c_str());
	}

	// 5. const vector<shared_ptr<Blob<Dtype> > >& blobs()
	std::vector<boost::shared_ptr<caffe::Blob<float>>> blobs = net->blobs();
	fprintf(stderr, "print all blobs dim: blob size: %d\n", blobs.size());
	for (auto blob : blobs) {
		std::vector<int> shape = blob->shape();
		fprintf(stderr, "blob dim: %d, ", shape.size());
		for (auto value : shape) {
			fprintf(stderr, "  %d  ", value);
		}
		fprintf(stderr, "\n");
	}

	// 6. const vector<shared_ptr<Layer<Dtype> > >& layers()
	std::vector<boost::shared_ptr<caffe::Layer<float>>> layers = net->layers();
	fprintf(stderr, "print all layers bottom and top blobs num: layer size: %d\n", layers.size());
	for (const auto layer : layers) {
		fprintf(stderr, "layer type: %s, bottom blob num: %d, top blob num: %d\n",
			layer->type(), layer->ExactNumBottomBlobs(), layer->ExactNumTopBlobs());
	}

	// 7. Phase phase()
	caffe::Phase phase = net->phase();
	fprintf(stderr, "net phase: %d\n", phase);

	// 8. const vector<vector<Blob<Dtype>*> >& bottom_vecs()
	std::vector<std::vector<caffe::Blob<float>*>> bottom_vecs = net->bottom_vecs();
	fprintf(stderr, "print layer bottom blob: layer size: %d\n", bottom_vecs.size());
	for (auto layer : bottom_vecs) {
		for (auto blob : layer) {
			fprintf(stderr, "layer blob shape: %s\n", (blob->shape_string()).c_str());
		}
	}

	// 9. const vector<vector<Blob<Dtype>*> >& top_vecs()
	std::vector<std::vector<caffe::Blob<float>*>> top_vecs = net->top_vecs();
	fprintf(stderr, "print layer top blol: layer size: %d\n", top_vecs.size());
	for (auto layer : top_vecs) {
		for (const auto blob : layer) {
			fprintf(stderr, "layer top shape: %s\n", (blob->shape_string()).c_str());
		}
	}

	// 10. const vector<vector<bool> >& bottom_need_backward()
	std::vector<std::vector<bool>> bottom_need_backward = net->bottom_need_backward();
	fprintf(stderr, "print bottom need backward info: layer size: %d\n", bottom_need_backward.size());
	for (auto layer : bottom_need_backward) {
		for (auto flag : layer) {
			fprintf(stderr, "  %s  ", flag ? "true" : "false");
		}
		fprintf(stderr, "\n");
	}
	fprintf(stderr, "\n");

	// 11. const vector<Dtype>& blob_loss_weights()
	std::vector<float> blob_loss_weights = net->blob_loss_weights();
	fprintf(stderr, "print blob loss weights: blob size: %d\n", blob_loss_weights.size());
	for (auto weight : blob_loss_weights) {
		fprintf(stderr, "weight: %f\n", weight);
	}

	// 12. const vector<bool>& layer_need_backward()
	std::vector<bool> layer_need_backward = net->layer_need_backward();
	fprintf(stderr, "print layer need backward: layer size: %d\n", layer_need_backward.size());
	for (auto flag : layer_need_backward) {
		fprintf(stderr, "layer need backward: %s\n", flag ? "true" : "false");
	}

	// 13. const vector<shared_ptr<Blob<Dtype> > >& params()
	std::vector<boost::shared_ptr<caffe::Blob<float>>> params = net->params();
	fprintf(stderr, "print net params info: blob size: %d\n", params.size());
	for (auto blob : params) {
		fprintf(stderr, "blob shape: %s\n", blob->shape_string().c_str());
	}

	// 14. const vector<Blob<Dtype>*>& learnable_params()
	std::vector<caffe::Blob<float>*> learnable_params = net->learnable_params();
	fprintf(stderr, "print learnable params info: blob size: %d\n", learnable_params.size());
	for (const auto blob : learnable_params) {
		fprintf(stderr, "blob shape: %s\n", blob->shape_string().c_str());
	}

	// 15. const vector<float>& params_lr()
	std::vector<float> params_lr = net->params_lr();
	fprintf(stderr, "print learnable rate info: size: %d\n", params_lr.size());
	for (auto value : params_lr) {
		fprintf(stderr, "learnable rate: %f\n", value);
	}

	// 16. const vector<bool>& has_params_lr()
	std::vector<bool> has_params_lr = net->has_params_lr();
	fprintf(stderr, "print has learnable rate info: size: %d\n", has_params_lr.size());
	for (auto flag : has_params_lr) {
		fprintf(stderr, "has learnable rate: %s\n", flag ? "true" : "false");
	}

	// 17. const vector<float>& params_weight_decay()
	std::vector<float> params_weight_decay = net->params_weight_decay();
	fprintf(stderr, "print weight decay info: size: %d\n", params_weight_decay.size());
	for (auto value : params_weight_decay) {
		fprintf(stderr, "weight decay: %f\n", value);
	}

	// 18. const vector<bool>& has_params_decay()
	std::vector<bool> has_params_decay = net->has_params_decay();
	fprintf(stderr, "print has decay info: size: %d\n", has_params_decay.size());
	for (auto value : has_params_decay) {
		fprintf(stderr, "has decay: %s\n", value ? "true" : "false");
	}

	// 19. const map<string, int>& param_names_index()
	const std::map<std::string, int> param_names_index = net->param_names_index();
	fprintf(stderr, "print param names index info: size: %d\n", param_names_index.size());
	auto it = param_names_index.begin();
	while (it != param_names_index.end()) {
		fprintf(stderr, "param names index: %s : %d\n", it->first.c_str(), it->second);
		++it;
	}

	// 20. const vector<int>& param_owners()
	std::vector<int> param_owers = net->param_owners();
	fprintf(stderr, "print param owers info: size: %d\n", param_owers.size());
	for (auto value : param_owers) {
		fprintf(stderr, "param owers: %d\n", value);
	}

	// 21. int num_inputs() const
	int num_inputs = net->num_inputs();
	fprintf(stderr, "num inputs: %d\n", num_inputs);

	// 22. int num_outputs() const
	int num_outputs = net->num_outputs();
	fprintf(stderr, "num outputs: %d\n", num_outputs);

	// 23. const vector<Blob<Dtype>*>& input_blobs()
	const std::vector<caffe::Blob<float>*> input_blobs = net->input_blobs();
	fprintf(stderr, "print input blobs info: %d\n", input_blobs.size());
	for (auto blob : input_blobs) {
		fprintf(stderr, "input blob shape: %s\n", blob->shape_string().c_str());
	}

	// 24. const vector<Blob<Dtype>*>& output_blobs()
	const std::vector<caffe::Blob<float>*> output_blobs = net->output_blobs();
	fprintf(stderr, "print output blobs info: %d\n", output_blobs.size());
	for (auto blob : output_blobs) {
		fprintf(stderr, "output blob shape: %s\n", blob->shape_string().c_str());
	}

	// 25. const vector<int>& input_blob_indices()
	std::vector<int> input_blob_indices = net->input_blob_indices();
	fprintf(stderr, "print input blob indices info: size: %d\n", input_blob_indices.size());
	for (auto value : input_blob_indices) {
		fprintf(stderr, "input blob indices: %d\n", value);
	}

	// 26. const vector<int>& output_blob_indices()
	std::vector<int> output_blob_indices = net->output_blob_indices();
	fprintf(stderr, "print output blob indices info: size: %d\n", output_blob_indices.size());
	for (auto value : output_blob_indices) {
		fprintf(stderr, "output blob indices: %d\n", value);
	}

	// 27. bool has_blob(const string& blob_name)
	bool has_blob1 = net->has_blob("data");
	bool has_blob2 = net->has_blob("loss");
	fprintf(stderr, "net has blob data: %d, has blob loss: %d\n", has_blob1, has_blob2);

	// 28. const shared_ptr<Blob<Dtype> > blob_by_name
	const std::vector<std::string> blob_by_names{ "innerproduct", "loss" };
	for (auto name : blob_by_names) {
		const boost::shared_ptr<caffe::Blob<float>> blob = net->blob_by_name(name);
		if (blob != nullptr)
			fprintf(stderr, "blob shape: %s\n", blob->shape_string().c_str());
		else
			fprintf(stderr, "unknown blob name: %s\n", name.c_str());
	}

	// 29. bool has_layer(const string& layer_name)
	const std::vector<std::string> has_layers{"innerproduct", "top_loss"};
	for (auto name : has_layers) {
		bool has_layer = net->has_layer(name);
		fprintf(stderr, "has layer %s: %d\n", name.c_str(), has_layer);
	}

	// 30. const shared_ptr<Layer<Dtype> > layer_by_name
	const std::vector<std::string> layer_by_names{ "data", "top_loss" };
	for (auto name : layer_by_names) {
		const boost::shared_ptr<caffe::Layer<float>> layer = net->layer_by_name(name);
		if (layer != nullptr)
			fprintf(stderr, "layer type: %s\n", layer->type());
		else
			fprintf(stderr, "unknown layer name: %s\n", name.c_str());
	}

	// 31. void set_debug_info(const bool value)
	net->set_debug_info(true);

	// 32. void ToHDF5(const string& filename, bool write_diff = false)
#ifdef _MSC_VER
	// std::string hdf5_name{"E:/GitCode/Caffe_Test/test_data/hdf5.h5"};
#else
	// std::string hdf5_name{"test_data/hdf5.h5"};
#endif
	// net->ToHDF5(hdf5_name, false); // Note: some .prototxt will crash

	// 33. void ToProto(NetParameter* param, bool write_diff = false)
	caffe::NetParameter param2;
	net->ToProto(&param2, false);
	fprintf(stderr, "new net name: %s\n", param2.name().c_str());

	return 0;
}

int test_caffe_layer_pooling()
{
	// Blog: http://blog.csdn.net/fengbingchun/article/details/60962777
#ifdef CPU_ONLY
	caffe::Caffe::set_mode(caffe::Caffe::CPU);
#else
	caffe::Caffe::set_mode(caffe::Caffe::GPU);
#endif

	// set layer parameter
	caffe::LayerParameter layer_param;
	layer_param.set_phase(caffe::Phase::TRAIN);

	// cv::Mat -> caffe::Blob
#ifdef _MSC_VER
	std::string image_name = "E:/GitCode/Caffe_Test/test_data/images/a.jpg";
#else
	std::string image_name = "test_data/images/a.jpg";
#endif
	cv::Mat mat1 = cv::imread(image_name, 1);
	if (!mat1.data) {
		fprintf(stderr, "read image fail: %s\n", image_name.c_str());
		return -1;
	}
	mat1.convertTo(mat1, CV_32FC3);
	std::vector<cv::Mat> mat2;
	cv::split(mat1, mat2);
	std::vector<int> mat_reshape{ 1, (int)mat2.size(), mat2[0].rows, mat2[0].cols };

	caffe::Blob<float> blob;
	blob.Reshape(mat_reshape);
	size_t size = mat2[0].rows * mat2[0].cols;
	float* data = new float[mat2.size() * size];
	memcpy(data, mat2[0].data, size * sizeof(float));
	memcpy(data + size, mat2[1].data, size * sizeof(float));
	memcpy(data + 2 * size, mat2[2].data, size * sizeof(float));
	blob.set_cpu_data(data);

	for (int method = 0; method < 2; ++method) {
		// set pooling parameter
		caffe::PoolingParameter* pooling_param = layer_param.mutable_pooling_param();
		if (method == 0) pooling_param->set_pool(caffe::PoolingParameter::MAX);
		else pooling_param->set_pool(caffe::PoolingParameter::AVE);
		pooling_param->set_kernel_size(3);
		pooling_param->set_pad(2);
		pooling_param->set_stride(2);
		pooling_param->set_global_pooling(false);
		
		caffe::Blob<float> tmp;
		std::vector<caffe::Blob<float>*> bottom_blob{ &blob }, top_blob{&tmp};

		// test PoolingLayer function
		caffe::PoolingLayer<float> pooling_layer(layer_param);
		pooling_layer.SetUp(bottom_blob, top_blob);
		fprintf(stderr, "top blob info: channels: %d, height: %d, width: %d\n",
			top_blob[0]->channels(), top_blob[0]->height(), top_blob[0]->width());

		pooling_layer.Forward(bottom_blob, top_blob);

		int height = top_blob[0]->height();
		int width = top_blob[0]->width();

		const float* p = top_blob[0]->cpu_data();

		std::vector<cv::Mat> mat3{ cv::Mat(height, width, CV_32FC1, (float*)p),
			cv::Mat(height, width, CV_32FC1, (float*)(p + height * width)),
			cv::Mat(height, width, CV_32FC1, (float*)(p + height * width * 2)) };
		cv::Mat mat4;
		cv::merge(mat3, mat4);
		mat4.convertTo(mat4, CV_8UC3);
#ifdef _MSC_VER
		if (method == 0) image_name = "E:/GitCode/Caffe_Test/test_data/images/forward0.jpg";
		else image_name = "E:/GitCode/Caffe_Test/test_data/images/forward1.jpg";
#else
		if (method == 0) image_name = "test_data/images/forward0.jpg";
		else image_name = "test_data/images/forward1.jpg";
#endif
		cv::imwrite(image_name, mat4);

		for (int i = 0; i < bottom_blob[0]->count(); ++i)
			bottom_blob[0]->mutable_cpu_diff()[i] = bottom_blob[0]->cpu_data()[i];

		for (int i = 0; i < top_blob[0]->count(); ++i)
			top_blob[0]->mutable_cpu_diff()[i] = top_blob[0]->cpu_data()[i];

		std::vector<bool> propagate_down{ true };
		pooling_layer.Backward(top_blob, propagate_down, bottom_blob);

		height = bottom_blob[0]->height();
		width = bottom_blob[0]->width();
		p = bottom_blob[0]->cpu_diff();
		std::vector<cv::Mat> mat5{ cv::Mat(height, width, CV_32FC1, (float*)p),
			cv::Mat(height, width, CV_32FC1, (float*)(p + height * width)),
			cv::Mat(height, width, CV_32FC1, (float*)(p + height * width * 2)) };
		cv::Mat mat6;
		cv::merge(mat5, mat6);
		mat6.convertTo(mat6, CV_8UC3);
#ifdef _MSC_VER
		if (method == 0) image_name = "E:/GitCode/Caffe_Test/test_data/images/backward0.jpg";
		else image_name = "E:/GitCode/Caffe_Test/test_data/images/backward1.jpg";
#else
		if (method == 0) image_name = "test_data/images/backward0.jpg";
		else image_name = "test_data/images/backward1.jpg";
#endif
		cv::imwrite(image_name, mat6);
	}

	delete[] data;
	return 0;
}

static bool ReadImageToDatumReference(const std::string& filename, const int label,
	const int height, const int width, const bool is_color, caffe::Datum* datum)
{
	cv::Mat cv_img;
	int cv_read_flag = (is_color ? CV_LOAD_IMAGE_COLOR : CV_LOAD_IMAGE_GRAYSCALE);

	cv::Mat cv_img_origin = cv::imread(filename, cv_read_flag);
	if (!cv_img_origin.data) {
		fprintf(stderr, "Could not open or find file: %s\n", filename.c_str());
		return false;
	}
	if (height > 0 && width > 0)
		cv::resize(cv_img_origin, cv_img, cv::Size(width, height));
	else
		cv_img = cv_img_origin;


	int num_channels = (is_color ? 3 : 1);
	datum->set_channels(num_channels);
	datum->set_height(cv_img.rows);
	datum->set_width(cv_img.cols);
	datum->set_label(label);
	datum->clear_data();
	datum->clear_float_data();
	std::string* datum_string = datum->mutable_data();

	if (is_color) {
		for (int c = 0; c < num_channels; ++c) {
			for (int h = 0; h < cv_img.rows; ++h) {
				for (int w = 0; w < cv_img.cols; ++w) {
					datum_string->push_back(static_cast<char>(cv_img.at<cv::Vec3b>(h, w)[c]));
				}
			}
		}
	} else {  // Faster than repeatedly testing is_color for each pixel w/i loop
		for (int h = 0; h < cv_img.rows; ++h) {
			for (int w = 0; w < cv_img.cols; ++w) {
				datum_string->push_back(static_cast<char>(cv_img.at<uchar>(h, w)));
			}
		}
	}

	return true;
}

static int CompareDatumMat(const caffe::Datum& datum1, const caffe::Datum& datum2)
{
	if (datum1.channels() != datum2.channels() || datum1.height() != datum2.height() ||
		datum1.width() != datum2.width() || datum1.data().size() != datum2.data().size()) {
		fprintf(stderr, "these values should be equal\n");
		return -1;
	}

	const std::string& data1 = datum1.data();
	const std::string& data2 = datum2.data();
	for (int i = 0; i < datum1.data().size(); ++i) {
		if (data1[i] != data2[i]) {
			fprintf(stderr, "their data should be equal\n");
			return -1;
		}
	}

	return 0;
}

static int CompareDatumMat(const caffe::Datum& datum, const cv::Mat& mat)
{
	if (datum.channels() != mat.channels() || datum.height() != mat.rows || datum.width() != mat.cols) {
		fprintf(stderr, "these values should be equal\n");
		return -1;
	}

	const std::string& datum_data = datum.data();
	int index = 0;
	for (int c = 0; c < mat.channels(); ++c) {
		for (int h = 0; h < mat.rows; ++h) {
			for (int w = 0; w < mat.cols; ++w) {
				if (datum_data[index++] != static_cast<char>(mat.at<cv::Vec3b>(h, w)[c])) {
					fprintf(stderr, "their data should be equal\n");
					return -1;
				}
			}
		}
	}

	return 0;
}

static int CompareDatumMat(const cv::Mat& mat1, const cv::Mat& mat2)
{
	if (mat1.channels() != mat2.channels() || mat1.rows != mat2.rows || mat1.cols != mat2.cols) {
		fprintf(stderr, "these values should be equal\n");
		return -1;
	}

	for (int c = 0; c < mat1.channels(); ++c) {
		for (int h = 0; h < mat1.rows; ++h) {
			for (int w = 0; w < mat1.cols; ++w) {
				if (mat1.at<cv::Vec3b>(h, w)[c] != mat2.at<cv::Vec3b>(h, w)[c]) {
					fprintf(stderr, "their data should be equal\n");
					return -1;
				}
			}
		}
	}

	return 0;
}

int test_caffe_util_io()
{
	// Blog: http://blog.csdn.net/fengbingchun/article/details/60140215
#ifdef CPU_ONLY
	caffe::Caffe::set_mode(caffe::Caffe::CPU);
#else
	caffe::Caffe::set_mode(caffe::Caffe::GPU);
#endif

#ifdef _MSC_VER 
	std::string filename{ "E:/GitCode/Caffe_Test/test_data/images/a.jpg" };
#else
	std::string filename{ "test_data/images/a.jpg" };
#endif

	// 1. caffe::ReadImageToDatum
	caffe::Datum datum1;
	caffe::ReadImageToDatum(filename, 0, &datum1);
	fprintf(stderr, "datum1: channels: %d, height: %d, width: %d\n",
		datum1.channels(), datum1.height(), datum1.width());

	// 2. test ReadImageToDatumReference
	caffe::Datum datum2, datum_ref2;
	caffe::ReadImageToDatum(filename, 0, 0, 0, true, &datum2);
	ReadImageToDatumReference(filename, 0, 0, 0, true, &datum_ref2);
	if (CompareDatumMat(datum2, datum_ref2) != 0) return -1;

	// 3. test ReadImageToDatumReferenceResized
	caffe::Datum datum3, datum_ref3;
	caffe::ReadImageToDatum(filename, 0, 100, 200, true, &datum3);
	ReadImageToDatumReference(filename, 0, 100, 200, true, &datum_ref3);
	if (CompareDatumMat(datum3, datum_ref3) != 0) return -1;

	// 4. test ReadImageToDatumContent
	caffe::Datum datum4;
	caffe::ReadImageToDatum(filename, 0, &datum4);
	cv::Mat cv_img = caffe::ReadImageToCVMat(filename);
	if (CompareDatumMat(datum4, cv_img) != 0) return -1;

	// 5. test CVMatToDatumContent
	cv_img = caffe::ReadImageToCVMat(filename);
	caffe::Datum datum5;
	caffe::CVMatToDatum(cv_img, &datum5);
	caffe::Datum datum_ref5;
	caffe::ReadImageToDatum(filename, 0, &datum_ref5);
	if (CompareDatumMat(datum5, datum_ref5) != 0) return -1;

	// 6. test ReadFileToDatum
	caffe::Datum datum6;
	if (!caffe::ReadFileToDatum(filename, &datum6)) {
		fprintf(stderr, "read file to datum fail\n");
		return -1;
	}
	fprintf(stderr, "datum encoded: %d; datum label: %d, datum size: %d\n",
		datum6.encoded(), datum6.label(), datum6.data().size());

	// 7. test DecodeDatum
	caffe::Datum datum7;
	caffe::ReadFileToDatum(filename, &datum7);
	if (!caffe::DecodeDatum(&datum7, true)) return -1;
	if (caffe::DecodeDatum(&datum7, true)) return -1;

	caffe::Datum datum_ref7;
	ReadImageToDatumReference(filename, 0, 0, 0, true, &datum_ref7);
	if (CompareDatumMat(datum7, datum_ref7) != 0) return -1;

	// 8. test DecodeDatumToCVMatContent
	caffe::Datum datum8;
	if (!caffe::ReadImageToDatum(filename, 0, std::string("jpg"), &datum8)) return -1;
	cv::Mat cv_img8 = caffe::DecodeDatumToCVMat(datum8, true);
	cv::Mat cv_img_ref = caffe::ReadImageToCVMat(filename);
	// if (CompareDatumMat(cv_img8, cv_img_ref) != 0) return -1; // Note: some values are not equal

	// 9. read prototxt and parse
#ifdef _MSC_VER
	std::string solver_prototxt{ "E:/GitCode/Caffe_Test/test_data/model/mnist/lenet_solver.prototxt" };
#else
	std::string solver_prototxt{ "test_data/model/mnist/lenet_solver_linux.prototxt" };
#endif

	caffe::SolverParameter solver_param;
	if (!caffe::ReadProtoFromTextFile(solver_prototxt.c_str(), &solver_param)) {
		fprintf(stderr, "parse solver.prototxt fail\n");
		return -1;
	}

	// 10. write prototxt to text file
#ifdef _MSC_VER
	std::string save_prototxt{ "E:/GitCode/Caffe_Test/test_data/test.prototxt" };
#else
	std::string save_prototxt{ "test_data/test.prototxt" };
#endif
	caffe::WriteProtoToTextFile(solver_param, save_prototxt);

	// 11. make temp dir/filename
	std::string temp_dirname{ " " };
	caffe::MakeTempDir(&temp_dirname);
	fprintf(stderr, "temp dir name: %s\n", temp_dirname.c_str());
	std::string temp_filename{ " " };
	caffe::MakeTempFilename(&temp_filename);
	fprintf(stderr, "temp file name: %s\n", temp_filename.c_str());
	caffe::RemoveCaffeTempDir();

	return 0;
}

int test_caffe_blob()
{
	// Blog: http://blog.csdn.net/fengbingchun/article/details/59106613
#ifdef CPU_ONLY
	caffe::Caffe::set_mode(caffe::Caffe::CPU);
#else
	caffe::Caffe::set_mode(caffe::Caffe::GPU);
#endif
	caffe::Blob<float> blob1;

	std::vector<int> shape{ 2, 3, 4, 5 };
	caffe::Blob<float> blob2(shape);

	std::vector<int> blob_shape = blob2.shape();
	fprintf(stderr, "blob shape: ");
	for (auto index : blob_shape) {
		fprintf(stderr, "%d    ", index);
	}
	std::vector<int> shape_{ 6, 7, 8, 9 };
	blob2.Reshape(shape_);
	std::vector<int> blob_shape_ = blob2.shape();
	fprintf(stderr, "\nnew blob shape: ");
	for (auto index : blob_shape_) {
		fprintf(stderr, "%d    ", index);
	}
	fprintf(stderr, "\n");

	int value = blob2.shape(-1);
	fprintf(stdout, "blob index -1: %d\n", value);
	int num_axes = blob2.num_axes();
	fprintf(stderr, "blob num axes(dimension): %d\n", num_axes);
	int count = blob2.count();
	fprintf(stderr, "blob count sum: %d\n", count);
	count = blob2.count(2, 4);
	fprintf(stderr, "blob count(start_axis(2), end_axis(4)): %d\n", count);
	count = blob2.count(1);
	fprintf(stderr, "blob count(start_axis(1)): %d\n", count);
	int canonical_axis_index = blob2.CanonicalAxisIndex(-2);
	fprintf(stderr, "blob canonical axis index: %d\n", canonical_axis_index);

	int num = blob2.num();
	int channels = blob2.channels();
	int height = blob2.height();
	int width = blob2.width();
	int legacy_shape = blob2.LegacyShape(-2);
	fprintf(stderr, "blob num: %d, channels: %d, height: %d, width: %d, legacy shape(-2): %d\n",
		num, channels, height, width, legacy_shape);

	std::vector<int> indices{ 2, 3, 7, 6 };
	int offset1 = blob2.offset(indices);
	int offset2 = blob2.offset(indices[0], indices[1], indices[2], indices[3]);
	fprintf(stderr, "blob offset1: %d, offset2: %d\n", offset1, offset2);

	std::string shape_string = blob2.shape_string();
	fprintf(stderr, "shape string: %s\n", shape_string.c_str());

	caffe::BlobProto blob_proto;
	blob_proto.set_num(6);
	blob_proto.set_channels(7);
	blob_proto.set_height(8);
	blob_proto.set_width(9);

	bool flag = blob2.ShapeEquals(blob_proto);
	fprintf(stderr, "blob2's shape and blob_proto's shape are equal: %d\n", flag);
	int blob_proto_data_size_float = blob_proto.data_size();
	int blob_proto_data_size_double = blob_proto.double_data_size();
	int blob_proto_diff_size_float = blob_proto.diff_size();
	int blob_proto_diff_size_double = blob_proto.double_diff_size();
	fprintf(stderr, "blob_proto data/diff size: %d, %d, %d, %d\n", blob_proto_data_size_float,
		blob_proto_data_size_double, blob_proto_diff_size_float, blob_proto_diff_size_double);

	caffe::BlobShape blob_proto_shape;
	for (int i = 0; i < 4; ++i) {
		blob_proto_shape.add_dim(i + 10);
	}
	blob2.Reshape(blob_proto_shape);
	blob_shape_ = blob2.shape();
	fprintf(stderr, "new blob shape: ");
	for (auto index : blob_shape_) {
		fprintf(stderr, "%d    ", index);
	}
	fprintf(stderr, "\n");

	fprintf(stderr, "blob proto shape: ");
	for (int i = 0; i < blob_proto_shape.dim_size(); ++i) {
		fprintf(stderr, "%d    ", blob_proto_shape.dim(i));
	}
	fprintf(stderr, "\n");

	// 注：以上进行的所有操作均不会申请分配任何内存

	// cv::Mat -> Blob
#ifdef _MSC_VER
	std::string image_name = "E:/GitCode/Caffe_Test/test_data/images/a.jpg";
#else
	std::string image_name = "test_data/images/a.jpg";
#endif
	cv::Mat mat = cv::imread(image_name, 1);
	if (!mat.data) {
		fprintf(stderr, "read image fail: %s\n", image_name.c_str());
		return -1;
	}
	cv::Mat mat2;
	mat.convertTo(mat2, CV_32FC3);
	std::vector<int> mat_reshape{ 1, mat2.channels(), mat2.rows, mat2.cols };
	blob2.Reshape(mat_reshape);
	float sum1 = blob2.asum_data();
	blob2.set_cpu_data((float*)mat2.data);
	float sum2 = blob2.asum_data();
	blob2.scale_data(0.5);
	float sum3 = blob2.asum_data();
	float sum4 = blob2.sumsq_data();
	fprintf(stderr, "sum1: %f, sum2: %f, sum3: %f, sum4: %f\n", sum1, sum2, sum3, sum4);

	float value2 = blob2.data_at(0, 2, 100, 200);
	fprintf(stderr, "data at value: %f\n", value2);

	const float* data = blob2.cpu_data();
	fprintf(stderr, "data at 0: %f\n", data[0]);

	cv::Mat mat3;
	mat2.convertTo(mat3, CV_8UC3);
#ifdef _MSC_VER
	image_name = "E:/GitCode/Caffe_Test/test_data/images/a_ret.jpg";
#else
	image_name = "test_data/images/a_ret.jpg";
#endif
	cv::imwrite(image_name, mat3);

	return 0;
}

int test_caffe_syncedmem()
{
	// Blog: http://blog.csdn.net/fengbingchun/article/details/56665919
#ifdef CPU_ONLY
	caffe::Caffe::set_mode(caffe::Caffe::CPU);
#else
	caffe::Caffe::set_mode(caffe::Caffe::GPU);
#endif

	caffe::SyncedMemory mem(10);
	caffe::SyncedMemory* p_mem = new caffe::SyncedMemory(10 * sizeof(float));

	if (mem.head() != caffe::SyncedMemory::UNINITIALIZED ||
		mem.size() != 10 ||
		p_mem->size() != 10 * sizeof(float) ||
#ifdef CPU_ONLY
		mem.cpu_data() == nullptr ||
		mem.mutable_cpu_data() == nullptr ||
		mem.head() != caffe::SyncedMemory::HEAD_AT_CPU
#else
		mem.gpu_data() == nullptr ||
		mem.mutable_gpu_data() == nullptr ||
		mem.head() != caffe::SyncedMemory::HEAD_AT_GPU
#endif
		) {
		fprintf(stderr, "Error: Line: %d\n", __LINE__);
		return -1;
	}

	fprintf(stderr, "p_mem size: %d\n", p_mem->size());
	fprintf(stderr, "mem size: %d\n", mem.size());

	void* cpu_data = mem.mutable_cpu_data();
	if (mem.head() != caffe::SyncedMemory::HEAD_AT_CPU) {
		fprintf(stderr, "Error: Line: %d\n", __LINE__);
		return -1;
	}

	caffe::caffe_memset(mem.size(), 1, cpu_data);
	for (int i = 0; i < mem.size(); ++i) {
		if ((static_cast<char*>(cpu_data))[i] != 1) {
			fprintf(stderr, "Error: Line: %d\n", __LINE__);
			return -1;
		}
	}

	cpu_data = mem.mutable_cpu_data();
	if (mem.head() != caffe::SyncedMemory::HEAD_AT_CPU) {
		fprintf(stderr, "Error: Line: %d\n", __LINE__);
		return -1;
	}

	caffe::caffe_memset(mem.size(), 2, cpu_data);
	for (int i = 0; i < mem.size(); ++i) {
		if ((static_cast<char*>(cpu_data))[i] != 2) {
			fprintf(stderr, "Error: Line: %d\n", __LINE__);
			return -1;
		}
	}

	delete p_mem;

	return 0;
}

int test_caffe_util_math_functions()
{
	// Blog: http://blog.csdn.net/fengbingchun/article/details/56280708
#ifdef CPU_ONLY
	caffe::Caffe::set_mode(caffe::Caffe::CPU);
#else
	caffe::Caffe::set_mode(caffe::Caffe::GPU);
#endif
	float alpha{ 0.5f }, beta{ 0.1f };
	// h*w: A: 2*3; B: 3*4; C: 2*4
	float A[2 * 3] {1, 2, 3, 4, 5, 6}, B[3 * 4] {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12},
		y1[2 * 4]{1, 2, 3, 4, 5, 6, 7, 8}, x[3]{1, 2, 3}, y2[2]{1, 2},
		y3[6] {1, 2, 3, 4, 5, 6}, y4[6] {1, 2, 3, 4, 5, 6},
		y7[6]{1, 2, 3, 4, 5, 6}, y10[6] {1, 2, 3, 4, 5, 6},
		y11[6] {1, 2, 3, 4, 5, 6}, C[6] {-2, -1, 0, 1, 2, 3}, y19[6] {-10, -10, -10, -10, -10, -10};
	float  y5[6] {0.f, 0.f, 0.f, 0.f, 0.f, 0.f}, y6[6], y20[6], y21[6], y22[6];
	int y12[6] {1, 2, 3, 4, 5, 6};

	fprintf(stderr, "test math function: caffe_cpu_gemm(C=alpha*A*B+beta*C)\n");
	// A、B、y1: matrix
#ifdef CPU_ONLY
	caffe::caffe_cpu_gemm(CblasNoTrans, CblasNoTrans, 2, 4, 3, alpha, A, B, beta, y1);
#else
	caffe::caffe_gpu_gemm(CblasNoTrans, CblasNoTrans, 2, 4, 3, alpha, A, B, beta, y1);
#endif
	for (auto ret : y1) {
		fprintf(stderr, "%f    ", ret);
	}
	fprintf(stderr, "\n");

	fprintf(stderr, "test math function: caffe_cpu_gemv(y=alpha*A*x+beta*y)\n");
	// A: matrix; x、y2: vector
#ifdef CPU_ONLY
	caffe::caffe_cpu_gemv(CblasNoTrans, 2, 3, alpha, A, x, beta, y2);
#else
	caffe::caffe_gpu_gemv(CblasNoTrans, 2, 3, alpha, A, x, beta, y2);
#endif
	for (auto ret : y2) {
		fprintf(stderr, "%f    ", ret);
	}
	fprintf(stderr, "\n");

	fprintf(stderr, "test math function: caffe_axpy(Y=alpha*X+Y)\n");
	caffe::caffe_axpy(6, alpha, A, y3);
	for (auto ret : y3) {
		fprintf(stderr, "%f    ", ret);
	}
	fprintf(stderr, "\n");

	fprintf(stderr, "test math function: caffe_cpu_axpby(Y= alpha*X+beta*Y)\n");
#ifdef CPU_ONLY
	caffe::caffe_cpu_axpby(6, alpha, A, beta, y4);
#else
	caffe::caffe_gpu_axpby(6, alpha, A, beta, y4);
#endif
	for (auto ret : y4) {
		fprintf(stderr, "%f    ", ret);
	}
	fprintf(stderr, "\n");

	fprintf(stderr, "test math function: caffe_copy\n");
#ifdef CPU_ONLY
	caffe::caffe_copy(3, A, y5); // Note: GPU will crash
#endif
	for (auto ret : y5) {
		fprintf(stderr, "%f    ", ret);
	}
	fprintf(stderr, "\n");

	fprintf(stderr, "test math function: caffe_set\n");
	caffe::caffe_set(3, alpha, y6);
	for (auto ret : y6) {
		fprintf(stderr, "%f    ", ret);
	}
	fprintf(stderr, "\n");

	fprintf(stderr, "test math function: caffe_scal(X=alpha*X)\n");
	caffe::caffe_scal(4, alpha, y7);
	for (auto ret : y7) {
		fprintf(stderr, "%f    ", ret);
	}
	fprintf(stderr, "\n");

	fprintf(stderr, "test math function: caffe_rng_rand\n");
	unsigned int y8 = caffe::caffe_rng_rand();
	fprintf(stderr, "caffe rng rand: %d\n", y8);

	fprintf(stderr, "test math function: caffe_nextafter\n");
	float y9 = caffe::caffe_nextafter(alpha);
	fprintf(stderr, " caffe next after: %f\n", y9);

	fprintf(stderr, "test math function: caffe_rng_uniform\n");
	caffe::caffe_rng_uniform(4, -2.f, 2.f, y10);
	for (auto ret : y10) {
		fprintf(stderr, "%f    ", ret);
	}
	fprintf(stderr, "\n");

	fprintf(stderr, "test math function: caffe_rng_gaussian\n");
	caffe::caffe_rng_gaussian(4, -2.f, alpha, y11);
	for (auto ret : y11) {
		fprintf(stderr, "%f    ", ret);
	}
	fprintf(stderr, "\n");

	fprintf(stderr, "test math function: caffe_rng_bernoulli\n");
	caffe::caffe_rng_bernoulli(4, alpha, y12);
	for (auto ret : y12) {
		fprintf(stderr, "%d    ", ret);
	}
	fprintf(stderr, "\n");

	fprintf(stderr, "test math function: caffe_cpu_dot\n");
#ifdef CPU_ONLY
	float y13 = caffe::caffe_cpu_dot(3, A, B);
#else
	float y13{ 0.f };
	//caffe::caffe_gpu_dot(3, A, B, &y13); // Note: GPU will crash
#endif
	fprintf(stderr, "caffe cpu dot: %f\n", y13);

	fprintf(stderr, "test math function: caffe_cpu_strided_dot\n");
#ifdef CPU_ONLY
	float y14 = caffe::caffe_cpu_strided_dot(2, A, 2, B, 2);
	fprintf(stderr, "caffe cpu strided dot: %f\n", y14);
#endif

	fprintf(stderr, "test math function: caffe_cpu_asum\n");
#ifdef CPU_ONLY
	float y16 = caffe::caffe_cpu_asum(5, C);
#else
	float y16{ 0.f };
	//caffe::caffe_gpu_asum(5, C, &y16); // Note: GPU will crash
#endif
	fprintf(stderr, "caffe cpu asum: %f\n", y16);

	fprintf(stderr, "test math function: caffe_sign\n");
	int8_t y17 = caffe::caffe_sign(-10.0f);
	int8_t y18 = caffe::caffe_sign(10.0f);
	fprintf(stderr, "caffe sign: -10.0f: %d, 10.0f: %d\n", y17, y18);

	fprintf(stderr, "test math function: caffe_cpu_scale\n");
#ifdef CPU_ONLY
	caffe::caffe_cpu_scale(5, alpha, C, y19);
#else
	caffe::caffe_gpu_scale(5, alpha, C, y19);
#endif
	for (auto ret : y19) {
		fprintf(stderr, "%f    ", ret);
	}
	fprintf(stderr, "\n");

	fprintf(stderr, "test math function: caffe_cpu_sign\n");
#ifdef CPU_ONLY
	caffe::caffe_cpu_sign(5, C, y20);
#else
	caffe::caffe_gpu_sign(5, C, y20);
#endif
	for (auto ret : y20) {
		fprintf(stderr, "%f    ", ret);
	}
	fprintf(stderr, "\n");

	fprintf(stderr, "test math function: caffe_cpu_sgnbit\n");
#ifdef CPU_ONLY
	caffe::caffe_cpu_sgnbit(5, C, y21);
#else
	caffe::caffe_gpu_sgnbit(5, C, y21);
#endif
	for (auto ret : y21) {
		fprintf(stderr, "%f    ", ret);
	}
	fprintf(stderr, "\n");

	fprintf(stderr, "test math function: caffe_cpu_fabs\n");
#ifdef CPU_ONLY
	caffe::caffe_cpu_fabs(5, C, y22);
#else
	//caffe::caffe_gpu_fabs(5, C, y22); // no implement
#endif
	for (auto ret : y22) {
		fprintf(stderr, "%f    ", ret);
	}
	fprintf(stderr, "\n");

	return 0;
}

int test_caffe_util_mkl_alternate()
{
	// Blog: http://blog.csdn.net/fengbingchun/article/details/56280708
#ifdef CPU_ONLY
	caffe::Caffe::set_mode(caffe::Caffe::CPU);
#else
	caffe::Caffe::set_mode(caffe::Caffe::GPU);
#endif

	const int N{ 5 };
	float a[N] {1, 2, 3, 4, 5}, b{ 2 }, alpha{ 0.2f }, beta{0.4f};
	float y1[N], y2[N], y3[N], y4[N]{6, 7, 8, 9, 10};
	
	fprintf(stderr, "test unary function: vsSqr\n");
	vsSqr(N, a, y1);
	for (auto ret : y1) {
		fprintf(stderr, "%f    ", ret);
	}
	fprintf(stderr, "\n");

	fprintf(stderr, "test function unary function with singular parameter: vsPowx\n");
	vsPowx(N, a, b, y2);
	for (auto ret : y2) {
		fprintf(stderr, "%f    ", ret);
	}
	fprintf(stderr, "\n");

	fprintf(stderr, "test binary function: vsAdd\n");
	vsAdd(N, a, a, y3);
	for (auto ret : y3) {
		fprintf(stderr, "%f    ", ret);
	}
	fprintf(stderr, "\n");

	fprintf(stderr, "test axpby function(Y=alpha*X+beta*Y): cblas_saxpby\n");
	cblas_saxpby(N, alpha, a, 1, beta, y4, 1);
	for (auto ret : y4) {
		fprintf(stderr, "%f    ", ret);
	}
	fprintf(stderr, "\n");

	return 0;
}

int get_layer_type_list()
{
	// Blog: http://blog.csdn.net/fengbingchun/article/details/54310956
#ifdef CPU_ONLY
	caffe::Caffe::set_mode(caffe::Caffe::CPU);
#else
	caffe::Caffe::set_mode(caffe::Caffe::GPU);
#endif

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
	// Blog: http://blog.csdn.net/fengbingchun/article/details/54955236
	// 1. test macro NOT_IMPLEMENTED
	//NOT_IMPLEMENTED; // error, fatal log

	// 2. test global initialization function GlobalInit
	int argc = 2;
	char** argv = nullptr;
	argv = new char*[2];
#ifdef CPU_ONLY
#ifdef _MSC_VER
#ifdef _DEBUG
	argv[0] = "E:/GitCode/Caffe_Test/lib/dbg/x64_vc12/Caffe_Test.exe";
#else
	argv[0] = "E:/GitCode/Caffe_Test/lib/rel/x64_vc12/Caffe_Test.exe";
#endif // _DEBUG
#else
	argv[0] = "/Caffe_Test.exe";
#endif // _MSC_VER
#else
#ifdef _MSC_VER
#ifdef _DEBUG
	argv[0] = "E:/GitCode/Caffe_Test/lib/dbg/x64_vc12/Caffe_GPU_Test.exe";
#else
	argv[0] = "E:/GitCode/Caffe_Test/lib/rel/x64_vc12/Caffe_GPU_Test.exe";
#endif // _DEBUG
#else
	argv[0] = "Caffe_Test";
#endif // _MSC_VER
#endif // CPU_ONLY
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
#ifdef CPU_ONLY
	caffe::Caffe::set_mode(caffe::Caffe::CPU);
#else
	caffe::Caffe::set_mode(caffe::Caffe::GPU);
#endif
	caffe::Caffe::Brew run_mode = caffe1.mode();
	fprintf(stderr, "0: CPU, 1: GPU, run_mode: %d\n", run_mode);

	// set and get solver_count
	fprintf(stderr, "solver count: %d\n", caffe::Caffe::solver_count());
	caffe::Caffe::set_solver_count(5);
	fprintf(stderr, "solver count: %d\n", caffe::Caffe::solver_count());
	caffe::Caffe::set_solver_count(1);

	// set and get solver_rank
	fprintf(stderr, "solver rank: %d\n", caffe::Caffe::solver_rank());
	caffe::Caffe::set_solver_rank(4);
	fprintf(stderr, "solver rank: %d\n", caffe::Caffe::solver_rank());
	caffe::Caffe::set_solver_rank(0);

	// set multiprocess
	fprintf(stderr, "multi process: %d\n", caffe::Caffe::multiprocess());
	caffe::Caffe::set_multiprocess(true);
	fprintf(stderr, "multi process: %d\n", caffe::Caffe::multiprocess());
	caffe::Caffe::set_multiprocess(false);

	// root solver
	fprintf(stderr, "root solver: %d\n", caffe::Caffe::root_solver());

#ifndef CPU_ONLY
	// find device
	int devide_id = caffe::Caffe::FindDevice(0);
	fprintf(stderr, "the first available device: %d\n", devide_id);
	// set device
	caffe::Caffe::SetDevice(devide_id);
	// device query
	caffe::Caffe::DeviceQuery();
#endif

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
#ifdef CPU_ONLY
	caffe::Caffe::set_mode(caffe::Caffe::CPU);
#else
	caffe::Caffe::set_mode(caffe::Caffe::GPU);
#endif

	return 0;
}

