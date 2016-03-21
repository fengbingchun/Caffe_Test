# CMake generated Testfile for 
# Source directory: E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/hl/c++/examples
# Build directory: E:/GitCode/Caffe_Test/src/thirdparty/HDF5/vs2013/hl/c++/examples
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(HL_CPP_ex_ptExampleFL-clear-objects "D:/ProgramFiles/CMake/bin/cmake.exe" "-E" "remove" "PTcppexampleFL.h5")
elseif("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(HL_CPP_ex_ptExampleFL-clear-objects "D:/ProgramFiles/CMake/bin/cmake.exe" "-E" "remove" "PTcppexampleFL.h5")
else()
  add_test(HL_CPP_ex_ptExampleFL-clear-objects NOT_AVAILABLE)
endif()
if("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(HL_CPP_ex_ptExampleFL "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/vs2013/bin/Debug/ptExampleFL.exe")
  set_tests_properties(HL_CPP_ex_ptExampleFL PROPERTIES  DEPENDS "HL_CPP_ex_ptExampleFL-clear-objects")
elseif("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(HL_CPP_ex_ptExampleFL "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/vs2013/bin/Release/ptExampleFL.exe")
  set_tests_properties(HL_CPP_ex_ptExampleFL PROPERTIES  DEPENDS "HL_CPP_ex_ptExampleFL-clear-objects")
else()
  add_test(HL_CPP_ex_ptExampleFL NOT_AVAILABLE)
endif()
