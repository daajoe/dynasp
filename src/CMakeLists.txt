include_directories(${PROJECT_SOURCE_DIR}/include) # ${CMAKE_SOURCE_DIR})
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../cmake")
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} /home/vagrant/.local/lib)

#TODO:
#ExternalProject_Get_Property(htd install_dir)
#include_directories(${install_dir}/include)

#add_dependencies(project_b_exe project_a)
#target_link_libraries(project_b_exe ${install_dir}/lib/alib.lib)

#TODO: osx
#write_compiler_detection_header(
#        FILE "${PROJECT_SOURCE_DIR}/include/dynasp/CompilerDetection.hpp"
#        PREFIX DYNASP
#        COMPILERS GNU Clang AppleClang MSVC
#        FEATURES cxx_noexcept cxx_override cxx_lambdas
#)
#check_include_file_cxx(DYNASP/CompilerDetection.hpp DYNASP_HAVE_COMPILER_DETECTION)

set(CMAKE_REQUIRED_INCLUDES "${PROJECT_SOURCE_DIR}/include")

set(DYNASP_SOVERSION_INTERFACE 0)
set(DYNASP_SOVERSION_REVISION 0)
set(DYNASP_SOVERSION_AGE 0)
set(DYNASP_SOVERSION ${DYNASP_SOVERSION_INTERFACE}.${DYNASP_SOVERSION_REVISION}.${DYNASP_SOVERSION_AGE})

file(GLOB_RECURSE HEADER_LIST ${PROJECT_SOURCE_DIR}/include/*.hpp)

set(DYNASP_MAIN_VERSION_MAJOR 2)
set(DYNASP_MAIN_VERSION_MINOR 0)
set(DYNASP_MAIN_VERSION_PATCH 0)
set(DYNASP_MAIN_VERSION ${DYNASPMAIN_VERSION_MAJOR}.${DYNASP_MAIN_VERSION_MINOR}.${DYNASP_MAIN_VERSION_PATCH})

find_package(HTD REQUIRED)
find_package(SHARP REQUIRED)
find_package(GMP 10.3.2 REQUIRED)
find_package(BISON 2.4.1 REQUIRED)
find_package(FLEX 2.5.39 EXACT REQUIRED)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
#include_directories(${GMP_INCLUDE_DIR})

BISON_TARGET(LParseParser parsers/LParseBisonParser.yy ${CMAKE_CURRENT_BINARY_DIR}/parsers/LParseBisonParser.cc)
FLEX_TARGET(LParseLexer parsers/LParseLexer.ll ${CMAKE_CURRENT_BINARY_DIR}/parsers/LParseLexer.cc)
ADD_FLEX_BISON_DEPENDENCY(LParseLexer LParseParser)

LINK_DIRECTORIES( ${SHARP_LIBRARIES} )
LINK_DIRECTORIES( ${HTD_LIBRARIES} )

set(CMAKE_REQUIRED_INCLUDES "${PROJECT_SOURCE_DIR}/include")
file(GLOB_RECURSE HEADER_LIST ${PROJECT_SOURCE_DIR}/include/*.hpp)

file(GLOB_RECURSE DYNASP_SOURCES "*.cpp")

file(GLOB_RECURSE DYNASP_GEN_HEADERS "*.hh")
file(GLOB_RECURSE DYNASP_HEADERS "src/*.h")
file(GLOB_RECURSE DYNASP_GEN_SOURCES "${PROJECT_SOURCE_DIR}/src/parsers/*.cc")

file(GLOB_RECURSE APP_SRC *.cpp)
add_library(dynasp ${DYNASP_SOURCES} ${HEADER_LIST} ${DYNASP_GEN_HEADERS} ${DYNASP_GEN_SOURCES} ${BISON_LParseParser_OUTPUTS} ${FLEX_LParseLexer_OUTPUTS})

set_property(TARGET dynasp PROPERTY CXX_STANDARD 11)
set_property(TARGET dynasp PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET dynasp PROPERTY POSITION_INDEPENDENT_CODE 1)

set_target_properties(dynasp PROPERTIES VERSION ${DYNASP_SOVERSION} SOVERSION ${DYNASP_SOVERSION_INTERFACE})

set_target_properties(dynasp
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        )


add_executable(dynasp2 main.cpp ${BISON_LParseParser_OUTPUTS} ${FLEX_LParseLexer_OUTPUTS})
target_link_libraries(dynasp2 htd sharp gmp gmpxx dynasp)

set_property(TARGET dynasp2 PROPERTY CXX_STANDARD 11)
set_property(TARGET dynasp2 PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET dynasp2 PROPERTY POSITION_INDEPENDENT_CODE 1)

set_target_properties(dynasp2
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        )


install(TARGETS dynasp DESTINATION bin)
