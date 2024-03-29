# brl/CMakeLists.txt
# Mutual dependencies:  bbas --> bseg  and  bbas --> bmvl

project( brl )

##
#
# A macro to install resource files
#
macro(brl_resource_file infile installprefix)
  install(FILES ${infile}
      DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${installprefix}
      PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ
      COMPONENT Resource )
endmacro()

include(GNUInstallDirs)

doxygen_add_package(contrib/brl "Brown University Libraries")

if (NOT VXL_BUILD_CORE_VIDEO)
  message(FATAL_ERROR "VXL_BUILD_CORE_VIDEO:BOOL=ON is required for building BRL")
endif()

# In contrib/brl there is sometimes an "extra" level in the directory structure
# (.../contrib/brl/{bbas,bseg,bmvl}/foo/foo.h)
# Setting BRL_INCLUDE_DIR in this way allows #include <foo/foo.h> without
# having to know the internal directory structure.
# The root directory should be listed last in case anyone tries to append to this path
set( BRL_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bbas;${CMAKE_CURRENT_BINARY_DIR}/bbas;${CMAKE_CURRENT_SOURCE_DIR}/bseg;${CMAKE_CURRENT_SOURCE_DIR}/bpro;${CMAKE_CURRENT_SOURCE_DIR}" CACHE STATIC "Brown University package include files" FORCE )
set( BRL_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}")
set( BRL_INSTALL_INCLUDE_DIR
  ${CMAKE_INSTALL_PREFIX}/include/vxl/contrib/brl
  ${CMAKE_INSTALL_PREFIX}/include/vxl/contrib/brl/bbas
  ${CMAKE_INSTALL_PREFIX}/include/vxl/contrib/brl/bseg
  ${CMAKE_INSTALL_PREFIX}/include/vxl/contrib/brl/bpro
  CACHE INTERNAL "Brown University package include files"
)

# Many BRL modules require geotiff.  Include FindGEOTIFF once here.
if (VXL_USE_GEOTIFF)
  include( ${VXL_CMAKE_DIR}/FindGEOTIFF.cmake)
  if (GEOTIFF_FOUND)
    set(HAS_GEOTIFF 1 )
  endif()
endif()

# several BRL modules use OpenCL
find_package(OpenCL)
if(OpenCL_FOUND)

  # targeted OpenCL version
  if (VXL_TARGET_OPENCL_VERSION)
    set(CL_TARGET_OPENCL_VERSION ${VXL_TARGET_OPENCL_VERSION})
  else()
    set(CL_TARGET_OPENCL_VERSION "${OpenCL_VERSION_MAJOR}${OpenCL_VERSION_MINOR}0")
  endif()
  message(STATUS "BRL targeting OpenCL v${CL_TARGET_OPENCL_VERSION}")

  # compiler definitions & include
  add_definitions( -DHAS_OPENCL=1 )
  add_definitions( -DCL_TARGET_OPENCL_VERSION=${CL_TARGET_OPENCL_VERSION} )
  include_directories( ${OpenCL_INCLUDE_DIRS} )

endif()

# discover python libraries if requested
if(VXL_BUILD_BRL_PYTHON)
  find_package(Python3 COMPONENTS Development)
endif()

# Location of brl's private FindXXX.cmake CMake modules.
set( BRL_CMAKE_DIR ${CMAKE_CURRENT_LIST_DIR}/bmods )
# This is identical to BRL_CMAKE_DIR.  Perhaps we should eliminate BRL_MODULE_PATH?
set( BRL_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/bmods )

include_directories( ${BRL_INCLUDE_DIR} )

# 3rd party libraries. These are dependenices of other BRL libraries, so it needs
# to come before other add_subdirectories, or else they may be missing.
add_subdirectory(b3p)

# basic services package
add_subdirectory(bbas)

if( VXL_BUILD_GEL )
  # segmentation package
  add_subdirectory(bseg)
endif()

# process libraries
add_subdirectory(bpro)

# cvg library
add_subdirectory(cvg)

