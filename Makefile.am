############################
# FLAGS:                   #
############################

# tell automake to pass certain flags to autoconf on re-run
ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}

# tell automake to pass certain flags to 'ctags'
AM_CTAGSFLAGS = --extra=+f

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

# tell automake which libraries to build
noinst_LTLIBRARIES = libdynasp.la

#----------------------------#
# libdynasp.la build options #
#----------------------------#

# list all source code files for the libdynasp.la library
libdynasp_la_SOURCES = \
	src/util/errorhandling.cpp \
	src/util/errorhandling.hpp

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

# tell automake that the files after the "=" should not be installed into a
# distribution tarball of the library.
dist_noinst_SCRIPTS = autogen.sh

# tell automake which files to package into distribution tarballs not involved
# in rules or autoconf templates
EXTRA_DIST = 

############################
# UNIT TESTS:              #
############################

# tell automake to recursively go into the test directory
SUBDIRS = tests
