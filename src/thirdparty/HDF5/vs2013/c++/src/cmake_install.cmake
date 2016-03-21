# Install script for directory: E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/c++/src

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/install")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "cppheaders")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/c++/src/H5AbstractDs.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/c++/src/H5Alltypes.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/c++/src/H5ArrayType.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/c++/src/H5AtomType.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/c++/src/H5Attribute.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/c++/src/H5Classes.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/c++/src/H5CommonFG.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/c++/src/H5CompType.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/c++/src/H5Cpp.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/c++/src/H5CppDoc.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/c++/src/H5DataSet.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/c++/src/H5DataSpace.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/c++/src/H5DataType.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/c++/src/H5DcreatProp.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/c++/src/H5DxferProp.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/c++/src/H5EnumType.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/c++/src/H5Exception.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/c++/src/H5FaccProp.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/c++/src/H5FcreatProp.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/c++/src/H5File.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/c++/src/H5FloatType.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/c++/src/H5Group.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/c++/src/H5IdComponent.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/c++/src/H5Include.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/c++/src/H5IntType.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/c++/src/H5Library.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/c++/src/H5Location.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/c++/src/H5Object.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/c++/src/H5PredType.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/c++/src/H5PropList.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/c++/src/H5StrType.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/c++/src/H5VarLenType.h"
    )
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "cpplibraries")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/vs2013/bin/Debug/libhdf5_cpp_D.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/vs2013/bin/Release/libhdf5_cpp.lib")
  endif()
endif()

