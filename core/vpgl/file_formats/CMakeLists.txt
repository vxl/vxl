# core/vpgl/file_formats/CMakeLists.txt
# various file_formats for camera parameter I/O
# file format could include image files with embedded camera models
# so make this libary separate to avoid cross linking unless desired

SET( vpgl_file_formats_sources
  vpgl_nitf_rational_camera.h  vpgl_nitf_rational_camera.cxx
)

INCLUDE(${VXL_CMAKE_DIR}/FindTIFF.cmake)
IF(TIFF_FOUND)
  INCLUDE_DIRECTORIES(${TIFF_INCLUDE_DIR})
  INCLUDE( ${VXL_CMAKE_DIR}/FindGEOTIFF.cmake)
  IF(GEOTIFF_FOUND)
    INCLUDE_DIRECTORIES(${GEOTIFF_INCLUDE_DIR})

    SET( vpgl_file_formats_sources ${vpgl_file_formats_sources}
         vpgl_geo_camera.h         vpgl_geo_camera.cxx)

  ENDIF(GEOTIFF_FOUND)
ENDIF(TIFF_FOUND)

ADD_LIBRARY(vpgl_file_formats ${vpgl_file_formats_sources})

TARGET_LINK_LIBRARIES(vpgl_file_formats vpgl vil vnl)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

INSTALL_TARGETS(/lib vpgl_file_formats)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/core/vpgl/file_formats ${vpgl_file_formats_sources})
