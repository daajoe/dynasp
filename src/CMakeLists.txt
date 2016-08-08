include_directories(${PROJECT_SOURCE_DIR}/include)
aux_source_directory(. SRC_LIST)

#TODO:
#ExternalProject_Add(htd
#        URL ...htd
#        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/htd
#        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
#        )

#ExternalProject_Get_Property(htd install_dir)
#include_directories(${install_dir}/include)

#add_dependencies(project_b_exe project_a)
#target_link_libraries(project_b_exe ${install_dir}/lib/alib.lib)


#write_compiler_detection_header(
#        FILE "${PROJECT_SOURCE_DIR}/include/dynasp/CompilerDetection.hpp"
#        PREFIX DYNASP
#        COMPILERS GNU Clang AppleClang MSVC
#        FEATURES cxx_noexcept cxx_override cxx_lambdas
#)

#configure_file("${PROJECT_SOURCE_DIR}/cmake/Id.hpp.in" "${PROJECT_SOURCE_DIR}/include/dynasp/Id.hpp")

set(CMAKE_REQUIRED_INCLUDES "${PROJECT_SOURCE_DIR}/include")

#check_include_file_cxx(DYNASP/CompilerDetection.hpp DYNASP_HAVE_COMPILER_DETECTION)

set(DYNASP_SOVERSION_INTERFACE 0)
set(DYNASP_SOVERSION_REVISION 0)
set(DYNASP_SOVERSION_AGE 0)
set(DYNASP_SOVERSION ${DYNASP_SOVERSION_INTERFACE}.${DYNASP_SOVERSION_REVISION}.${DYNASP_SOVERSION_AGE})

file(GLOB_RECURSE HEADER_LIST ${PROJECT_SOURCE_DIR}/include/*.hpp)

add_executable(dynasp2 ${SRC_LIST} ${HEADER_LIST})

set_property(TARGET dynasp2 PROPERTY CXX_STANDARD 11)
set_property(TARGET dynasp2 PROPERTY CXX_STANDARD_REQUIRED ON)

set_property(TARGET dynasp2 PROPERTY POSITION_INDEPENDENT_CODE 1)

set_target_properties(dynasp2 PROPERTIES VERSION ${DYNASP_SOVERSION} SOVERSION ${DYNASP_SOVERSION_INTERFACE})

set_target_properties(dynasp2
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        )

if (UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -Ddynasp_EXPORTS")
endif (UNIX)

if (BUILD_SHARED_LIBS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDYNASP_SHARED_LIBRARY")
endif (BUILD_SHARED_LIBS)

#TODO: output dynasp binary

install(TARGETS dynasp2 DESTINATION bin)
install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/dynasp" DESTINATION include)