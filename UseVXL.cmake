# If this is not the vxl tree, load the cache entries from the vxl
# tree
IF( NOT BUILDING_IN_VXL_TREE )

  IF( VXL_BINARY_PATH )
    LOAD_CACHE( ${VXL_BINARY_PATH}
      EXCLUDE
        BUILD_CONVERSIONS
        BUILD_TBL
        BUILD_MUL
        BUILD_OXL
        BUILD_GEL
        BUILD_RPL
        BUILD_TESTING
        BUILD_EXAMPLES
        BUILD_SHARED_LIBS
        LIBRARY_OUTPUT_PATH
        EXECUTABLE_OUTPUT_PATH
        CMAKE_INSTALL_PREFIX
        WARN_DEPRECATED
        WARN_DEPRECATED_ONCE
        WARN_DEPRECATED_ABORT
        MAKECOMMAND
        SITE
        BUILDNAME
        DART_ROOT
        DROP_SITE
        DROP_LOCATION
        DROP_SITE_USER
        DROP_SITE_PASSWORD
        TRIGGER_SITE
        NIGHTLY_START_TIME
        DROP_METHOD
      INCLUDE_INTERNALS
        allvxl_SOURCE_DIR
        allvxl_BINARY_DIR
    )
    LINK_DIRECTORIES( ${VXL_LIBRARY_PATH} )
  ELSE( VXL_BINARY_PATH )
    MESSAGE( SEND_ERROR "UseVXL.cmake requires the variable VXL_BINARY_PATH to point to the vxl binary directory" )
  ENDIF( VXL_BINARY_PATH )

ENDIF( NOT BUILDING_IN_VXL_TREE )

# Everything below should be valid for both the vxl source and for
# client projects.

INCLUDE( ${CMAKE_ROOT}/Modules/Dart.cmake )

IF( WIN32 )
  OPTION( BUILD_SHARED_LIBS "Should shared libraries be the default?" NO )
ELSE( WIN32 )
  OPTION( BUILD_SHARED_LIBS "Should shared libraries be the default?" YES )
ENDIF( WIN32 )

SET( SOLARIS 0 )
IF( CMAKE_SYSTEM MATCHES "SunOS.*" )
  SET( SOLARIS 1 )
ENDIF( CMAKE_SYSTEM MATCHES "SunOS.*" )

# Some people may want to build the test cases even though they aren't
# using Dart.
IF( NOT DART_ROOT )
  IF( WIN32 )
    OPTION( BUILD_TESTING "Should the tests be built?" NO )
  ELSE( WIN32 )
    OPTION( BUILD_TESTING "Should the tests be built?" YES )
  ENDIF( WIN32 )

  IF( BUILD_TESTING )
    ENABLE_TESTING()
  ENDIF( BUILD_TESTING )
ENDIF( NOT DART_ROOT )
OPTION( BUILD_TESTING "Should the tests be built?" YES )

# By default, build examples when building tests.
OPTION( BUILD_EXAMPLES "Should the examples be built?" ${BUILD_TESTING} )

OPTION( WARN_DEPRECATED "Enable runtime warnings for deprecated functions?" YES )
OPTION( WARN_DEPRECATED_ONCE "Only warn once per function (if runtime warnings are enabled)?" YES )
OPTION( WARN_DEPRECATED_ABORT "Abort on executing a deprecated function (if runtime warnings are enabled)?" NO )

MARK_AS_ADVANCED( WARN_DEPRECATED 
                  WARN_DEPRECATED_ONCE
                  WARN_DEPRECATED_ABORT )

IF(WARN_DEPRECATED)
  ADD_DEFINITIONS( -DVXL_WARN_DEPRECATED )
  IF(WARN_DEPRECATED_ONCE)
     ADD_DEFINITIONS( -DVXL_WARN_DEPRECATED_ONCE )
  ENDIF(WARN_DEPRECATED_ONCE)
  IF(WARN_DEPRECATED_ABORT)
     ADD_DEFINITIONS( -DVXL_WARN_DEPRECATED_ABORT )
  ENDIF(WARN_DEPRECATED_ABORT)
ENDIF(WARN_DEPRECATED)



# For out of place builds, the configure generated headers are in the
# binary directory
INCLUDE_DIRECTORIES(${allvxl_BINARY_DIR}/vcl
                    ${allvxl_BINARY_DIR}/vxl)

INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/vcl)

IF(WIN32)
  IF(NOT CYGWIN)
    INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/vcl/config.win32)
  ENDIF(NOT CYGWIN)
ENDIF(WIN32)
