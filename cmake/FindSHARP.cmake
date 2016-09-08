# Try to find the SHARP librairies
#  SHARP_FOUND - system has SHARP lib
#  SHARP_INCLUDE_DIR - the SHARP include directory
#  SHARP_LIBRARIES - Libraries needed to use SHARP

# Copyright (c) 2016, Johannes K. Fichte, <fichte@dbai.tuwien.ac.at>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#find_package(SHARP)

if (SHARP_INCLUDE_DIR AND SHARP_LIBRARIES)
  # Already in cache, be silent
  set(SHARP_FIND_QUIETLY TRUE)
endif (SHARP_INCLUDE_DIR AND SHARP_LIBRARIES)

find_path(SHARP_INCLUDE_DIR NAMES sharp/Benchmark.hpp)
find_library(SHARP_LIBRARIES NAMES libsharp sharp)

MESSAGE(STATUS "SHARP libs: " ${SHARP_LIBRARIES} " " ${SHARPXX_LIBRARIES} )

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SHARP DEFAULT_MSG SHARP_INCLUDE_DIR SHARP_LIBRARIES)

mark_as_advanced(SHARP_INCLUDE_DIR SHARP_LIBRARIES)
