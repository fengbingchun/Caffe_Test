#include "protobuf_test.hpp"

#include <iostream>
#include <fstream>

#include "ml.helloworld.pb.h"
#include "caffe_tmp.pb.h"

static void ListMsg(const lm::helloworld& msg)
{
	std::cout << msg.id() << std::endl;
	std::cout << msg.str() << std::endl;
}

int test_protobuf_1()
{
	/* reference:
		http://www.ibm.com/developerworks/cn/linux/l-cn-gpb/
		http://blog.163.com/jiang_tao_2010/blog/static/12112689020114305013458/
		http://www.cnblogs.com/dkblog/archive/2012/03/27/2419010.html
	*/
	// 1-->首先编写一个ml.helloworld.proto文件，内容如下：
	/*
	syntax = "proto2";
	package lm;

	message helloworld
	{
	required int32     id = 1;  // ID
	required string    str = 2;  // str
	optional int32     opt = 3;  //optional field
	}
	*/

	// 2-->利用protoc.exe生成ml.helloworld.pb.h和ml.hellowrold.pb.cc
	// protoc.exe ml.helloworld.proto --cpp_out=./

	// 3-->Writer,将把一个结构化数据写入磁盘，以便其他人来读取
	lm::helloworld msg1;
	msg1.set_id(101);
	msg1.set_str("hello");

#ifdef _MSC_VER
	const std::string data_path = "E:/GitCode/Caffe_Test/test_data/third_party_library/";
#else
	const std::string data_path = "test_data/third_party_library/";
#endif
	// Write the new address book back to disk. 
	std::string file_proto = data_path + "log";
	std::fstream output(file_proto.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
	if (!msg1.SerializeToOstream(&output)) {
		std::cerr << "Failed to write msg." << std::endl;
		return -1;
	}
	output.close();

	// 4-->Reader,读取结构化数据,log文件
	lm::helloworld msg2;

	std::fstream input(file_proto.c_str(), std::ios::in | std::ios::binary);
	if (!msg2.ParseFromIstream(&input)) {
		std::cerr << "Failed to parse address book." << std::endl;
		return -1;
	}
	input.close();

	ListMsg(msg2);

	return 0;
}

int test_protobuf_2()
{
	// Blog: http://blog.csdn.net/fengbingchun/article/details/49977903
	// 1. 首先编写一个caffe_tmp.proto文件，然后通过protoc.exe生成相应的pb.h、pb.cc文件
	// protoc.exe caffe_tmp.proto --cpp_out=./
	// caffe_tmp.proto文件存放在test_data/third_party_library目录下

	// 2. 序列化，将数据存入文件
	caffe_tmp::BlobShape blobShape;
	blobShape.add_dim(4);
	blobShape.add_dim(8);
	int size_blobShape = blobShape.ByteSize();

	caffe_tmp::BlobProto blobProto;
	blobProto.add_data(1.5);
	blobProto.add_diff(3.0);
	blobProto.set_channels(3);
	blobProto.set_height(100);
	blobProto.set_width(200);
	blobProto.set_num(5);
	blobProto.set_data(0, -1.0);
	blobProto.add_data(3.3);
	caffe_tmp::BlobShape* blobShape1 = blobProto.mutable_shape();
	blobShape1->add_dim(10);
	int size_blobProto = blobProto.ByteSize();

	caffe_tmp::Datum datum;
	unsigned char tmp1[5] = { 1, 2, 3, 4, 5 };
	datum.set_data(tmp1, 5);
	int size_datum = datum.ByteSize();

	caffe_tmp::NetState netState;
	std::string str[3] = { "hello", "protobuf", "caffe" };
	netState.add_stage(str[0]);
	netState.add_stage(str[1]);
	netState.add_stage(str[2]);
	caffe_tmp::Phase phase = caffe_tmp::Phase::TRAIN;
	netState.set_phase(phase);
	int size_netState = netState.ByteSize();

	caffe_tmp::LossParameter lossParameter;
	lossParameter.set_normalize(false);
	int size_lossParameter = lossParameter.ByteSize();

	caffe_tmp::ConvolutionParameter convolutionParameter;
	convolutionParameter.set_num_output(2);
	convolutionParameter.set_engine(caffe_tmp::ConvolutionParameter::Engine::ConvolutionParameter_Engine_CAFFE);
	int size_convolutionParameter = convolutionParameter.ByteSize();

	caffe_tmp::PoolingParameter poolingParameter;
	poolingParameter.set_pool(caffe_tmp::PoolingParameter::PoolMethod::PoolingParameter_PoolMethod_AVE);
	poolingParameter.set_engine(caffe_tmp::PoolingParameter::Engine::PoolingParameter_Engine_CUDNN);
	int size_poolingParameter = poolingParameter.ByteSize();

#ifdef _MSC_VER
	const std::string data_path = "E:/GitCode/Caffe_Test/test_data/third_party_library/";
#else
	const std::string data_path = "test_data/third_party_library/";
#endif
	std::fstream output;
	std::string file_out = data_path + "caffe.bin";
	output.open(file_out.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
	if (!output.is_open()) {
		std::cout << "write, open file fail" << std::endl;
		return -1;
	}

	if (!blobShape.SerializeToOstream(&output) || !blobProto.SerializeToOstream(&output) || !datum.SerializeToOstream(&output) ||
		!netState.SerializeToOstream(&output) || !lossParameter.SerializeToOstream(&output) ||
		!convolutionParameter.SerializeToOstream(&output) || !poolingParameter.SerializeToOstream(&output)) {
		std::cout << "failed to write" << std::endl;
		return -1;
	}
	output.close();

	// 3. 解析(反序列化)
	std::fstream input;
	std::string file_in = data_path + "caffe.bin";
	input.open(file_in.c_str(), std::ios::in | std::ios::binary);
	if (!input.is_open()) {
		std::cout << "read, open file fail" << std::endl;
		return -1;
	}

	char* buf = new char[1024];
	input.read((char*)buf, size_blobShape);
	caffe_tmp::BlobShape blobShape_;
	blobShape_.ParseFromString((char*)buf);
	assert(blobShape_.ByteSize() == size_blobShape);
	assert(blobShape_.dim_size() == 2);
	assert(blobShape_.dim(0) == 4);
	assert(blobShape_.dim(1) == 8);

	input.read(buf, size_blobProto);
	caffe_tmp::BlobProto blobProto_;
	blobProto_.ParseFromArray(buf, size_blobProto);
	assert(blobProto_.ByteSize() == size_blobProto);
	assert(blobProto_.has_shape() == true);
	blobShape_ = blobProto_.shape();
	assert(blobShape_.dim(0) == 10);
	assert(blobProto_.data_size() == 2);
	assert(blobProto_.data(0) == -1.0);
	assert(blobProto_.diff(0) == 3.0);
	assert(blobProto_.has_num() == true);

	input.read(buf, size_datum);
	caffe_tmp::Datum datum_;
	datum_.ParseFromArray(buf, size_datum);
	assert(datum_.ByteSize() == size_datum);
	assert(datum_.has_channels() == false);
	assert(datum_.float_data_size() == 0);
	assert(datum_.has_data() == true);
	std::string str1 = datum_.data();
	assert(str1.size() == 5);
	std::vector<unsigned char> bytes(str1.begin(), str1.end());
	assert(bytes[4] == 5);

	input.read(buf, size_netState);
	caffe_tmp::NetState netState_;
	netState_.ParseFromArray(buf, size_netState);
	assert(netState_.ByteSize() == size_netState);
	assert(netState_.has_phase() == true);
	caffe_tmp::Phase phase_ = netState_.phase();
	assert(phase_ == 0);
	assert(netState_.has_level() == false);
	assert(netState_.stage_size() == 3);
	assert(netState_.stage(1) == "protobuf");

	input.read(buf, size_lossParameter);
	caffe_tmp::LossParameter lossParameter_;
	lossParameter_.ParseFromArray(buf, size_lossParameter);
	assert(lossParameter_.ByteSize() == size_lossParameter);
	assert(lossParameter_.has_ignore_label() == false);
	assert(lossParameter_.normalize() == false);

	input.read(buf, size_convolutionParameter);
	caffe_tmp::ConvolutionParameter convolutionParameter_;
	convolutionParameter_.ParseFromArray(buf, size_convolutionParameter);
	assert(convolutionParameter_.ByteSize() == size_convolutionParameter);
	assert(convolutionParameter_.has_kernel_size() == false);
	assert(convolutionParameter_.num_output() == 2);
	assert(convolutionParameter_.has_engine() == true);
	caffe_tmp::ConvolutionParameter_Engine engine_ = convolutionParameter_.engine();
	assert(engine_ == caffe_tmp::ConvolutionParameter::Engine::ConvolutionParameter_Engine_CAFFE);

	input.read(buf, size_poolingParameter);
	caffe_tmp::PoolingParameter poolingParameter_;
	poolingParameter_.ParseFromArray(buf, size_poolingParameter);
	assert(poolingParameter_.ByteSize() == size_poolingParameter);
	assert(poolingParameter_.has_pool() == true);
	caffe_tmp::PoolingParameter_PoolMethod poolMethod = poolingParameter_.pool();
	assert(poolMethod == caffe_tmp::PoolingParameter::PoolMethod::PoolingParameter_PoolMethod_AVE);
	caffe_tmp::PoolingParameter_Engine pooling_engine_ = poolingParameter_.engine();
	assert(pooling_engine_ == 2);
	fprintf(stdout, "pooling_engine_: %d\n", pooling_engine_);

	delete[] buf;
	input.close();

	return 0;
}
