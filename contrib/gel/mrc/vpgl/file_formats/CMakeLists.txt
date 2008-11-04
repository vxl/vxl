# gel/mrc/vpgl/file_formats/CMakeLists.txt
# various file_formats for camera parameter I/O
# file format could include image files with embedded camera models
# so make this libary separate to avoid cross linking unless desired
INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR}/mrc)

SET( vpgl_file_formats_sources
  vpgl_nitf_rational_camera.h  vpgl_nitf_rational_camera.cxx
)

INCLUDE(${MODULE_PATH}/FindTIFF.cmake)
IF(TIFF_FOUND)
  INCLUDE_DIRECTORIES(${TIFF_INCLUDE_DIR}) 
  INCLUDE( ${MODULE_PATH}/FindGEOTIFF.cmake)
  IF(GEOTIFF_FOUND)
    INCLUDE_DIRECTORIES(${GEOTIFF_INCLUDE_DIR}) 
    
    SET( vpgl_file_formats_sources ${vpgl_file_formats_sources}
         vpgl_geo_camera.h         vpgl_geo_camera.cxx )
         
  ENDIF(GEOTIFF_FOUND)
ENDIF(TIFF_FOUND)

ADD_LIBRARY(vpgl_file_formats ${vpgl_file_formats_sources})

TARGET_LINK_LIBRARIES(vpgl_file_formats vpgl vil)
