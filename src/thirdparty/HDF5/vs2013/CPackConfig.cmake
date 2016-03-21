# This file will be configured to contain variables for CPack. These variables
# should be set in the CMake list file of the project before CPack module is
# included. The list of available CPACK_xxx variables and their associated
# documentation may be obtained using
#  cpack --help-variable-list
#
# Some variables are common to all generators (e.g. CPACK_PACKAGE_NAME)
# and some are specific to a generator
# (e.g. CPACK_NSIS_EXTRA_INSTALL_COMMANDS). The generator specific variables
# usually begin with CPACK_<GENNAME>_xxxx.


SET(CPACK_BINARY_7Z "")
SET(CPACK_BINARY_BUNDLE "")
SET(CPACK_BINARY_CYGWIN "")
SET(CPACK_BINARY_DEB "")
SET(CPACK_BINARY_DRAGNDROP "")
SET(CPACK_BINARY_IFW "")
SET(CPACK_BINARY_NSIS "")
SET(CPACK_BINARY_OSXX11 "")
SET(CPACK_BINARY_PACKAGEMAKER "")
SET(CPACK_BINARY_RPM "")
SET(CPACK_BINARY_STGZ "")
SET(CPACK_BINARY_TBZ2 "")
SET(CPACK_BINARY_TGZ "")
SET(CPACK_BINARY_TXZ "")
SET(CPACK_BINARY_TZ "")
SET(CPACK_BINARY_WIX "")
SET(CPACK_BINARY_ZIP "")
SET(CPACK_CMAKE_GENERATOR "Visual Studio 12 2013 Win64")
SET(CPACK_COMPONENTS_ALL "Unspecified;configinstall;cppheaders;cpplibraries;hdfdocuments;headers;hlcppheaders;hlcpplibraries;hlheaders;hllibraries;hltoolsapplications;libraries;toolsapplications;toolsheaders;toolslibraries")
SET(CPACK_COMPONENT_UNSPECIFIED_HIDDEN "TRUE")
SET(CPACK_COMPONENT_UNSPECIFIED_REQUIRED "TRUE")
SET(CPACK_GENERATOR "ZIP")
SET(CPACK_INSTALL_CMAKE_PROJECTS "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/vs2013;HDF5;ALL;/")
SET(CPACK_INSTALL_PREFIX "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/install")
SET(CPACK_MODULE_PATH "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/config/cmake;E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/config/cmake_ext_mod")
SET(CPACK_NSIS_CONTACT "help@hdfgroup.org")
SET(CPACK_NSIS_DISPLAY_NAME "HDF5 1.8.15-patch1")
SET(CPACK_NSIS_DISPLAY_NAME_SET "TRUE")
SET(CPACK_NSIS_INSTALLER_ICON_CODE "")
SET(CPACK_NSIS_INSTALLER_MUI_ICON_CODE "")
SET(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
SET(CPACK_NSIS_MODIFY_PATH "ON")
SET(CPACK_NSIS_MUI_ICON "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/config/cmake_ext_mod\\hdf.ico")
SET(CPACK_NSIS_MUI_UNIICON "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/config/cmake_ext_mod\\hdf.ico")
SET(CPACK_NSIS_PACKAGE_NAME "HDF5 1.8.15-patch1")
SET(CPACK_OUTPUT_CONFIG_FILE "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/vs2013/CPackConfig.cmake")
SET(CPACK_PACKAGE_DEFAULT_LOCATION "/")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/release_docs/RELEASE.txt")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "HDF5 built using CMake")
SET(CPACK_PACKAGE_FILE_NAME "HDF5-1.8.15-patch1-win64")
SET(CPACK_PACKAGE_ICON "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/config/cmake_ext_mod\\hdf.bmp")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "HDF_Group\\HDF5\\1.8.15-patch1")
SET(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "HDF5-1.8.15-patch1 (Win64)")
SET(CPACK_PACKAGE_NAME "HDF5")
SET(CPACK_PACKAGE_RELOCATABLE "true")
SET(CPACK_PACKAGE_VENDOR "HDF_Group")
SET(CPACK_PACKAGE_VERSION "1.8.15-patch1")
SET(CPACK_PACKAGE_VERSION_MAJOR "1.8")
SET(CPACK_PACKAGE_VERSION_MINOR "15")
SET(CPACK_PACKAGE_VERSION_PATCH "")
SET(CPACK_RESOURCE_FILE_LICENSE "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/vs2013/COPYING.txt")
SET(CPACK_RESOURCE_FILE_README "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/release_docs/RELEASE.txt")
SET(CPACK_RESOURCE_FILE_WELCOME "D:/ProgramFiles/CMake/share/cmake-3.3/Templates/CPack.GenericWelcome.txt")
SET(CPACK_SET_DESTDIR "OFF")
SET(CPACK_SOURCE_7Z "ON")
SET(CPACK_SOURCE_CYGWIN "")
SET(CPACK_SOURCE_GENERATOR "7Z;ZIP")
SET(CPACK_SOURCE_OUTPUT_CONFIG_FILE "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/vs2013/CPackSourceConfig.cmake")
SET(CPACK_SOURCE_TBZ2 "")
SET(CPACK_SOURCE_TGZ "")
SET(CPACK_SOURCE_TXZ "")
SET(CPACK_SOURCE_TZ "")
SET(CPACK_SOURCE_ZIP "ON")
SET(CPACK_SYSTEM_NAME "win64")
SET(CPACK_TOPLEVEL_TAG "win64")
SET(CPACK_WIX_PRODUCT_ICON "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/hdf5-1.8.15-patch1/config/cmake_ext_mod\\hdf.ico")
SET(CPACK_WIX_PROPERTY_ARPCOMMENTS "HDF5 (Hierarchical Data Format 5) Software Library and Utilities")
SET(CPACK_WIX_PROPERTY_ARPHELPLINK "help@hdfgroup.org")
SET(CPACK_WIX_PROPERTY_ARPURLINFOABOUT "http://www.hdfgroup.org")
SET(CPACK_WIX_ROOT "")
SET(CPACK_WIX_SIZEOF_VOID_P "8")
SET(CPACK_WIX_UNINSTALL "1")

