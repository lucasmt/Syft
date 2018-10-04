# Try to find RSYNTH headers and libraries.
#
# Usage of this module as follows:
#
# find_package(RSYNTH)
#
# Variables used by this module, they can change the default behaviour and need
# to be set before calling find_package:
#
# RSYNTH_ROOT Set this variable to the root installation of
# libcudd if the module has problems finding the
# proper installation path.
#
# Variables defined by this module:
#
# RSYNTH_FOUND System has RSYNTH libraries and headers
# RSYNTH_LIBRARIES The RSYNTH library
# RSYNTH_INCLUDE_DIRS The location of RSYNTH headers

# Get hint from environment variable (if any)
if(NOT RSYNTH_ROOT AND DEFINED ENV{RSYNTH_ROOT})
	set(RSYNTH_ROOT "$ENV{RSYNTH_ROOT}" CACHE PATH "RSYNTH base directory location (optional, used for nonstandard installation paths)")
	mark_as_advanced(RSYNTH_ROOT)
endif()

set(RSYNTH_ROOT "$ENV{HOME}/rsynth")
# Search path for nonstandard locations
if(RSYNTH_ROOT)
	set(RSYNTH_INCLUDE_PATH PATHS "${RSYNTH_ROOT}/include" NO_DEFAULT_PATH)
	set(RSYNTH_LIBRARY_PATH PATHS "${RSYNTH_ROOT}/lib" NO_DEFAULT_PATH)
endif()

find_path(RSYNTH_INCLUDE_DIRS NAMES BDDMgr.hpp HINTS ${RSYNTH_INCLUDE_PATH})
find_library(RSYNTH_LIBRARIES NAMES rsynth HINTS ${RSYNTH_LIBRARY_PATH})

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(RSYNTH DEFAULT_MSG RSYNTH_LIBRARIES RSYNTH_INCLUDE_DIRS)

mark_as_advanced(RSYNTH_ROOT RSYNTH_LIBRARIES RSYNTH_INCLUDE_DIRS)
