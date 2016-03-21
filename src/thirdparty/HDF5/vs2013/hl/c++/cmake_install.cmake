# Install script for directory: E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/hl/c++

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

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("E:/GitCode/Caffe_Test/src/thirdparty/HDF5/vs2013/hl/c++/src/cmake_install.cmake")
  include("E:/GitCode/Caffe_Test/src/thirdparty/HDF5/vs2013/hl/c++/examples/cmake_install.cmake")
  include("E:/GitCode/Caffe_Test/src/thirdparty/HDF5/vs2013/hl/c++/test/cmake_install.cmake")

endif()

