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

IF(HAS_JPEG)
  SOURCE_FILES( vil_file_formats_sources 
                vil_jpeg 
                vil_jpeg_compressor
                vil_jpeg_decompressor
                vil_jpeg_destination_mgr
                vil_jpeg_source_mgr )
ENDIF(HAS_JPEG)

IF(HAS_PNG)
  SOURCE_FILES( vil_file_formats_sources
    vil_png
  )
ENDIF(HAS_PNG)

IF(HAS_TIFF)
  SOURCE_FILES( vil_file_formats_sources
    vil_tiff
  )
ENDIF(HAS_TIFF)