if(NOT CPACK_PROPERTIES_FILE)
  set(CPACK_PROPERTIES_FILE "E:/GitCode/Caffe_Test/src/thirdparty/HDF5/vs2013/CPackProperties.cmake")
endif()

if(EXISTS ${CPACK_PROPERTIES_FILE})
  include(${CPACK_PROPERTIES_FILE})
endif()

# Configuration for installation type "Full"
list(APPEND CPACK_ALL_INSTALL_TYPES Full)
set(CPACK_INSTALL_TYPE_FULL_DISPLAY_NAME "Everything")

# Configuration for installation type "Developer"
list(APPEND CPACK_ALL_INSTALL_TYPES Developer)

# Configuration for component group "Runtime"

# Configuration for component group "Documents"
set(CPACK_COMPONENT_GROUP_DOCUMENTS_DESCRIPTION "Release notes for developing HDF5 applications")
set(CPACK_COMPONENT_GROUP_DOCUMENTS_EXPANDED TRUE)

# Configuration for component group "Development"
set(CPACK_COMPONENT_GROUP_DEVELOPMENT_DESCRIPTION "All of the tools you'll need to develop HDF5 applications")
set(CPACK_COMPONENT_GROUP_DEVELOPMENT_EXPANDED TRUE)

# Configuration for component group "Applications"
set(CPACK_COMPONENT_GROUP_APPLICATIONS_DESCRIPTION "Tools for HDF5 files")
set(CPACK_COMPONENT_GROUP_APPLICATIONS_EXPANDED TRUE)

# Configuration for component "libraries"

SET(CPACK_COMPONENTS_ALL Unspecified configinstall cppheaders cpplibraries hdfdocuments headers hlcppheaders hlcpplibraries hlheaders hllibraries hltoolsapplications libraries toolsapplications toolsheaders toolslibraries)
set(CPACK_COMPONENT_LIBRARIES_DISPLAY_NAME "HDF5 Libraries")
set(CPACK_COMPONENT_LIBRARIES_GROUP Runtime)
set(CPACK_COMPONENT_LIBRARIES_INSTALL_TYPES Full Developer User)

# Configuration for component "headers"

SET(CPACK_COMPONENTS_ALL Unspecified configinstall cppheaders cpplibraries hdfdocuments headers hlcppheaders hlcpplibraries hlheaders hllibraries hltoolsapplications libraries toolsapplications toolsheaders toolslibraries)
set(CPACK_COMPONENT_HEADERS_DISPLAY_NAME "HDF5 Headers")
set(CPACK_COMPONENT_HEADERS_GROUP Development)
set(CPACK_COMPONENT_HEADERS_DEPENDS libraries)
set(CPACK_COMPONENT_HEADERS_INSTALL_TYPES Full Developer)

