#
# Root vxl 
#
#   geoff.cross@crd.ge.com

PROJECT(allvxl)

OPTION(BUILD_TESTS "Should the tests be built?" NO)
OPTION(BUILD_SHARED_LIBS "Should shared libraries be the default?" NO)

OPTION(WARN_DEPRECATED "Enable runtime warnings for deprecated functions?" YES)
OPTION(WARN_DEPRECATED_ONCE "Only warn once per function (if runtime warnings are enabled)?" YES)
OPTION(WARN_DEPRECATED_ABORT "Abort on executing a deprecated function (if runtime warnings are enabled)?" NO)

IF(WARN_DEPRECATED)
  ADD_DEFINITIONS( -DVXL_WARN_DEPRECATED )
  IF(WARN_DEPRECATED_ONCE)
     ADD_DEFINITIONS( -DVXL_WARN_DEPRECATED_ONCE )
  ENDIF(WARN_DEPRECATED_ONCE)
  IF(WARN_DEPRECATED_ABORT)
     ADD_DEFINITIONS( -DVXL_WARN_DEPRECATED_ABORT )
  ENDIF(WARN_DEPRECATED_ABORT)
ENDIF(WARN_DEPRECATED)


SUBDIRS(vcl v3p vxl oxl gel mul tbl conversions)

# For out of place builds, the configure generated headers are in the
# binary directory
INCLUDE_DIRECTORIES(${allvxl_BINARY_DIR}/vcl
                    ${allvxl_BINARY_DIR}/vxl)

INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/vcl)

IF(WIN32)
  INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/vcl/config.win32-VC60)
ENDIF(WIN32)

IF(RUN_CONFIGURE)
  EXEC_PROGRAM( ${allvxl_SOURCE_DIR}/config.cmake/configure
                ${allvxl_BINARY_DIR} )
ENDIF(RUN_CONFIGURE)

INCLUDE( ${allvxl_SOURCE_DIR}/CMakeListsLocal.txt OPTIONAL )
