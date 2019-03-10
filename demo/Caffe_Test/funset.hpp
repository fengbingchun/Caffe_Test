#ifndef FBC_CAFFE_TEST_FUNSET_HPP_
#define FBC_CAFFE_TEST_FUNSET_HPP_

int lenet_5_mnist_train();
int lenet_5_mnist_test();

int cifar10_train();
int cifar10_convert();
int cifar10_compute_image_mean();
int cifar10_predict();

int mnist_tensorrt_predict();
int mnist_train();
int mnist_predict();
int mnist_convert();

int image_joint();

int get_layer_type_list();
int test_caffe_common();
int test_caffe_util_mkl_alternate();
int test_caffe_util_math_functions();
int test_caffe_syncedmem();
int test_caffe_blob();
int test_caffe_util_io();
int test_caffe_layer_pooling();
int test_caffe_net1();
int test_caffe_net2();
int test_caffe_solver();

#endif // FBC_CAFFE_TEST_FUNSET_HPP_
