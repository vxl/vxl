#
# Look for a directory containing STLPort.
#
# Notes:
#   [1] Experimental version.
#   [2] Inadequate and poorly tested.
#   [3] Use at own risk.
#

# Provide some common installation locations.
# Otherwise, the user will have to specify it in the cache.
FIND_PATH( STLPORT_INSTALL stlport/iostream
   /usr/local/STLPort-4.5.3
)

# Assume for the moment that the STLPORT_INSTALL directory contains
# both the subdirectory for header file includes (.../stlport) and
# the subdirectory for libraries (../lib).


FIND_PATH( STLPORT_INCLUDE_DIR iostream
   ${STLPORT_INSTALL}/stlport
)


IF(CMAKE_BUILD_TYPE MATCHES "Debug")
  # "Debug" probably means we do not want the non-debug ones.
  FIND_LIBRARY( STLPORT_LIBRARIES
    NAMES stlport_cygwin_debug
          stlport_cygwin_stldebug
          stlport_gcc_debug
          stlport_gcc_stldebug
    PATHS ${STLPORT_INSTALL}/lib
  )
ELSE(CMAKE_BUILD_TYPE MATCHES "Debug")
  # if we only have debug libraries, use them.
  # that is surely better than nothing.
  FIND_LIBRARY( STLPORT_LIBRARIES
    NAMES stlport_cygwin
          stlport_cygwin_debug
          stlport_cygwin_stldebug
          stlport_gcc
          stlport_gcc_debug
          stlport_gcc_stldebug
    PATHS ${STLPORT_INSTALL}/lib
  )
ENDIF(CMAKE_BUILD_TYPE MATCHES "Debug")


#
# For GCC, should we consider using -nostdinc or -isystem to 
# point to the STLPort system header directory? It is quite
# important that we get the STLPort C++ header files and not
# those that come with GCC.
#


IF( STLPORT_INCLUDE_DIR )
  IF( STLPORT_LIBRARIES )
    SET( STLPORT_FOUND "YES" )

    # stlport_gcc needs pthread.
    IF(UNIX)
      SET( STLPORT_LIBRARIES
        ${STLPORT_LIBRARIES} pthread )
    ENDIF(UNIX)

  ENDIF( STLPORT_LIBRARIES )
ENDIF( STLPORT_INCLUDE_DIR )
