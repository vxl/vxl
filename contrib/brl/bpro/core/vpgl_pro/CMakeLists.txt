# brl/bpro/core/vpgl_pro/CMakeLists.txt
# Brown vpgl processes

include_directories( ${BRL_INCLUDE_DIR} )
include_directories( ${BRL_INCLUDE_DIR}/bpro )
include_directories( ${BRL_INCLUDE_DIR}/bpro/core )
include_directories( ${BRL_INCLUDE_DIR}/bseg )
include_directories( ${GEL_INCLUDE_DIR})
if(EXPAT_FOUND)
 if(EXPATPP_FOUND)
  include_directories(${EXPAT_INCLUDE_DIRS} )
  include_directories(${EXPATPP_INCLUDE_DIRS} )
 endif()
endif()
set(GEOTIFF_VALID FALSE)
include(${VXL_CMAKE_DIR}/FindTIFF.cmake)
if(TIFF_FOUND)
  include_directories(${TIFF_INCLUDE_DIR})
  include( ${VXL_CMAKE_DIR}/FindGEOTIFF.cmake)
  if(GEOTIFF_FOUND)
    include_directories(${GEOTIFF_INCLUDE_DIR})
    set(GEOTIFF_VALID TRUE)
 endif()
endif()
if(NOT GEOTIFF_VALID)
set( vpgl_pro_sources
  vpgl_processes.h
  vpgl_register.h      vpgl_register.cxx
)
endif()
if(GEOTIFF_VALID)
set( vpgl_pro_sources
  vpgl_processes.h
  vpgl_register.h      vpgl_register.cxx
  vpgl_dem_manager.h   vpgl_dem_manager.cxx
)
endif()
aux_source_directory(Templates vpgl_pro_sources)
aux_source_directory(processes vpgl_pro_sources)

vxl_add_library(LIBRARY_NAME vpgl_pro LIBRARY_SOURCES ${vpgl_pro_sources})
target_link_libraries(vpgl_pro bprb vsph bpgl bpgl_algo ${VXL_LIB_PREFIX}vpgl_io ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vpgl_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vbl_io ${VXL_LIB_PREFIX}vul bkml brad bbas_pro vsol)


if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
