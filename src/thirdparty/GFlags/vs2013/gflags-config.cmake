## gflags CMake configuration file

# library version information
set (gflags_VERSION_STRING "2.2.0")
set (gflags_VERSION_MAJOR  2)
set (gflags_VERSION_MINOR  2)
set (gflags_VERSION_PATCH  0)

# import targets
include ("${CMAKE_CURRENT_LIST_DIR}/gflags-export.cmake")

# installation prefix
get_filename_component (CMAKE_CURRENT_LIST_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
get_filename_component (_INSTALL_PREFIX "${CMAKE_CURRENT_LIST_DIR}/." ABSOLUTE)

# include directory
#
# Newer versions of CMake set the INTERFACE_INCLUDE_DIRECTORIES property
# of the imported targets. It is hence not necessary to add this path
# manually to the include search path for targets which link to gflags.
set (gflags_INCLUDE_DIR "${_INSTALL_PREFIX}/Include")

# default settings
if (NOT DEFINED gflags_SHARED)
  if (TARGET gflags-static OR TARGET gflags_nothreads-static)
    set (gflags_SHARED FALSE)
  else ()
    set (gflags_SHARED TRUE)
  endif ()
endif ()
if (NOT DEFINED gflags_NOTHREADS)
  if (TARGET gflags-static OR TARGET gflags-shared)
    set (gflags_NOTHREADS FALSE)
  else ()
    set (gflags_NOTHREADS TRUE)
  endif ()
endif ()

# choose imported library target
if (NOT gflags_TARGET)
  if (gflags_SHARED)
    if (gflags_NOTHREADS)
      set (gflags_TARGET gflags_nothreads-shared)
    else ()
      set (gflags_TARGET gflags-shared)
    endif ()
  else ()
    if (gflags_NOTHREADS)
      set (gflags_TARGET gflags_nothreads-static)
    else ()
      set (gflags_TARGET gflags-static)
    endif ()
  endif ()
endif ()
if (NOT TARGET ${gflags_TARGET})
  message (FATAL_ERROR "Your gflags installation does not contain a ${gflags_TARGET} library target!"
                       " Try a different combination of gflags_SHARED and gflags_NOTHREADS.")
endif ()

# add more convenient "gflags" import target
if (NOT TARGET gflags)
  if (gflags_SHARED)
    add_library (gflags SHARED IMPORTED)
  else ()
    add_library (gflags STATIC IMPORTED)
  endif ()
  # INTERFACE_INCLUDE_DIRECTORIES
  get_target_property (_gflags_INCLUDES ${gflags_TARGET} INTERFACE_INCLUDE_DIRECTORIES)
  if (_gflags_INCLUDES)
    set_target_properties(gflags PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${_gflags_INCLUDES}"
    )
  endif ()
  unset (_gflags_INCLUDES)
  # set configuration specific properties
  get_target_property (_gflags_CONFIGURATIONS ${gflags_TARGET} IMPORTED_CONFIGURATIONS)
  set_target_properties (gflags PROPERTIES IMPORTED_CONFIGURATIONS "${_gflags_CONFIGURATIONS}")
  foreach (_gflags_CONFIG IN LISTS _gflags_CONFIGURATIONS)
    # IMPORTED_LOCATION_<config>
    get_target_property (_gflags_LOCATION ${gflags_TARGET} IMPORTED_LOCATION_${_gflags_CONFIG})
    if (_gflags_LOCATION)
      set_target_properties(gflags PROPERTIES
        IMPORTED_LOCATION_${_gflags_CONFIG} "${_gflags_LOCATION}"
      )
    endif ()
    unset (_gflags_LOCATION)
    # IMPORTED_LINK_INTERFACE_LANGUAGES_<config> (static)
    get_target_property (_gflags_LANGUAGES ${gflags_TARGET} IMPORTED_LINK_INTERFACE_LANGUAGES_${_gflags_CONFIG})
    if (_gflags_LANGUAGES)
      set_target_properties(gflags PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES_${_gflags_CONFIG} "${_gflags_LANGUAGES}"
      )
    endif ()
    unset (_gflags_LANGUAGES)
    # IMPORTED_SONAME_<config> (shared)
    get_target_property (_gflags_SONAME ${gflags_TARGET} IMPORTED_SONAME_${_gflags_CONFIG})
    if (_gflags_SONAME)
      set_target_properties(gflags PROPERTIES
        IMPORTED_SONAME_${_gflags_CONFIG} "${_gflags_SONAME}"
      )
    endif ()
    unset (_gflags_SONAME)
  endforeach ()
  unset (_gflags_CONFIGURATIONS)
endif ()

# alias for default import target to be compatible with older CMake package configurations
set (gflags_LIBRARIES "${gflags_TARGET}")

# unset private variables
unset (_INSTALL_PREFIX)
