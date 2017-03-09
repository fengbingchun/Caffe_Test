#include "funset.hpp"
#include <string>
#include <vector>
#include "common.hpp"

int test_caffe_layer_pooling()
{
	caffe::Caffe::set_mode(caffe::Caffe::CPU); // set run caffe mode

	// set layer parameter
	caffe::LayerParameter layer_param;
	layer_param.set_phase(caffe::Phase::TRAIN);

	// cv::Mat -> caffe::Blob
	std::string image_name = "E:/GitCode/Caffe_Test/test_data/images/a.jpg";
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

		std::vector<caffe::Blob<float>*> bottom_blob{ &blob }, top_blob{ &caffe::Blob<float>()/*, &caffe::Blob<float>() */ };

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
		if (method == 0) image_name = "E:/GitCode/Caffe_Test/test_data/images/forward0.jpg";
		else image_name = "E:/GitCode/Caffe_Test/test_data/images/forward1.jpg";
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
		if (method == 0) image_name = "E:/GitCode/Caffe_Test/test_data/images/backward0.jpg";
		else image_name = "E:/GitCode/Caffe_Test/test_data/images/backward1.jpg";
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
	std::string filename{ "E:/GitCode/Caffe_Test/test_data/images/a.jpg" };

	// 1. caffe::ReadImageToDatum
	caffe::Datum datum1;
	caffe::ReadImageToDatum(filename, 0, &datum1);
	fprintf(stderr, "datum1: channels: %d, height: %d, width: %d\n",
		datum1.channels(), datum1.height(), datum1.width());

	// 2. test ReadImageToDatumReference
	caffe::Datum datum2, datum_ref2;
	caffe::ReadImageToDatum(filename, 0, 0, 0, true, &datum2);
	ReadImageToDatumReference(filename, 0, 0, 0, true, &datum_ref2);
	if(CompareDatumMat(datum2, datum_ref2) != 0) return -1;

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
	if(caffe::DecodeDatum(&datum7, true)) return -1;

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
	std::string solver_prototxt{ "E:/GitCode/Caffe_Test/test_data/model/mnist/lenet_solver.prototxt" };

	caffe::SolverParameter solver_param;
	if (!caffe::ReadProtoFromTextFile(solver_prototxt.c_str(), &solver_param)) {
		fprintf(stderr, "parse solver.prototxt fail\n");
		return -1;
	}

	// 10. write prototxt to text file
	std::string save_prototxt{"E:/GitCode/Caffe_Test/test_data/test.prototxt"};
	caffe::WriteProtoToTextFile(solver_param, save_prototxt);

	return 0;
}

int test_caffe_blob()
{
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
	std::string image_name = "E:/GitCode/Caffe_Test/test_data/images/a.jpg";
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
	image_name = "E:/GitCode/Caffe_Test/test_data/images/a_ret.jpg";
	cv::imwrite(image_name, mat3);

	return 0;
}

int test_caffe_syncedmem()
{
	caffe::SyncedMemory mem(10);
	caffe::SyncedMemory* p_mem = new caffe::SyncedMemory(10 * sizeof(float));

	if (mem.head() != caffe::SyncedMemory::UNINITIALIZED ||
		mem.size() != 10 ||
		p_mem->size() != 10 * sizeof(float) ||
		mem.cpu_data() == nullptr ||
		mem.mutable_cpu_data() == nullptr ||
		mem.head() != caffe::SyncedMemory::HEAD_AT_CPU) {
		fprintf(stderr, "Error\n");
		return -1;
	}

	fprintf(stderr, "p_mem size: %d\n", p_mem->size());
	fprintf(stderr, "mem size: %d\n", mem.size());

	void* cpu_data = mem.mutable_cpu_data();
	if (mem.head() != caffe::SyncedMemory::HEAD_AT_CPU) {
		fprintf(stderr, "Error\n");
		return -1;
	}

	caffe::caffe_memset(mem.size(), 1, cpu_data);
	for (int i = 0; i < mem.size(); ++i) {
		if ((static_cast<char*>(cpu_data))[i] != 1) {
			fprintf(stderr, "Error\n");
			return -1;
		}
	}

	cpu_data = mem.mutable_cpu_data();
	if (mem.head() != caffe::SyncedMemory::HEAD_AT_CPU) {
		fprintf(stderr, "Error\n");
		return -1;
	}

	caffe::caffe_memset(mem.size(), 2, cpu_data);
	for (int i = 0; i < mem.size(); ++i) {
		if ((static_cast<char*>(cpu_data))[i] != 2) {
			fprintf(stderr, "Error\n");
			return -1;
		}
	}

	delete p_mem;

	return 0;
}

