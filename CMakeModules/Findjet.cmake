# Try to find JET headers and libraries.
#
# Usage of this module as follows:
#
# find_package(JET)
#
# Variables used by this module, they can change the default behaviour and need
# to be set before calling find_package:
#
# JET_ROOT Set this variable to the root installation of
# libjet if the module has problems finding the
# proper installation path.
#
# Variables defined by this module:
#
# JET_FOUND System has JET libraries and headers
# JET_LIBRARIES The JET library
# JET_INCLUDE_DIRS The location of JET headers

# Get hint from environment variable (if any)
if(NOT JET_ROOT AND DEFINED ENV{JET_ROOT})
	set(JET_ROOT "$ENV{JET_ROOT}" CACHE PATH "JET base directory location (optional, used for nonstandard installation paths)")
	mark_as_advanced(JET_ROOT)
endif()

set(JET_ROOT "$ENV{HOME}/jet")
# Search path for nonstandard locations
if(JET_ROOT)
	set(JET_INCLUDE_PATH PATHS "${JET_ROOT}" NO_DEFAULT_PATH)
	set(JET_LIBRARY_PATH PATHS "${JET_ROOT}/lib" NO_DEFAULT_PATH)
endif()

find_path(JET_INCLUDE_DIRS NAMES Attr.hpp JoinExpTree.hpp HINTS ${JET_INCLUDE_PATH})
find_library(JET_LIBRARIES NAMES jet HINTS ${JET_LIBRARY_PATH})

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(JET DEFAULT_MSG JET_LIBRARIES JET_INCLUDE_DIRS)

mark_as_advanced(JET_ROOT JET_LIBRARIES JET_INCLUDE_DIRS)
