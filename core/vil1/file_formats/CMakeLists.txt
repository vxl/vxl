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
)

INCLUDE(${CMAKE_SOURCE_DIR}/CMake/Modules/FindJPEG.cmake)

IF(NATIVE_JPEG_INCLUDE_PATH)

  INCLUDE_DIRECTORIES( ${NATIVE_JPEG_INCLUDE_PATH})
  SOURCE_FILES( vil_file_formats_sources 
                vil_jpeg 
                vil_jpeg_compressor
                vil_jpeg_decompressor
                vil_jpeg_destination_mgr
                vil_jpeg_source_mgr )

ELSE(NATIVE_JPEG_INCLUDE_PATH)

  INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/v3p/jpeg)
  SOURCE_FILES( vil_file_formats_sources 
                vil_jpeg 
                vil_jpeg_compressor
                vil_jpeg_decompressor
                vil_jpeg_destination_mgr
                vil_jpeg_source_mgr )

ENDIF(NATIVE_JPEG_INCLUDE_PATH)

#  currently not used:
# vil_png
# vil_tiff
# vil_viff
