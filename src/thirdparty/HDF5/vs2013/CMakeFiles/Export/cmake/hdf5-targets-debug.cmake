#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "hdf5" for configuration "Debug"
set_property(TARGET hdf5 APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(hdf5 PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libhdf5_D.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS hdf5 )
list(APPEND _IMPORT_CHECK_FILES_FOR_hdf5 "${_IMPORT_PREFIX}/lib/libhdf5_D.lib" )

# Import target "hdf5_tools" for configuration "Debug"
set_property(TARGET hdf5_tools APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(hdf5_tools PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libhdf5_tools_D.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS hdf5_tools )
list(APPEND _IMPORT_CHECK_FILES_FOR_hdf5_tools "${_IMPORT_PREFIX}/lib/libhdf5_tools_D.lib" )

# Import target "hdf5_hl" for configuration "Debug"
set_property(TARGET hdf5_hl APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(hdf5_hl PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libhdf5_hl_D.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS hdf5_hl )
list(APPEND _IMPORT_CHECK_FILES_FOR_hdf5_hl "${_IMPORT_PREFIX}/lib/libhdf5_hl_D.lib" )

# Import target "hdf5_cpp" for configuration "Debug"
set_property(TARGET hdf5_cpp APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(hdf5_cpp PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libhdf5_cpp_D.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS hdf5_cpp )
list(APPEND _IMPORT_CHECK_FILES_FOR_hdf5_cpp "${_IMPORT_PREFIX}/lib/libhdf5_cpp_D.lib" )

# Import target "hdf5_hl_cpp" for configuration "Debug"
set_property(TARGET hdf5_hl_cpp APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(hdf5_hl_cpp PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libhdf5_hl_cpp_D.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS hdf5_hl_cpp )
list(APPEND _IMPORT_CHECK_FILES_FOR_hdf5_hl_cpp "${_IMPORT_PREFIX}/lib/libhdf5_hl_cpp_D.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
