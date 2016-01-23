#MESSAGE(FATAL_ERROR "VXL has moved!
#VXL is now hosted in a GitHub repository at:
# https://github.com/vxl/vxl
#Run:
# git config --unset remote.origin.pushurl
# git remote set-url origin https://github.com/vxl/vxl.git
# git fetch origin
# git checkout master
# git reset --hard origin/master
#")
#
# Root vxl
#
#   vxl-maintainers@lists.sf.net
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.9)

# Support for CMake 2.6
IF( COMMAND CMAKE_POLICY )
  CMAKE_POLICY(SET CMP0003 NEW)
ENDIF( COMMAND CMAKE_POLICY )

# CMake 2.8 stuff
SET( CMAKE_LEGACY_CYGWIN_WIN32 0 ) # Remove when CMake >= 2.8.4 is required

# Use @rpath on OS X
IF( POLICY CMP0042 )
  CMAKE_POLICY(SET CMP0042 NEW)
ENDIF()

PROJECT(vxl)

include(CMakeDependentOption)

find_program( MEMORYCHECK_COMMAND valgrind )
if(MEMORYCHECK_COMMAND)
  set( MEMORYCHECK_COMMAND_OPTIONS "--trace-children=yes --leak-check=full --malloc-fill=0xFF" )
  set( MEMORYCHECK_SUPPRESSIONS_FILE "${PROJECT_SOURCE_DIR}/config/valgrind.supp" )
endif()

SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${vxl_BINARY_DIR}/lib CACHE PATH
     "Output directory for the vxl libraries" )

IF( NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY )
    SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY "." )
ENDIF( NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY )


# CMake support directory.
SET(VXL_CMAKE_DIR ${vxl_SOURCE_DIR}/config/cmake/Modules)

INCLUDE( ${VXL_CMAKE_DIR}/VXLStandardOptions.cmake )
INCLUDE( ${vxl_SOURCE_DIR}/config/cmake/config/vxl_utils.cmake )
# INCLUDE( ${vxl_SOURCE_DIR}/UseSTLPort.cmake )
INCLUDE(${vxl_SOURCE_DIR}/config/cmake/doxygen/doxygen.cmake)

# Copy the UseVXL.cmake file to the binary directory so that client
# projects don't need to find the source directory first. They can run
# the UseVXL.cmake from the vxl binary directory, and determine the
# vxl source directory by loading the cache.
CONFIGURE_FILE( ${VXL_CMAKE_DIR}/UseVXL.cmake
                ${vxl_BINARY_DIR}/UseVXL.cmake COPYONLY )

# Copy CTestCustom.cmake to top of build tree
CONFIGURE_FILE( ${VXL_CMAKE_DIR}/CTestCustom.cmake
  ${vxl_BINARY_DIR}/CTestCustom.cmake COPYONLY )

# Location of VXL's FindXXX.cmake CMake modules.
# This is identical to VXL_CMAKE_DIR.  Perhaps we should eliminate MODULE_PATH?
SET( MODULE_PATH ${vxl_SOURCE_DIR}/config/cmake/Modules CACHE STATIC "VXL module path" )

# For use in client projects that call UseVXL.cmake
SET (VXL_LIBRARY_PATH ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} CACHE STATIC "Where all the vxl libraries are, for clients to use." )

