#
# Root vxl 
#
#   geoff.cross@crd.ge.com

PROJECT(allvxl)

INCLUDE (${CMAKE_ROOT}/Modules/Dart.cmake)

IF(WIN32)
  OPTION(BUILD_SHARED_LIBS "Should shared libraries be the default?" NO)
ELSE(WIN32)
  OPTION(BUILD_SHARED_LIBS "Should shared libraries be the default?" YES)
ENDIF(WIN32)

# Some people may want to build the test cases even though they aren't
# using Dart.
IF(NOT DART_ROOT)
  IF(WIN32)
    OPTION(BUILD_TESTING "Should the tests be built?" NO)
  ELSE(WIN32)
    OPTION(BUILD_TESTING "Should the tests be built?" YES)
  ENDIF(WIN32)

  IF(BUILD_TESTING)
    ENABLE_TESTING()
  ENDIF(BUILD_TESTING)
ENDIF(NOT DART_ROOT)
OPTION(BUILD_TESTING "Should the tests be built?" YES)

# By default, build examples when building tests.
OPTION(BUILD_EXAMPLES "Should the examples be built?" ${BUILD_TESTING})

OPTION(WARN_DEPRECATED "Enable runtime warnings for deprecated functions?" YES)
OPTION(WARN_DEPRECATED_ONCE "Only warn once per function (if runtime warnings are enabled)?" YES)
OPTION(WARN_DEPRECATED_ABORT "Abort on executing a deprecated function (if runtime warnings are enabled)?" NO)

OPTION(BUILD_MUL "Should the Manchester University Library be built?" YES)
OPTION(BUILD_OXL "Should the Oxford University Library be built?" YES)
OPTION(BUILD_GEL "Should the GE Library be built?" YES)
OPTION(BUILD_TBL "Should the TB Library be built?" YES)
OPTION(BUILD_RPL "Should the RPI Library be built?" YES)
OPTION(BUILD_OUL "Should the Otago University package be built?" YES)
OPTION(BUILD_CONVERSIONS "Should the Conversions Examples be built?" YES)



IF(WARN_DEPRECATED)
  ADD_DEFINITIONS( -DVXL_WARN_DEPRECATED )
  IF(WARN_DEPRECATED_ONCE)
     ADD_DEFINITIONS( -DVXL_WARN_DEPRECATED_ONCE )
  ENDIF(WARN_DEPRECATED_ONCE)
  IF(WARN_DEPRECATED_ABORT)
     ADD_DEFINITIONS( -DVXL_WARN_DEPRECATED_ABORT )
  ENDIF(WARN_DEPRECATED_ABORT)
ENDIF(WARN_DEPRECATED)


SUBDIRS(vcl v3p vxl)

IF(BUILD_MUL)
  SUBDIRS(mul)
ENDIF(BUILD_MUL)

IF(BUILD_OXL)
  SUBDIRS(oxl)
ENDIF(BUILD_OXL)

IF(BUILD_GEL)
  SUBDIRS(gel)
ENDIF(BUILD_GEL)

IF(BUILD_TBL)
  SUBDIRS(tbl)
ENDIF(BUILD_TBL)

IF(BUILD_RPL)
  SUBDIRS(rpl)
ENDIF(BUILD_RPL)

IF(BUILD_OUL)
  SUBDIRS(oul)
ENDIF(BUILD_OUL)

IF(BUILD_CONVERSIONS)
  SUBDIRS(conversions)
ENDIF(BUILD_CONVERSIONS)


# For out of place builds, the configure generated headers are in the
# binary directory
INCLUDE_DIRECTORIES(${allvxl_BINARY_DIR}/vcl
                    ${allvxl_BINARY_DIR}/vxl)

INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/vcl)

IF(WIN32)
  INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/vcl/config.win32-VC60)
ENDIF(WIN32)

IF(RUN_CONFIGURE)
  EXEC_PROGRAM( ${allvxl_SOURCE_DIR}/configure
                ${allvxl_BINARY_DIR} )
ENDIF(RUN_CONFIGURE)

# include CMakeListsLocal.txt from source directory if it exists
# also include it from the binary dir if different from source dir
IF( ${allvxl_BINARY_DIR} MATCHES ${allvxl_SOURCE_DIR} )
  INCLUDE( ${allvxl_SOURCE_DIR}/CMakeListsLocal.txt OPTIONAL )
ELSE( ${allvxl_BINARY_DIR} MATCHES ${allvxl_SOURCE_DIR} )
  INCLUDE( ${allvxl_SOURCE_DIR}/CMakeListsLocal.txt OPTIONAL )
  INCLUDE( ${allvxl_BINARY_DIR}/CMakeListsLocal.txt OPTIONAL )
ENDIF( ${allvxl_BINARY_DIR} MATCHES ${allvxl_SOURCE_DIR} )
