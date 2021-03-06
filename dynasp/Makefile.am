############################
# FLAGS:                   #
############################

# tell automake to pass certain flags to autoconf on re-run
ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}

# tell automake to pass certain flags to 'ctags'
AM_CTAGSFLAGS = --extra=+f

# tell automake to pass certain flags to 'lex'
AM_LFLAGS = -olex.yy.c

# tell automake to pass certain flags to 'yacc'
AM_YFLAGS = --defines=y.tab.h -oy.tab.c

#FIXME: don't assume GCC, do feature detection in configure.ac
# tell automake to pass certain CPPFLAGS to the compiler
AM_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-pedantic-errors \
	-Wpedantic \
	-Wall \
	-Wextra \
	-Wdouble-promotion \
	-Wswitch-default \
	-fvisibility=hidden \
	-fvisibility-inlines-hidden

				
############################
# LIBRARIES TO BUILD:	   #
############################

# tell automake that some source files are generated
BUILT_SOURCES = 

# tell automake which libraries to build
lib_LTLIBRARIES = libdynasp.la

#----------------------------#
# libdynasp.la build options #
#----------------------------#

# list of generated header files
LIBDYNASP_GENERATED_HEADER = \
	src/parsers/LParseBisonParser.hh \
	stack.hh \
	position.hh \
	location.hh

# list of generated source files
LIBDYNASP_GENERATED_SOURCE = \
	src/parsers/LParseBisonParser.cc \
	src/parsers/LParseLexer.cc

# add those to the automake variable for generated files
BUILT_SOURCES += $(LIBDYNASP_GENERATED_HEADER) $(LIBDYNASP_GENERATED_SOURCE)

# list the names of the public header files of the libdynasp.la library
libdynasp_la_include_HEADERS = \
	include/dynasp/main \
	include/dynasp/global \
	\
	include/dynasp/IDynAspCountingSolutionExtractor.hpp \
	include/dynasp/IDynAspCountingSolution.hpp \
	include/dynasp/IDynAspTupleFactory.hpp \
	include/dynasp/IDynAspTuple.hpp \
	include/dynasp/IGroundAspInstanceFactory.hpp \
	include/dynasp/IGroundAspInstance.hpp \
	include/dynasp/IGroundAspRuleFactory.hpp \
	include/dynasp/IGroundAspRule.hpp \
	include/dynasp/IHypergraphConverterFactory.hpp \
	include/dynasp/IHypergraphConverter.hpp \
	include/dynasp/IParserFactory.hpp \
	include/dynasp/IParser.hpp \
	\
	include/dynasp/create.hpp \
	include/dynasp/Parser.hpp \
	\
	include/dynasp/Atom.hpp \
	include/dynasp/Rule.hpp \
	\
	include/dynasp/TreeNodeInfo.hpp \
	include/dynasp/DynAspAlgorithm.hpp

# list all source code files for the libdynasp.la library
# don't need to list the generated .cc files from bison and flex
# since automake already knows about them, and adds them automatically
libdynasp_la_SOURCES = \
	src/assembly.cpp \
	\
	src/create.cpp \
	src/Parser.cpp \
	\
	src/algorithms/DynAspAlgorithm.cpp \
	src/algorithms/DynAspCountingSolution.cpp \
	src/algorithms/DynAspCountingSolutionExtractor.cpp \
	src/algorithms/DynAspCountingSolutionExtractor.hpp \
	src/algorithms/DynAspCountingSolution.hpp \
	src/algorithms/FullDynAspTuple.cpp \
	src/algorithms/FullDynAspTuple.hpp \
	src/algorithms/InverseSimpleDynAspTuple.cpp \
	src/algorithms/InverseSimpleDynAspTuple.hpp \
	src/algorithms/RuleSetDynAspTuple.cpp \
	src/algorithms/RuleSetDynAspTuple.hpp \
	src/algorithms/SimpleDynAspTuple.cpp \
	src/algorithms/SimpleDynAspTuple.hpp \
	\
	src/instances/GroundAspInstance.cpp \
	src/instances/GroundAspInstance.hpp \
	src/instances/GroundAspRule.cpp \
	src/instances/GroundAspRule.hpp \
	src/instances/IncidenceHypergraphConverter.cpp \
	src/instances/IncidenceHypergraphConverter.hpp \
	src/instances/IncidencePrimalHypergraphConverter.cpp \
	src/instances/IncidencePrimalHypergraphConverter.hpp \
	src/instances/PrimalHypergraphConverter.cpp \
	src/instances/PrimalHypergraphConverter.hpp \
	\
	src/parsers/LParseBisonParser.hpp \
	src/parsers/LParseBisonParser.yy \
	src/parsers/LParseLexer.hpp \
	src/parsers/LParseLexer.ll \
	src/parsers/LParseParser.cpp \
	src/parsers/LParseParser.hpp \
	\
	src/util/errorhandling.cpp \
	src/util/errorhandling.hpp \
	src/util/debug.cpp \
	src/util/debug.hpp \
	\
	$(LIBDYNASP_GENERATED_HEADER)

# tell automake that the installation directory for public header files of the
# libdynasp.la library should be a subfolder in the default ${includedir}
libdynasp_la_includedir = ${includedir}/dynasp

# tell libtool to pass certain LDFLAGS to the compiler when building libsharp.la
#
# 1) Specify the Shared Object Version Information for the library, which
# consists of three numbers: CURRENT:REVISION:AGE
#
#   CURRENT: This is the version of the (external) ABI in the library.
#       Increase if the ABI changes (i.e. interfaces added or changed)!
#
#   REVISION: The revision represents how often the library was changed for a
#       given ABI version. Increase this whenever the library code changes, but
#       reset to 0 when incrementing CURRENT.
#
#   AGE: This is the difference between the oldest and newest ABI version
#       that the library is backwards compatible with (e.g. if CURRENT is 4,
#       but we are backwards compatible to ABI version 1, then AGE = 3). Set to
#       0 if there is a backwards-compatibility breaking change, otherwise
#       increase by 1 if CURRENT was increased by one.
#
libdynasp_la_LDFLAGS = -version-info 0:0:0

############################
# PROGRAMS TO BUILD:	   #
############################

# Link the main library (libdynasp.la) to all programs.
LDADD = libdynasp.la

# tell automake which programs to build
bin_PROGRAMS = dynasp

#----------------------#
# dynasp build options #
#----------------------#

# list all source code files for the dynasp program
dynasp_SOURCES = \
	src/main.cpp

############################
# DISTRIBUTION OPTIONS:	   #
############################

# tell automake about scripts that should be distributed in the tarball, but
# not installed on "make install"
dist_noinst_SCRIPTS = autogen.sh

# tell automake which files to package into distribution tarballs not involved
# in rules or autoconf templates
EXTRA_DIST = 

# tell automake to clean the generated files on "make clean"
CLEANFILES = $(BUILT_SOURCES)

############################
# UNIT TESTS:              #
############################

# tell automake to recursively go into the test directory
SUBDIRS = tests
