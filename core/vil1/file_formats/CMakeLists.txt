#
# ./vxl/vil/file_formats/CMakeLists.txt
#
#   geoff.cross@crd.ge.com
#

ADD_LIBRARY(vil_file_formats vil_file_formats_sources)

SOURCE_FILES( vil_file_formats_sources
vil_bmp
vil_bmp_core_header
vil_bmp_file_header
vil_bmp_info_header
vil_gen
vil_gif
vil_iris
vil_mit
vil_pnm
vil_viff
vil_viff_support
)

INCLUDE(${CMAKE_SOURCE_DIR}/config.cmake/Modules/FindJPEG.cmake)
INCLUDE(${CMAKE_SOURCE_DIR}/config.cmake/Modules/FindPNG.cmake)
INCLUDE(${CMAKE_SOURCE_DIR}/config.cmake/Modules/FindTIFF.cmake)

IF(HAS_NATIVE_JPEG)

  INCLUDE_DIRECTORIES( ${NATIVE_JPEG_INCLUDE_PATH})
  SOURCE_FILES( vil_file_formats_sources 
                vil_jpeg 
                vil_jpeg_compressor
                vil_jpeg_decompressor
                vil_jpeg_destination_mgr
                vil_jpeg_source_mgr )

ELSE(HAS_NATIVE_JPEG)

  INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/v3p/jpeg)
  SOURCE_FILES( vil_file_formats_sources 
                vil_jpeg 
                vil_jpeg_compressor
                vil_jpeg_decompressor
                vil_jpeg_destination_mgr
                vil_jpeg_source_mgr )

ENDIF(HAS_NATIVE_JPEG)

IF(NATIVE_PNG_LIBRARY)

  INCLUDE_DIRECTORIES( ${NATIVE_PNG_INCLUDE_PATH} )
  SOURCE_FILES( vil_file_formats
    vil_png
  )

ELSE(NATIVE_PNG_LIBRARY)

  INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/v3p/png)
  SOURCE_FILES( vil_file_formats
    vil_png
  )

ENDIF(NATIVE_PNG_LIBRARY)

IF(NATIVE_TIFF_LIBRARY)

  INCLUDE_DIRECTORIES( ${NATIVE_TIFF_INCLUDE_PATH} )
  SOURCE_FILES( vil_file_formats
    vil_tiff
  )

ELSE(NATIVE_TIFF_LIBRARY)

  INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/v3p/tiff)
  SOURCE_FILES( vil_file_formats
    vil_tiff
  )

ENDIF(NATIVE_TIFF_LIBRARY)


#  currently not used:
# vil_viff
