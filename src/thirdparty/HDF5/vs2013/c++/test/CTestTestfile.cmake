# CMake generated Testfile for 
# Source directory: E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/c++/test
# Build directory: E:/GitCode/Caffe_Test/src/thirdparty/HDF5/vs2013/c++/test
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(CPP_testhdf5-clear-objects "D:/ProgramFiles/CMake/bin/cmake.exe" "-E" "remove" "tattr_basic.h5" "tattr_compound.h5" "tattr_dtype.h5" "tattr_multi.h5" "tattr_scalar.h5" "tfattrs.h5")
elseif("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(CPP_testhdf5-clear-objects "D:/ProgramFiles/CMake/bin/cmake.exe" "-E" "remove" "tattr_basic.h5" "tattr_compound.h5" "tattr_dtype.h5" "tattr_multi.h5" "tattr_scalar.h5" "tfattrs.h5")
else()
  add_test(CPP_testhdf5-clear-objects NOT_AVAILABLE)
endif()
if("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(CPP_testhdf5 "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/vs2013/bin/Debug/cpp_testhdf5.exe")
  set_tests_properties(CPP_testhdf5 PROPERTIES  DEPENDS "CPP_testhdf5-clear-objects")
elseif("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(CPP_testhdf5 "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/vs2013/bin/Release/cpp_testhdf5.exe")
  set_tests_properties(CPP_testhdf5 PROPERTIES  DEPENDS "CPP_testhdf5-clear-objects")
else()
  add_test(CPP_testhdf5 NOT_AVAILABLE)
endif()
