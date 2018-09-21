#! /bin/bash

echo "Warning: install boost: $ sudo apt-get install libboost-all-dev"
echo "Warning: install openblas: $ sudo apt-get install libopenblas-dev"

real_path=$(realpath $0)
dir_name=`dirname "${real_path}"`
echo "real_path: ${real_path}, dir_name: ${dir_name}"

data_dir="test_data"
if [ -d ${dir_name}/${data_dir} ]; then
	rm -rf ${dir_name}/${data_dir}
fi

ln -s ${dir_name}/./../../${data_dir} ${dir_name}

new_dir_name=${dir_name}/build
mkdir -p ${new_dir_name}
cd ${new_dir_name}
echo "pos: ${new_dir_name}"
if [ "$(ls -A ${new_dir_name})" ]; then
	echo "directory is not empty: ${new_dir_name}"
	rm -r *
else
	echo "directory is empty: ${new_dir_name}"
fi

cd -
# build glog
echo "############## start build glog:"
glog_path=${dir_name}/../../src/glog
mkdir -p ${glog_path}/build
cd ${glog_path}/build
cmake ..
make
ln -s ${glog_path}/build/libglog.a ${new_dir_name}
echo "############## finish build glog"

cd -
# build gflags
echo "############## start build gflags:"
gflags_path=${dir_name}/../../src/gflags
mkdir -p ${gflags_path}/build
cd ${gflags_path}/build
cmake ..
make
ln -s ${gflags_path}/build/lib/libgflags.a ${new_dir_name}
echo "############## finish build gflags"

cd -
# build leveldb
echo "############## start build leveldb:"
leveldb_path=${dir_name}/../../src/leveldb
mkdir -p ${leveldb_path}/build
cd ${leveldb_path}/build
cmake ..
make
ln -s ${leveldb_path}/build/libleveldb.a ${new_dir_name}
echo "############## finish build leveldb"

cd -
# build protobuf
echo "############## start build protobuf:"
protobuf_path=${dir_name}/../../src/protobuf
mkdir -p ${protobuf_path}/cmake/build
cd ${protobuf_path}/cmake/build
cmake ..
make
ln -s ${protobuf_path}/cmake/build/libprotoc.a ${new_dir_name}
ln -s ${protobuf_path}/cmake/build/libprotobuf.a ${new_dir_name}
ln -s ${protobuf_path}/cmake/build/protoc ${new_dir_name}
echo "############## finish build protobuf"

cd -
# build hdf5
echo "############## start build hdf5:"
hdf5_path=${dir_name}/../../src/hdf5
mkdir -p ${hdf5_path}/build
cd ${hdf5_path}/build
cmake ..
make
ln -s ${hdf5_path}/build/bin/libhdf5_cpp-static.a ${new_dir_name}
ln -s ${hdf5_path}/build/bin/libhdf5_hl-static.a ${new_dir_name}
ln -s ${hdf5_path}/build/bin/libhdf5_hl_cpp-static.a ${new_dir_name}
ln -s ${hdf5_path}/build/bin/libhdf5-static.a ${new_dir_name}
echo "############## finish build protobuf"

cd -
# build snappy
echo "############## start build snappy:"
snappy_path=${dir_name}/../../src/snappy
cd ${snappy_path}
./autogen.sh
./configure
make
ln -s ${snappy_path}/.libs/libsnappy.a ${new_dir_name}
echo "############## finish build snappy"

cd -
# build lmdb
echo "############## start build lmdb:"
lmdb_path=${dir_name}/../../src/lmdb
cd ${lmdb_path}
make
ln -s ${lmdb_path}/liblmdb.a ${new_dir_name}
echo "############## finish build lmdb"

cd -
# build openblas
# do this is because need to generate config.h file, although it can't normal execution
echo "############## start build openblas:"
openblas_path=${dir_name}/../../src/openblas
mkdir -p ${openblas_path}/build
cd ${openblas_path}/build
cmake ..
make
#ln -s ${openblas_path}/build/libopenblas.a ${new_dir_name}
echo "############## finish build openblas"

rc=$?
if [[ ${rc} != 0 ]]; then
	echo "##### Error: some of thess commands have errors above, please check"
	exit ${rc}
fi

cd -
cd ${new_dir_name}
cmake ..
make

cd -

