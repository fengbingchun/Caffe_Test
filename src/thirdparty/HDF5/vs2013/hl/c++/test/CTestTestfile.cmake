# CMake generated Testfile for 
# Source directory: E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/hl/c++/test
# Build directory: E:/GitCode/Caffe_Test/src/thirdparty/HDF5/vs2013/hl/c++/test
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(HL_CPP_ptableTest "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/vs2013/bin/Debug/hl_ptableTest.exe")
elseif("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(HL_CPP_ptableTest "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/vs2013/bin/Release/hl_ptableTest.exe")
else()
  add_test(HL_CPP_ptableTest NOT_AVAILABLE)
endif()
