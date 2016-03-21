# Install script for directory: E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/tools/lib

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

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "toolsheaders")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/tools/lib/h5trav.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/tools/lib/h5tools.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/tools/lib/h5tools_dump.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/tools/lib/h5tools_utils.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/tools/lib/h5tools_str.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/tools/lib/h5tools_ref.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/tools/lib/h5diff.h"
    )
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "toolslibraries")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/vs2013/bin/Debug/libhdf5_tools_D.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/vs2013/bin/Release/libhdf5_tools.lib")
  endif()
endif()

