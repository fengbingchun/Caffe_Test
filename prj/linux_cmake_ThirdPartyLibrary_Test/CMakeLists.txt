PROJECT(ThirdPartyLibrary_Test)
CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

# support C++11
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
# support C++14, when gcc version > 5.1, use -std=c++14 instead of c++1y
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")

IF(NOT CMAKE_BUILD_TYPE)
	SET(CMAKE_BUILD_TYPE "Release")
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -O2")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O2")
ELSE()
	SET(CMAKE_BUILD_TYPE "Debug")
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -O2")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -O2")
ENDIF()
MESSAGE(STATUS "cmake build type: ${CMAKE_BUILD_TYPE}")

MESSAGE(STATUS "cmake current source dir: ${CMAKE_CURRENT_SOURCE_DIR}")
SET(PATH_TEST_FILES ${CMAKE_CURRENT_SOURCE_DIR}/./../../demo/ThirdPartyLibrary_Test)
SET(PATH_SRC_GFLAGS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/./../../src/gflags)
SET(PATH_SRC_GLOG_FILES ${CMAKE_CURRENT_SOURCE_DIR}/./../../src/glog)
SET(PATH_SRC_HDF5_FILES ${CMAKE_CURRENT_SOURCE_DIR}/./../../src/hdf5)
SET(PATH_SRC_LEVELDB_FILES ${CMAKE_CURRENT_SOURCE_DIR}/./../../src/leveldb)
SET(PATH_SRC_LMDB_FILES ${CMAKE_CURRENT_SOURCE_DIR}/./../../src/lmdb)
SET(PATH_SRC_PROTOBUF_FILES ${CMAKE_CURRENT_SOURCE_DIR}/./../../src/protobuf)
SET(PATH_SRC_SNAPPY_FILES ${CMAKE_CURRENT_SOURCE_DIR}/./../../src/snappy)
SET(PATH_SRC_OPENBLAS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/./../../src/openblas)
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
)

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
LINK_DIRECTORIES(${DEP_LIB_DIR})

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

# build executable program
ADD_EXECUTABLE(ThirdPartyLibrary_Test ${TEST_CPP_LIST} ${TEST_CC_LIST})
# add dependent library: static and dynamic
TARGET_LINK_LIBRARIES(ThirdPartyLibrary_Test
	${DEP_LIB_DIR}/libgflags.a
	${DEP_LIB_DIR}/libglog.a
	${DEP_LIB_DIR}/libhdf5-static.a
	${DEP_LIB_DIR}/libhdf5_cpp-static.a
	${DEP_LIB_DIR}/libhdf5_hl-static.a
	${DEP_LIB_DIR}/libhdf5_hl_cpp-static.a	
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

