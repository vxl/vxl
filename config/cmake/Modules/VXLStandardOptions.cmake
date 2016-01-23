# vxl/config/cmake/VXLStandardOptions.cmake
#
# This CMake module is included by vxl/CMakeLists.txt.  It adds
# several vxl-standard testing and build options to the project:
#
#  VXL_BUILD_SHARED_LIBS
#  BUILD_TESTING
#  BUILD_EXAMPLES
#  WARN_DEPRECATED
#  WARN_DEPRECATED_ONCE
#  WARN_DEPRECATED_ABORT
#
# These options may be introduced into client projects with this line:
#
#  include(${VXL_CMAKE_DIR}/VXLStandardOptions.cmake)
#
# This module may be automatically included by UseVXL.cmake.
# See vxl/config/cmake/UseVXL.cmake for details.
#

# Everything here should be valid for both the vxl source and for
# client projects.

include(CTest)

if( WIN32 )
  option( VXL_BUILD_SHARED_LIBS "Should shared libraries be the default?" OFF )
else( WIN32 )
  option( VXL_BUILD_SHARED_LIBS "Should shared libraries be the default?" OFF )
endif( WIN32 )

set( SOLARIS 0 )
if( CMAKE_SYSTEM MATCHES "SunOS.*" )
  set( SOLARIS 1 )
endif( CMAKE_SYSTEM MATCHES "SunOS.*" )


option( WARN_DEPRECATED "Enable runtime warnings for deprecated functions?" ON )
option( WARN_DEPRECATED_ONCE "Only warn once per function (if runtime warnings are enabled)?" ON )
option( WARN_DEPRECATED_ABORT "Abort on executing a deprecated function (if runtime warnings are enabled)?" OFF )

mark_as_advanced( WARN_DEPRECATED WARN_DEPRECATED_ONCE WARN_DEPRECATED_ABORT )

if(WARN_DEPRECATED)
  add_definitions( -DVXL_WARN_DEPRECATED )
  if(WARN_DEPRECATED_ONCE)
    add_definitions( -DVXL_WARN_DEPRECATED_ONCE )
  endif(WARN_DEPRECATED_ONCE)
  if(WARN_DEPRECATED_ABORT)
    add_definitions( -DVXL_WARN_DEPRECATED_ABORT )
  endif(WARN_DEPRECATED_ABORT)
endif(WARN_DEPRECATED)



if(VCL_HAS_LFS OR WIN32)
  option( VXL_USE_LFS "Should VXL use Large File Support?" NO)
  mark_as_advanced( VXL_USE_LFS )
endif(VCL_HAS_LFS OR WIN32)

if(VXL_USE_LFS)
  if(WIN32)
    # TODO: MS Version Support
    #  message( SEND_ERROR "Sorry - Large File Support is not quite working on Win32 yet. Turning VXL_USE_LFS off")
    #  set(VXL_USE_LFS "NO" CACHE BOOL "Should VXL use Large File Support?" FORCE)
  else(WIN32)
    if(VCL_HAS_LFS)
      add_definitions( -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE)
    else(VCL_HAS_LFS)
      message( SEND_ERROR "This platform does not have Large File Support - turning VXL_USE_LFS off")
      set(VXL_USE_LFS "NO" CACHE BOOL "Should VXL use Large File Support?" FORCE)
    endif(VCL_HAS_LFS)
  endif(WIN32)
endif(VXL_USE_LFS)

