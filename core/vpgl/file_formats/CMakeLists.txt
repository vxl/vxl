# core/vpgl/file_formats/CMakeLists.txt
# various file_formats for camera parameter I/O
# file format could include image files with embedded camera models
# so make this libary separate to avoid cross linking unless desired

set( vpgl_file_formats_sources
  vpgl_nitf_rational_camera.h  vpgl_nitf_rational_camera.cxx
)

include(${VXL_CMAKE_DIR}/FindTIFF.cmake)
if(TIFF_FOUND)
  include_directories(${TIFF_INCLUDE_DIR})
  include( ${VXL_CMAKE_DIR}/FindGEOTIFF.cmake)
  if(GEOTIFF_FOUND)
    include_directories(${GEOTIFF_INCLUDE_DIR})

    set( vpgl_file_formats_sources ${vpgl_file_formats_sources}
         vpgl_geo_camera.h         vpgl_geo_camera.cxx)

  endif()
endif()

vxl_add_library(LIBRARY_NAME ${VXL_LIB_PREFIX}vpgl_file_formats LIBRARY_SOURCES ${vpgl_file_formats_sources})

target_link_libraries(${VXL_LIB_PREFIX}vpgl_file_formats ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vnl)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