# Configuration for component "hdfdocuments"

SET(CPACK_COMPONENTS_ALL Unspecified configinstall cppheaders cpplibraries hdfdocuments headers hlcppheaders hlcpplibraries hlheaders hllibraries hltoolsapplications libraries toolsapplications toolsheaders toolslibraries)
set(CPACK_COMPONENT_HDFDOCUMENTS_DISPLAY_NAME "HDF5 Documents")
set(CPACK_COMPONENT_HDFDOCUMENTS_GROUP Documents)
set(CPACK_COMPONENT_HDFDOCUMENTS_INSTALL_TYPES Full Developer)

# Configuration for component "configinstall"

SET(CPACK_COMPONENTS_ALL Unspecified configinstall cppheaders cpplibraries hdfdocuments headers hlcppheaders hlcpplibraries hlheaders hllibraries hltoolsapplications libraries toolsapplications toolsheaders toolslibraries)
set(CPACK_COMPONENT_CONFIGINSTALL_DISPLAY_NAME "HDF5 CMake files")
set(CPACK_COMPONENT_CONFIGINSTALL_GROUP Development)
set(CPACK_COMPONENT_CONFIGINSTALL_DEPENDS libraries)
set(CPACK_COMPONENT_CONFIGINSTALL_INSTALL_TYPES Full Developer User)

# Configuration for component "cpplibraries"

SET(CPACK_COMPONENTS_ALL Unspecified configinstall cppheaders cpplibraries hdfdocuments headers hlcppheaders hlcpplibraries hlheaders hllibraries hltoolsapplications libraries toolsapplications toolsheaders toolslibraries)
set(CPACK_COMPONENT_CPPLIBRARIES_DISPLAY_NAME "HDF5 C++ Libraries")
set(CPACK_COMPONENT_CPPLIBRARIES_GROUP Runtime)
set(CPACK_COMPONENT_CPPLIBRARIES_DEPENDS libraries)
set(CPACK_COMPONENT_CPPLIBRARIES_INSTALL_TYPES Full Developer User)

# Configuration for component "cppheaders"

SET(CPACK_COMPONENTS_ALL Unspecified configinstall cppheaders cpplibraries hdfdocuments headers hlcppheaders hlcpplibraries hlheaders hllibraries hltoolsapplications libraries toolsapplications toolsheaders toolslibraries)
set(CPACK_COMPONENT_CPPHEADERS_DISPLAY_NAME "HDF5 C++ Headers")
set(CPACK_COMPONENT_CPPHEADERS_GROUP Development)
set(CPACK_COMPONENT_CPPHEADERS_DEPENDS cpplibraries)
set(CPACK_COMPONENT_CPPHEADERS_INSTALL_TYPES Full Developer)

# Configuration for component "toolsapplications"

SET(CPACK_COMPONENTS_ALL Unspecified configinstall cppheaders cpplibraries hdfdocuments headers hlcppheaders hlcpplibraries hlheaders hllibraries hltoolsapplications libraries toolsapplications toolsheaders toolslibraries)
set(CPACK_COMPONENT_TOOLSAPPLICATIONS_DISPLAY_NAME "HDF5 Tools Applications")
set(CPACK_COMPONENT_TOOLSAPPLICATIONS_GROUP Applications)
set(CPACK_COMPONENT_TOOLSAPPLICATIONS_DEPENDS toolslibraries)
set(CPACK_COMPONENT_TOOLSAPPLICATIONS_INSTALL_TYPES Full Developer User)

# Configuration for component "toolslibraries"

SET(CPACK_COMPONENTS_ALL Unspecified configinstall cppheaders cpplibraries hdfdocuments headers hlcppheaders hlcpplibraries hlheaders hllibraries hltoolsapplications libraries toolsapplications toolsheaders toolslibraries)
set(CPACK_COMPONENT_TOOLSLIBRARIES_DISPLAY_NAME "HDF5 Tools Libraries")
set(CPACK_COMPONENT_TOOLSLIBRARIES_GROUP Runtime)
set(CPACK_COMPONENT_TOOLSLIBRARIES_DEPENDS libraries)
set(CPACK_COMPONENT_TOOLSLIBRARIES_INSTALL_TYPES Full Developer User)

