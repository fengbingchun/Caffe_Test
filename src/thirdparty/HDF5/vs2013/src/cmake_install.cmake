# Install script for directory: E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src

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

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/hdf5.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5api_adpt.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5public.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5version.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5overflow.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5Apkg.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5Apublic.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5ACpkg.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5ACpublic.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5B2pkg.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5B2public.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5Bpkg.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5Bpublic.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5Dpkg.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5Dpublic.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5Edefin.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5Einit.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5Epkg.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5Epubgen.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5Epublic.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5Eterm.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5Fpkg.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5Fpublic.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5FDcore.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5FDdirect.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5FDfamily.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5FDlog.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5FDmpi.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5FDmpio.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5FDmulti.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5FDpkg.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5FDpublic.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5FDsec2.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5FDstdio.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5FSpkg.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5FSpublic.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5Gpkg.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5Gpublic.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5HFpkg.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5HFpublic.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5HGpkg.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5HGpublic.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5HLpkg.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5HLpublic.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5MPpkg.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5Opkg.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5Opublic.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5Oshared.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5Ppkg.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5Ppublic.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5PLextern.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5PLpublic.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5Rpkg.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5Rpublic.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5Spkg.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5Spublic.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5SMpkg.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5Tpkg.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5Tpublic.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5Zpkg.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5Zpublic.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5Cpkg.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5Cpublic.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5Ipkg.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5Ipublic.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5Lpkg.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5Lpublic.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5MMpublic.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5Rpkg.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5Rpublic.h"
    "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/src/H5FDwindows.h"
    )
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "libraries")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/vs2013/bin/Debug/libhdf5_D.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/vs2013/bin/Release/libhdf5.lib")
  endif()
endif()

