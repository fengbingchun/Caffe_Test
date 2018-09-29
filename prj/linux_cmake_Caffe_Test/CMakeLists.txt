PROJECT(Caffe_Test)
CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

# support C++11
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
# support C++14, when gcc version > 5.1, use -std=c++14 instead of c++1y
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")

IF(NOT CMAKE_BUILD_TYPE)
	SET(CMAKE_BUILD_TYPE "Release")
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -Wall -O2")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wall -O2")
ELSE()
	SET(CMAKE_BUILD_TYPE "Debug")
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -fPIC -Wall -O2")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fPIC -Wall -O2")
ENDIF()
MESSAGE(STATUS "cmake build type: ${CMAKE_BUILD_TYPE}")

IF(BUILD_GPU)
	MESSAGE(STATUS "########## BUILD GPU ##########")
ELSE()
	MESSAGE(STATUS "########## BUILD CPU ##########")
	ADD_DEFINITIONS(-DCPU_ONLY)
ENDIF()

ADD_DEFINITIONS(-DUSE_LMDB)
ADD_DEFINITIONS(-DUSE_LEVELDB)
ADD_DEFINITIONS(-DUSE_OPENCV)
ADD_DEFINITIONS(-DCAFFE_BUILDING_STATIC_LIB)

MESSAGE(STATUS "cmake current source dir: ${CMAKE_CURRENT_SOURCE_DIR}")
SET(PATH_TEST_FILES ${CMAKE_CURRENT_SOURCE_DIR}/./../../demo/Caffe_Test)
SET(PATH_SRC_GFLAGS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/./../../src/gflags)
SET(PATH_SRC_GLOG_FILES ${CMAKE_CURRENT_SOURCE_DIR}/./../../src/glog)
SET(PATH_SRC_HDF5_FILES ${CMAKE_CURRENT_SOURCE_DIR}/./../../src/hdf5)
SET(PATH_SRC_LEVELDB_FILES ${CMAKE_CURRENT_SOURCE_DIR}/./../../src/leveldb)
SET(PATH_SRC_LMDB_FILES ${CMAKE_CURRENT_SOURCE_DIR}/./../../src/lmdb)
SET(PATH_SRC_PROTOBUF_FILES ${CMAKE_CURRENT_SOURCE_DIR}/./../../src/protobuf)
SET(PATH_SRC_SNAPPY_FILES ${CMAKE_CURRENT_SOURCE_DIR}/./../../src/snappy)
SET(PATH_SRC_OPENBLAS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/./../../src/openblas)
SET(PATH_SRC_CAFFE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/./../../src/caffe)
MESSAGE(STATUS "path test files: ${PATH_TEST_FILES}")

SET(PATH_OPENCV /opt/opencv3.4.2)
IF(EXISTS ${PATH_OPENCV})
	MESSAGE(STATUS "Found OpenCV: ${PATH_OPENCV}")
ELSE()
	MESSAGE(FATAL_ERROR "Can not find OpenCV in ${PATH_OPENCV}")
ENDIF()

# head file search path
INCLUDE_DIRECTORIES(
	${PATH_TEST_FILES}
	${PATH_OPENCV}/include
	${PATH_SRC_GFLAGS_FILES}/build/include
	${PATH_SRC_GLOG_FILES}/build
	${PATH_SRC_HDF5_FILES}/src
	${PATH_SRC_LEVELDB_FILES}/include
	#${PATH_SRC_LMDB_FILES} # if include this directory, will have many boost errors, so make system lmdb.h file 
	${PATH_SRC_PROTOBUF_FILES}/src
	${PATH_SRC_SNAPPY_FILES}
	${PATH_SRC_OPENBLAS_FILES}
	${PATH_SRC_CAFFE_FILES}/include
	${PATH_SRC_CAFFE_FILES}/include/caffe/proto
)

