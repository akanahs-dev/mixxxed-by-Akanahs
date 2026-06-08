#[=======================================================================[.rst:
FindEssentia
-----------

Finds the Essentia library.

Imported Targets
^^^^^^^^^^^^^^^^

This module provides the following imported targets, if found:

``Essentia::essentia``
  The Essentia library

Result Variables
^^^^^^^^^^^^^^^^

This will define the following variables:

``Essentia_FOUND``
  True if the system has the Essentia library.
``Essentia_INCLUDE_DIRS``
  Include directories needed to use Essentia.
``Essentia_LIBRARIES``
  Libraries needed to link to Essentia.

Cache Variables
^^^^^^^^^^^^^^^

The following cache variables may also be set:

``Essentia_INCLUDE_DIR``
  The directory containing ``essentia/essentia.h``.
``Essentia_LIBRARY``
  The path to the Essentia library.

#]=======================================================================]

# 1. Try config first
find_package(essentia CONFIG QUIET)
if(essentia_FOUND)
  set(Essentia_FOUND TRUE)
  set(Essentia_INCLUDE_DIRS ${essentia_INCLUDE_DIRS})
  set(Essentia_LIBRARIES ${essentia_LIBRARIES})
  if(NOT TARGET Essentia::essentia)
    if(TARGET essentia)
      add_library(Essentia::essentia INTERFACE IMPORTED)
      set_target_properties(Essentia::essentia PROPERTIES
        INTERFACE_LINK_LIBRARIES essentia
      )
    endif()
  endif()
  return()
endif()

# 2. Try PkgConfig
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
  pkg_check_modules(PC_Essentia QUIET essentia)
endif()

# Define hints
set(Essentia_HINTS
  ${ESSENTIA_ROOT}
  $ENV{ESSENTIA_ROOT}
  ${PC_Essentia_LIBRARY_DIRS}
)

find_path(
  Essentia_INCLUDE_DIR
  NAMES essentia/essentia.h
  HINTS ${Essentia_HINTS}
  PATH_SUFFIXES include
  DOC "Essentia include directory"
)
mark_as_advanced(Essentia_INCLUDE_DIR)

find_library(
  Essentia_LIBRARY
  NAMES essentia
  HINTS ${Essentia_HINTS}
  PATH_SUFFIXES lib lib64
  DOC "Essentia library"
)
mark_as_advanced(Essentia_LIBRARY)

if(DEFINED PC_Essentia_VERSION AND NOT PC_Essentia_VERSION STREQUAL "")
  set(Essentia_VERSION "${PC_Essentia_VERSION}")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  Essentia
  REQUIRED_VARS Essentia_LIBRARY Essentia_INCLUDE_DIR
  VERSION_VAR Essentia_VERSION
)

if(Essentia_FOUND)
  set(Essentia_LIBRARIES "${Essentia_LIBRARY}")
  set(Essentia_INCLUDE_DIRS "${Essentia_INCLUDE_DIR}")

  find_path(
    Eigen3_INCLUDE_DIR
    NAMES unsupported/Eigen/CXX11/Tensor
    HINTS
      /opt/homebrew/include/eigen3
      /usr/local/include/eigen3
      /usr/include/eigen3
      ${Essentia_HINTS}
    PATH_SUFFIXES eigen3
  )
  if(Eigen3_INCLUDE_DIR)
    list(APPEND Essentia_INCLUDE_DIRS "${Eigen3_INCLUDE_DIR}")
  endif()

  if(NOT TARGET Essentia::essentia)
    add_library(Essentia::essentia UNKNOWN IMPORTED)
    set_target_properties(
      Essentia::essentia
      PROPERTIES
        IMPORTED_LOCATION "${Essentia_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${Essentia_INCLUDE_DIRS}"
    )
    if(PC_Essentia_FOUND)
      set_target_properties(
        Essentia::essentia
        PROPERTIES
          INTERFACE_COMPILE_OPTIONS "${PC_Essentia_CFLAGS_OTHER}"
      )
    endif()
  endif()
endif()