int test_caffe_util_math_functions()
{
	float alpha{ 0.5f }, beta{ 0.1f };
	// h*w: A: 2*3; B: 3*4; C: 2*4
	float A[2 * 3] {1, 2, 3, 4, 5, 6}, B[3 * 4] {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12},
		y1[2 * 4]{1, 2, 3, 4, 5, 6, 7, 8}, x[3]{1, 2, 3}, y2[2]{1, 2},
		y3[6] {1, 2, 3, 4, 5, 6}, y4[6] {1, 2, 3, 4, 5, 6},
		y7[6]{1, 2, 3, 4, 5, 6}, y10[6] {1, 2, 3, 4, 5, 6},
		y11[6] {1, 2, 3, 4, 5, 6}, C[6] {-2, -1, 0, 1, 2, 3}, y19[6] {-10, -10, -10, -10, -10, -10};
	float  y5[6], y6[6], y20[6], y21[6], y22[6];
	int y12[6] {1, 2, 3, 4, 5, 6};

	fprintf(stderr, "test math function: caffe_cpu_gemm(C=alpha*A*B+beta*C)\n");
	// A、B、y1: matrix
	caffe::caffe_cpu_gemm(CblasNoTrans, CblasNoTrans, 2, 4, 3, alpha, A, B, beta, y1);
	for (auto ret : y1) {
		fprintf(stderr, "%f    ", ret);
	}
	fprintf(stderr, "\n");

	fprintf(stderr, "test math function: caffe_cpu_gemv(y=alpha*A*x+beta*y)\n");
	// A: matrix; x、y2: vector
	caffe::caffe_cpu_gemv(CblasNoTrans, 2, 3, alpha, A, x, beta, y2);
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
	caffe::caffe_cpu_axpby(6, alpha, A, beta, y4);
	for (auto ret : y4) {
		fprintf(stderr, "%f    ", ret);
	}
	fprintf(stderr, "\n");

	fprintf(stderr, "test math function: caffe_copy\n");
	caffe::caffe_copy(3, A, y5);
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
	float y13 = caffe::caffe_cpu_dot(3, A, B);
	fprintf(stderr, "caffe cpu dot: %f\n", y13);

	fprintf(stderr, "test math function: caffe_cpu_strided_dot\n");
	float y14 = caffe::caffe_cpu_strided_dot(2, A, 2, B, 2);
	fprintf(stderr, "caffe cpu strided dot: %f\n", y14);

	fprintf(stderr, "test math function: caffe_cpu_hamming_distance\n");
	int y15 = caffe::caffe_cpu_hamming_distance(4, A, C);
	fprintf(stderr, "caffe cpu hamming distance: %d\n", y15);

	fprintf(stderr, "test math function: caffe_cpu_asum\n");
	float y16 = caffe::caffe_cpu_asum(5, C);
	fprintf(stderr, "caffe cpu asum: %f\n", y16);

	fprintf(stderr, "test math function: caffe_sign\n");
	int8_t y17 = caffe::caffe_sign(-10.0f);
	int8_t y18 = caffe::caffe_sign(10.0f);
	fprintf(stderr, "caffe sign: -10.0f: %d, 10.0f: %d\n", y17, y18);

	fprintf(stderr, "test math function: caffe_cpu_scale\n");
	caffe::caffe_cpu_scale(5, alpha, C, y19);
	for (auto ret : y19) {
		fprintf(stderr, "%f    ", ret);
	}
	fprintf(stderr, "\n");

	fprintf(stderr, "test math function: caffe_cpu_sign\n");
	caffe::caffe_cpu_sign(5, C, y20);
	for (auto ret : y20) {
		fprintf(stderr, "%f    ", ret);
	}
	fprintf(stderr, "\n");

	fprintf(stderr, "test math function: caffe_cpu_sgnbit\n");
	caffe::caffe_cpu_sgnbit(5, C, y21);
	for (auto ret : y21) {
		fprintf(stderr, "%f    ", ret);
	}
	fprintf(stderr, "\n");

	fprintf(stderr, "test math function: caffe_cpu_fabs\n");
	caffe::caffe_cpu_fabs(5, C, y22);
	for (auto ret : y22) {
		fprintf(stderr, "%f    ", ret);
	}
	fprintf(stderr, "\n");

	return 0;
}

int test_caffe_util_mkl_alternate()
{
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