# Configuration for component "toolsheaders"

SET(CPACK_COMPONENTS_ALL Unspecified configinstall cppheaders cpplibraries hdfdocuments headers hlcppheaders hlcpplibraries hlheaders hllibraries hltoolsapplications libraries toolsapplications toolsheaders toolslibraries)
set(CPACK_COMPONENT_TOOLSHEADERS_DISPLAY_NAME "HDF5 Tools Headers")
set(CPACK_COMPONENT_TOOLSHEADERS_GROUP Development)
set(CPACK_COMPONENT_TOOLSHEADERS_DEPENDS toolslibraries)
set(CPACK_COMPONENT_TOOLSHEADERS_INSTALL_TYPES Full Developer)

# Configuration for component "hllibraries"

SET(CPACK_COMPONENTS_ALL Unspecified configinstall cppheaders cpplibraries hdfdocuments headers hlcppheaders hlcpplibraries hlheaders hllibraries hltoolsapplications libraries toolsapplications toolsheaders toolslibraries)
set(CPACK_COMPONENT_HLLIBRARIES_DISPLAY_NAME "HDF5 HL Libraries")
set(CPACK_COMPONENT_HLLIBRARIES_GROUP Runtime)
set(CPACK_COMPONENT_HLLIBRARIES_DEPENDS libraries)
set(CPACK_COMPONENT_HLLIBRARIES_INSTALL_TYPES Full Developer User)

# Configuration for component "hlheaders"

SET(CPACK_COMPONENTS_ALL Unspecified configinstall cppheaders cpplibraries hdfdocuments headers hlcppheaders hlcpplibraries hlheaders hllibraries hltoolsapplications libraries toolsapplications toolsheaders toolslibraries)
set(CPACK_COMPONENT_HLHEADERS_DISPLAY_NAME "HDF5 HL Headers")
set(CPACK_COMPONENT_HLHEADERS_GROUP Development)
set(CPACK_COMPONENT_HLHEADERS_DEPENDS hllibraries)
set(CPACK_COMPONENT_HLHEADERS_INSTALL_TYPES Full Developer)

# Configuration for component "hltoolsapplications"

SET(CPACK_COMPONENTS_ALL Unspecified configinstall cppheaders cpplibraries hdfdocuments headers hlcppheaders hlcpplibraries hlheaders hllibraries hltoolsapplications libraries toolsapplications toolsheaders toolslibraries)
set(CPACK_COMPONENT_HLTOOLSAPPLICATIONS_DISPLAY_NAME "HDF5 HL Tools Applications")
set(CPACK_COMPONENT_HLTOOLSAPPLICATIONS_GROUP Applications)
set(CPACK_COMPONENT_HLTOOLSAPPLICATIONS_DEPENDS hllibraries)
set(CPACK_COMPONENT_HLTOOLSAPPLICATIONS_INSTALL_TYPES Full Developer User)

# Configuration for component "hlcpplibraries"

SET(CPACK_COMPONENTS_ALL Unspecified configinstall cppheaders cpplibraries hdfdocuments headers hlcppheaders hlcpplibraries hlheaders hllibraries hltoolsapplications libraries toolsapplications toolsheaders toolslibraries)
set(CPACK_COMPONENT_HLCPPLIBRARIES_DISPLAY_NAME "HDF5 HL C++ Libraries")
set(CPACK_COMPONENT_HLCPPLIBRARIES_GROUP Runtime)
set(CPACK_COMPONENT_HLCPPLIBRARIES_DEPENDS hllibraries)
set(CPACK_COMPONENT_HLCPPLIBRARIES_INSTALL_TYPES Full Developer User)

# Configuration for component "hlcppheaders"

SET(CPACK_COMPONENTS_ALL Unspecified configinstall cppheaders cpplibraries hdfdocuments headers hlcppheaders hlcpplibraries hlheaders hllibraries hltoolsapplications libraries toolsapplications toolsheaders toolslibraries)
set(CPACK_COMPONENT_HLCPPHEADERS_DISPLAY_NAME "HDF5 HL C++ Headers")
set(CPACK_COMPONENT_HLCPPHEADERS_GROUP Development)
set(CPACK_COMPONENT_HLCPPHEADERS_DEPENDS hlcpplibraries)
set(CPACK_COMPONENT_HLCPPHEADERS_INSTALL_TYPES Full Developer)
