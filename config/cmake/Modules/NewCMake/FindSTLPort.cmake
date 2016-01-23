#
# Look for a directory containing STLPort.
#
# The following values are defined
# STLPORT_INSTALL_DIR - STLPort's installation directory.
# STLPORT_INCLUDE_DIR - where to find vector, etc.
# STLPORT_LIBRARIES   - link against these to use STLPort
# STLPORT_FOUND       - True if the X11 extensions are available.


# Provide some common installation locations.
# Otherwise, the user will have to specify it in the cache.
find_path( STLPORT_INSTALL_DIR stlport/iostream
   /usr/local/STLPort-4.5.3
)

# Assume for the moment that the STLPORT_INSTALL directory contains
# both the subdirectory for header file includes (.../stlport) and
# the subdirectory for libraries (../lib).


find_path( STLPORT_INCLUDE_DIR iostream
   ${STLPORT_INSTALL_DIR}/stlport
)


if(CMAKE_BUILD_TYPE MATCHES "Debug")
  # "Debug" probably means we do not want the non-debug ones.
  find_library( STLPORT_LIBRARIES
    NAMES stlport_cygwin_debug
          stlport_cygwin_stldebug
          stlport_gcc_debug
          stlport_gcc_stldebug
    PATHS ${STLPORT_INSTALL_DIR}/lib
  )
else(CMAKE_BUILD_TYPE MATCHES "Debug")
  # if we only have debug libraries, use them.
  # that is surely better than nothing.
  find_library( STLPORT_LIBRARIES
    NAMES stlport_cygwin
          stlport_cygwin_debug
          stlport_cygwin_stldebug
          stlport_gcc
          stlport_gcc_debug
          stlport_gcc_stldebug
    PATHS ${STLPORT_INSTALL_DIR}/lib
  )
endif(CMAKE_BUILD_TYPE MATCHES "Debug")


#
# For GCC, should we consider using -nostdinc or -isystem to
# point to the STLPort system header directory? It is quite
# important that we get the STLPort C++ header files and not
# those that come with GCC.
#


if( STLPORT_INCLUDE_DIR )
  if( STLPORT_LIBRARIES )
    set( STLPORT_FOUND "YES" )

    # stlport_gcc needs pthread.
    if(UNIX)
      set( STLPORT_LIBRARIES
        ${STLPORT_LIBRARIES} pthread )
    endif(UNIX)

  endif( STLPORT_LIBRARIES )
endif( STLPORT_INCLUDE_DIR )

mark_as_advanced(
  STLPORT_INCLUDE_DIR
  STLPORT_INSTALL_DIR
  STLPORT_LIBRARIES
)