# Options to add extra compiler and linker flags
#
# These options allow you to specify additional flags without
# affecting the default flags for a particular platform or build type.
# This is especially useful for adding extra warning flags.
SET( VXL_EXTRA_CMAKE_C_FLAGS             CACHE STRING "Extra flags appended to CMAKE_C_FLAGS" )
SET( VXL_EXTRA_CMAKE_CXX_FLAGS           CACHE STRING "Extra flags appended to CMAKE_CXX_FLAGS" )
SET( VXL_EXTRA_CMAKE_EXE_LINKER_FLAGS    CACHE STRING "Extra flags appended to CMAKE_EXE_LINKER_FLAGS" )
SET( VXL_EXTRA_CMAKE_MODULE_LINKER_FLAGS CACHE STRING "Extra flags appended to CMAKE_MODULE_LINKER_FLAGS" )
SET( VXL_EXTRA_CMAKE_SHARED_LINKER_FLAGS CACHE STRING "Extra flags appended to CMAKE_SHARED_LINKER_FLAGS" )
SET( CMAKE_C_FLAGS             "${CMAKE_C_FLAGS} ${VXL_EXTRA_CMAKE_C_FLAGS}" )
SET( CMAKE_CXX_FLAGS           "${CMAKE_CXX_FLAGS} ${VXL_EXTRA_CMAKE_CXX_FLAGS}" )
SET( CMAKE_EXE_LINKER_FLAGS    "${CMAKE_EXE_LINKER_FLAGS} ${VXL_EXTRA_CMAKE_EXE_LINKER_FLAGS}" )
SET( CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${VXL_EXTRA_CMAKE_MODULE_LINKER_FLAGS}" )
SET( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${VXL_EXTRA_CMAKE_SHARED_LINKER_FLAGS}" )


# Use this variable in vxl_config.h to tell if it is a shared build
set(VXL_BUILD_SHARED FALSE)
if(BUILD_SHARED_LIBS)
  set(VXL_BUILD_SHARED TRUE)
endif()


#-------------------------------------------------------------------
#-- BUILD CONFIG OPTIONS

# Optionally use old error reporting methods, rather than exceptions.
# The main use is in vil which often uses/used null images to imply an error.
OPTION (VXL_LEGACY_ERROR_REPORTING "Use old error reporting methods rather than exceptions?" ON)
IF (VXL_LEGACY_ERROR_REPORTING)
  ADD_DEFINITIONS( -DVXL_LEGACY_ERROR_REPORTING )
ENDIF (VXL_LEGACY_ERROR_REPORTING)

# Option to build Windows Unicode support, the string
# type of which is wchar_t, each character is a 16-bit unsigned integer.
IF(WIN32)
  IF(VXL_HAS_WIN_WCHAR_T)
    OPTION( VXL_USE_WIN_WCHAR_T "Build overloading functions that accept Windows wide char strings?" ON )
    IF(VXL_USE_WIN_WCHAR_T)   # Force it to be 0/1
      SET(VXL_USE_WIN_WCHAR_T 1)
    ELSE(VXL_USE_WIN_WCHAR_T)
      SET(VXL_USE_WIN_WCHAR_T 0)
    ENDIF(VXL_USE_WIN_WCHAR_T)
  ELSE(VXL_HAS_WIN_WCHAR_T)
    SET(VXL_USE_WIN_WCHAR_T 0)
  ENDIF(VXL_HAS_WIN_WCHAR_T)
ELSE(WIN32)
  # avoid empty macro definition
  SET(VXL_USE_WIN_WCHAR_T 0)
ENDIF(WIN32)

# In order to be able to link vxl libraries into shared libraries on 64 bit linux, the -fPIC
# compiler flag must be added.  Only do this if we are on a x86_64 *nix platform, we're building
# static libraries, and the user has not explicitly requested position dependent code.
IF(UNIX)
  IF(NOT BUILD_SHARED_LIBS AND CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
    OPTION(BUILD_POSITION_DEPENDENT_CODE "Generate position dependent code (i.e. code cannot be used in shared library)" OFF)
    MARK_AS_ADVANCED(BUILD_POSITION_DEPENDENT_CODE)
    IF (NOT BUILD_POSITION_DEPENDENT_CODE)
      message(STATUS "Adding -fPIC compiler flag to generate position independent code.")
      SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}-fPIC")
      SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}-fPIC")
    ENDIF(NOT BUILD_POSITION_DEPENDENT_CODE)
  ENDIF(NOT BUILD_SHARED_LIBS AND CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
ENDIF(UNIX)



# Use the old configure script if the cache tells us to do so.
IF(VXL_NO_CMAKE_CONFIGURE)
  IF(RUN_CONFIGURE)
    EXEC_PROGRAM( ${vxl_SOURCE_DIR}/configure
      ${vxl_BINARY_DIR} )
  ENDIF(RUN_CONFIGURE)
ENDIF(VXL_NO_CMAKE_CONFIGURE)

# Some types of path names can cause havoc with regular expressions,
# so avoid those.
IF( ${vxl_BINARY_DIR} MATCHES  \\+ )
  MESSAGE(SEND_ERROR "You cannot have a + in your binary path")
ENDIF( ${vxl_BINARY_DIR} MATCHES  \\+ )
IF( ${vxl_SOURCE_DIR} MATCHES  \\+ )
  MESSAGE(SEND_ERROR "You cannot have a + in your source path")
ENDIF( ${vxl_SOURCE_DIR} MATCHES  \\+ )

# include CMakeListsLocal.txt from source directory if it exists
# also include it from the binary dir if different from source dir
IF( ${vxl_BINARY_DIR} MATCHES ${vxl_SOURCE_DIR} )
  INCLUDE( ${vxl_SOURCE_DIR}/CMakeListsLocal.txt OPTIONAL )
ELSE( ${vxl_BINARY_DIR} MATCHES ${vxl_SOURCE_DIR} )
  INCLUDE( ${vxl_SOURCE_DIR}/CMakeListsLocal.txt OPTIONAL )
  INCLUDE( ${vxl_BINARY_DIR}/CMakeListsLocal.txt OPTIONAL )
ENDIF( ${vxl_BINARY_DIR} MATCHES ${vxl_SOURCE_DIR} )

# Standard include directories.
SET(VXLCORE_INCLUDE_DIR ${vxl_BINARY_DIR}/core ${vxl_SOURCE_DIR}/core)
SET(VXLCORE_INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include/vxl/core)
SET(VCL_INCLUDE_DIR ${vxl_BINARY_DIR}/vcl ${vxl_SOURCE_DIR}/vcl)
SET(VCL_INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include/vxl/vcl)

# Autoconf does not work on windows.  Use hardcoded results if not using
# cmake to configure.
IF(VXL_NO_CMAKE_CONFIGURE)
  IF(WIN32)
    IF(NOT CYGWIN)
      SET(VCL_INCLUDE_DIR ${vxl_SOURCE_DIR}/vcl/config.win32 ${VCL_INCLUDE_DIR})
      SET(VCL_INSTALL_INCLUDE_DIR
        ${CMAKE_INSTALL_PREFIX}/include/vcl/config.win32
        ${VCL_INSTALL_INCLUDE_DIR}
      )
    ENDIF(NOT CYGWIN)
  ENDIF(WIN32)
ENDIF(VXL_NO_CMAKE_CONFIGURE)

INCLUDE_DIRECTORIES(${VCL_INCLUDE_DIR} ${VXLCORE_INCLUDE_DIR})

# Do platform-specific configuration.
IF(NOT VXL_NO_CMAKE_CONFIGURE)
  add_subdirectory(config/cmake/config)
ENDIF(NOT VXL_NO_CMAKE_CONFIGURE)

#-------------------------------------------------------------------
#-- BUILD SELECTION OPTIONS
# Options for selecting the core core libraries
OPTION( BUILD_CORE_GEOMETRY "Build VXL's geometry libraries" ON )
OPTION( BUILD_CORE_NUMERICS "Build VXL's numerics libraries" ON )
OPTION( BUILD_CORE_SERIALISATION "Build VXL's serialisation libraries" ON )
OPTION( BUILD_CORE_UTILITIES "Build VXL's utility libraries" ON )
OPTION( BUILD_CORE_IMAGING "Build VXL's imaging libraries" ON )

# By default, build examples when building tests. Examples require most of the core libraries
CMAKE_DEPENDENT_OPTION( BUILD_EXAMPLES "Should the examples be built?" ${BUILD_TESTING}
                      "BUILD_CORE_GEOMETRY;BUILD_CORE_NUMERICS;BUILD_CORE_UTILITIES;BUILD_CORE_SERIALISATION;BUILD_CORE_IMAGING" OFF)

# Option to specify whether this is a build for the dashboard.  Each
# dashboard build should set BUILD_FOR_VXL_DASHBOARD to ON in the
# initial cache (set in the CTest script).
OPTION( BUILD_FOR_VXL_DASHBOARD "Enable deprecated code and known failing test?)" OFF )
OPTION( BUILD_NONDEPRECATED_ONLY "Build only nondeprecated libraries (Experimental)" ON )

CMAKE_DEPENDENT_OPTION(BUILD_CORE_PROBABILITY "Build VXL's probability libraries (Experimental)" ON "BUILD_CORE_NUMERICS;BUILD_FOR_VXL_DASHBOARD" OFF)

# Build the core vxl + support libraries
add_subdirectory(vcl)
add_subdirectory(v3p)
add_subdirectory(core)

# Optionally build the contributed libraries
if( EXISTS ${vxl_SOURCE_DIR}/contrib/CMakeLists.txt )
  CMAKE_DEPENDENT_OPTION(BUILD_CONTRIB "Build the contributed libraries?" ON "BUILD_CORE_GEOMETRY;BUILD_CORE_NUMERICS;BUILD_CORE_UTILITIES;BUILD_CORE_SERIALISATION;BUILD_CORE_IMAGING" OFF)
  add_subdirectory(contrib)
endif()

# This add_subdirectory command must be the last add_subdirectory command in this file
add_subdirectory(config/cmake/export)
