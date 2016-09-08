# Try to find the HTD librairies
#  HTD_FOUND - system has HTD lib
#  HTD_INCLUDE_DIR - the HTD include directory
#  HTD_LIBRARIES - Libraries needed to use HTD

# Copyright (c) 2016, Johannes K. Fichte, <fichte@dbai.tuwien.ac.at>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

if (HTD_INCLUDE_DIR AND HTD_LIBRARIES)
  # Already in cache, be silent
  set(HTD_FIND_QUIETLY TRUE)
endif (HTD_INCLUDE_DIR AND HTD_LIBRARIES)

find_path(HTD_INCLUDE_DIR NAMES htd/ITreeDecomposition.hpp )
find_library(HTD_LIBRARIES NAMES htd libhtd )
MESSAGE(STATUS "HTD libs: " ${HTD_LIBRARIES} " " ${HTDXX_LIBRARIES} )

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(HTD DEFAULT_MSG HTD_INCLUDE_DIR HTD_LIBRARIES)

mark_as_advanced(HTD_INCLUDE_DIR HTD_LIBRARIES)