# build caffe
SET(CAFFE_SRC_LIST )
LIST(APPEND CAFFE_SRC_LIST
	${PATH_SRC_CAFFE_FILES}/src/caffe/blob.cpp
	${PATH_SRC_CAFFE_FILES}/src/caffe/common.cpp
	${PATH_SRC_CAFFE_FILES}/src/caffe/data_transformer.cpp
	${PATH_SRC_CAFFE_FILES}/src/caffe/internal_thread.cpp
	${PATH_SRC_CAFFE_FILES}/src/caffe/layer.cpp
	${PATH_SRC_CAFFE_FILES}/src/caffe/layer_factory.cpp
	${PATH_SRC_CAFFE_FILES}/src/caffe/net.cpp
	${PATH_SRC_CAFFE_FILES}/src/caffe/parallel.cpp
	${PATH_SRC_CAFFE_FILES}/src/caffe/solver.cpp
	${PATH_SRC_CAFFE_FILES}/src/caffe/solver_factory.cpp
	${PATH_SRC_CAFFE_FILES}/src/caffe/syncedmem.cpp
)

FILE(GLOB_RECURSE SRC_CAFFE_LIST_CPU_LAYERS ${PATH_SRC_CAFFE_FILES}/src/caffe/layers/*.cpp)
FILE(GLOB_RECURSE SRC_CAFFE_LIST_CPU_PROTO ${PATH_SRC_CAFFE_FILES}/src/caffe/proto/*.cc)
FILE(GLOB_RECURSE SRC_CAFFE_LIST_CPU_SOLVERS ${PATH_SRC_CAFFE_FILES}/src/caffe/solvers/*.cpp)
FILE(GLOB_RECURSE SRC_CAFFE_LIST_CPU_UTIL ${PATH_SRC_CAFFE_FILES}/src/caffe/util/*.cpp)

FILE(GLOB_RECURSE SRC_CAFFE_LIST_GPU_LAYERS ${PATH_SRC_CAFFE_FILES}/src/caffe/layers/*.cu)
FILE(GLOB_RECURSE SRC_CAFFE_LIST_GPU_SOLVERS ${PATH_SRC_CAFFE_FILES}/src/caffe/solvers/*.cu)
FILE(GLOB_RECURSE SRC_CAFFE_LIST_GPU_UTIL ${PATH_SRC_CAFFE_FILES}/src/caffe/util/*.cu)

LIST(APPEND CAFFE_SRC_LIST
	${SRC_CAFFE_LIST_CPU_LAYERS}
	${SRC_CAFFE_LIST_CPU_PROTO}
	${SRC_CAFFE_LIST_CPU_SOLVERS}
	${SRC_CAFFE_LIST_CPU_UTIL}
)

IF(BUILD_GPU)
	LIST(APPEND CAFFE_SRC_LIST
		${SRC_CAFFE_LIST_GPU_LAYERS}
		${SRC_CAFFE_LIST_GPU_SOLVERS}
		${SRC_CAFFE_LIST_GPU_UTIL}
	)
ENDIF()

IF(BUILD_GPU)
	FIND_PACKAGE(CUDA)
	INCLUDE_DIRECTORIES(/usr/local/cuda-8.0/include)
	LIST(APPEND CUDA_NVCC_FLAGS -std=c++11;-O2)
	LIST(APPEND CUDA_NVCC_FLAGS -Xcompiler;-fPIC)
	LIST(APPEND CUDA_NVCC_FLAGS -gencode arch=compute_50,code=sm_50)
	LIST(APPEND CUDA_NVCC_FLAGS -gencode arch=compute_60,code=sm_60)
	SET(CUDA_PROPAGATE_HOST_FLAGS OFF)

	CUDA_ADD_LIBRARY(caffe SHARED ${CAFFE_SRC_LIST})
ELSE()
	ADD_LIBRARY(caffe SHARED ${CAFFE_SRC_LIST})
ENDIF()

# find opencv library
FIND_LIBRARY(opencv_core NAMES opencv_core PATHS ${PATH_OPENCV}/lib NO_DEFAULT_PATH)
FIND_LIBRARY(opencv_imgproc NAMES opencv_imgproc PATHS ${PATH_OPENCV}/lib NO_DEFAULT_PATH)
FIND_LIBRARY(opencv_highgui NAMES opencv_highgui PATHS ${PATH_OPENCV}/lib NO_DEFAULT_PATH)
FIND_LIBRARY(opencv_imgcodecs NAMES opencv_imgcodecs PATHS ${PATH_OPENCV}/lib NO_DEFAULT_PATH)
FIND_LIBRARY(opencv_video NAMES opencv_video PATHS ${PATH_OPENCV}/lib NO_DEFAULT_PATH)
FIND_LIBRARY(opencv_videoio NAMES opencv_videoio PATHS ${PATH_OPENCV}/lib NO_DEFAULT_PATH)
FIND_LIBRARY(opencv_objdetect NAMES opencv_objdetect PATHS ${PATH_OPENCV}/lib NO_DEFAULT_PATH)
FIND_LIBRARY(opencv_ml NAMES opencv_ml PATHS ${PATH_OPENCV}/lib NO_DEFAULT_PATH)
MESSAGE(STATUS "opencv libraries: ${opencv_core} ${opencv_imgproc} ${opencv_highgui} ${opencv_imgcodecs} ${opencv_video}" ${opencv_videoio} ${opencv_objdetect} ${opencv_ml})

# find dep library
SET(DEP_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build CACHE PATH "dep library path")
MESSAGE(STATUS "dep library dir: ${DEP_LIB_DIR}")
IF(BUILD_GPU)
	LINK_DIRECTORIES(${DEP_LIB_DIR} /usr/local/cuda-8.0/lib64)
ELSE()
	LINK_DIRECTORIES(${DEP_LIB_DIR})
ENDIF()

# find boost
#FIND_PACKAGE(Boost REQUIRED)
#INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

# find openblas
#FIND_PACKAGE(OpenBLAS REQUIRED) # no FindOpenBLAS.cmake file
#INCLUDE_DIRECTORIES(${OpenBLAS_INCLUDE_DIRS})

# recursive query match files :*.cpp
FILE(GLOB_RECURSE TEST_CPP_LIST ${PATH_TEST_FILES}/*.cpp)
FILE(GLOB_RECURSE TEST_CC_LIST ${PATH_TEST_FILES}/*.cc)
#MESSAGE(STATUS "test cpp list: ${TEST_CPP_LIST} ${TEST_C_LIST}")

SET(DEP_LIBS )
LIST(APPEND DEP_LIBS
	caffe
	${DEP_LIB_DIR}/libgflags.a
	${DEP_LIB_DIR}/libglog.a
	${DEP_LIB_DIR}/libhdf5-static.a
	${DEP_LIB_DIR}/libhdf5_cpp-static.a
	${DEP_LIB_DIR}/libhdf5_hl-static.a
	${DEP_LIB_DIR}/libhdf5_hl_cpp-static.a	
	${DEP_LIB_DIR}/libhdf5-static.a
	${DEP_LIB_DIR}/libleveldb.a
	${DEP_LIB_DIR}/liblmdb.a
	${DEP_LIB_DIR}/libprotobuf.a
	${DEP_LIB_DIR}/libprotoc.a
	${DEP_LIB_DIR}/libsnappy.a
	${opencv_ml}
	${opencv_core}
	${opencv_imgproc}
	${opencv_highgui}
	${opencv_imgcodecs}
	${opencv_video}
	${opencv_videoio}
	${opencv_objdetect}
	boost_system
	boost_thread
	boost_filesystem	
	openblas
	dl # hdf5: undefined reference to dlopen
	pthread
)

IF(BUILD_GPU)
	LIST(APPEND DEP_LIBS
		cublas
		curand
	)
ENDIF()

# build executable program
ADD_EXECUTABLE(Caffe_Test ${TEST_CPP_LIST} ${TEST_CC_LIST})
# add dependent library: static and dynamic
TARGET_LINK_LIBRARIES(Caffe_Test ${DEP_LIBS})

