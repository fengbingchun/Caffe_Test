# Caffe_Test
**The main role of the project: Caffe's usage**  
**Note: Clone this repository to E:/GitCode/ in windows**
- Caffe's third library usage: Boost、GFlags、GLog、LevelDB、LMDB、ProtoBuf、HDF5、OpenBLAS、Snappy
	- Boost version: 1.58.0
	- ProtoBuf version: 3.2
	- GLog version: mater, commit: da816ea, date: 2017.03.07
	- GFlags version: 2.2.0
	- LevelDB version: 1.18
	- LMDB version: 0.9.19
	- HDF5 version: 1.10.0-patch1
	- Snappy version: 1.1.4
	- OpenBLAS version: 0.2.19
- MNIST convert to LMDB/LevelDB
- MNIST train and predict
- cifar10 convert to LMDB/LevelDB
- compute image mean
- cifar10 train and predict
- Caffe main header files's usage, include:
	- caffe/common.hpp
	- caffe/util/mkl_alternate.hpp
	- caffe/util/math_functions.hpp
	- caffe/syncedmem.hpp
	- caffe/blob.hpp
	- caffe/util/io.hpp
	- caffe/layers/pooling_layer.hpp
	- caffe/net.hpp
	- caffe/solver.hpp
- Classic Networks test code:
	- LeNet-5

**The project support platform:**
- windows7/10 64 bits: It can be directly build with VS2013 in windows7/10 64bits, compile step:
	- from https://github.com/opencv/opencv/releases download opencv2.4.13, unzip to D:\soft\OpenCV2.4.13, add D:\soft\OpenCV2.4.13\opencv\build\x64\vc12\bin to Path
	- from http://www.boost.org/users/history/version_1_58_0.html download boost1.58.0, install to D:\ProgramFiles, add D:\ProgramFiles\local\boost_1_58_0\lib64-msvc-12.0 to Path
	- clone or download Caffe_Test to E:\GitCode, open Caffe.sln, first build dependent libraries, next build libcaffe.
	- if you have already installed and configured cuda8.0 in your PC, then you can build libcaffe_gpu too.
- Linux
	- ThirdPartyLibrary_Test support cmake build(file position: prj/linux_cmake_ThirdPartyLibrary_Test)
	- Caffe_Test support cmake build(file position: prj/linux_cmake_Caffe_Test)
		- build cpu mode: build.sh 0
		- build gpu mode: build.sh 1

**Windows VS Screenshot:**  
![](https://github.com/fengbingchun/Caffe_Test/blob/master/prj/x86_x64_vc12/Screenshot.png)

**Blog:** [fengbingchun](http://blog.csdn.net/fengbingchun/article/category/3185663)
