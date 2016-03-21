#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "hdf5" for configuration "Release"
set_property(TARGET hdf5 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(hdf5 PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libhdf5.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS hdf5 )
list(APPEND _IMPORT_CHECK_FILES_FOR_hdf5 "${_IMPORT_PREFIX}/lib/libhdf5.lib" )

# Import target "hdf5_tools" for configuration "Release"
set_property(TARGET hdf5_tools APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(hdf5_tools PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libhdf5_tools.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS hdf5_tools )
list(APPEND _IMPORT_CHECK_FILES_FOR_hdf5_tools "${_IMPORT_PREFIX}/lib/libhdf5_tools.lib" )

# Import target "hdf5_hl" for configuration "Release"
set_property(TARGET hdf5_hl APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(hdf5_hl PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libhdf5_hl.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS hdf5_hl )
list(APPEND _IMPORT_CHECK_FILES_FOR_hdf5_hl "${_IMPORT_PREFIX}/lib/libhdf5_hl.lib" )

# Import target "hdf5_cpp" for configuration "Release"
set_property(TARGET hdf5_cpp APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(hdf5_cpp PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libhdf5_cpp.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS hdf5_cpp )
list(APPEND _IMPORT_CHECK_FILES_FOR_hdf5_cpp "${_IMPORT_PREFIX}/lib/libhdf5_cpp.lib" )

# Import target "hdf5_hl_cpp" for configuration "Release"
set_property(TARGET hdf5_hl_cpp APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(hdf5_hl_cpp PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libhdf5_hl_cpp.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS hdf5_hl_cpp )
list(APPEND _IMPORT_CHECK_FILES_FOR_hdf5_hl_cpp "${_IMPORT_PREFIX}/lib/libhdf5_hl_cpp.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
