#
# ./vxl/vil/CMakeLists.txt
#

ADD_LIBRARY(vil vil_sources)

SOURCE_FILES( vil_sources

# basic things
vil_pixel
vil_image_impl
vil_image
vil_stream
vil_file_format
vil_byte_swap
vil_16bit
vil_32bit
vil_test

# various vil_stream classes
# from a file
vil_stream_fstream
# in-core vil_stream
vil_stream_core
# present a section of a vil_stream
vil_stream_section
# load image from URL
vil_stream_url

# classes derived from vil_image
vil_file_image
vil_memory_image
#vil_memory_image_of
#vil_memory_image_of_format
vil_image_proxy

# image adaptors
vil_memory_image_impl
vil_crop_image_impl
vil_skip_image_impl
vil_resample_image_impl
vil_flipud_impl
vil_flip_components_impl
vil_block_cache_image_impl
vil_clamp_image_impl
# Helper specializations for vil_clamp_image_impl
vil_clamp_image
vil_scale_intensities_image_impl
# Helper specializations for vil_scale_intensities_image_impl
vil_scale_intensities_image

# file format readers/writers (see below for conditional ones)
file_formats/vil_gen
file_formats/vil_pnm
file_formats/vil_viff
file_formats/vil_viff_support
file_formats/vil_iris
file_formats/vil_mit
file_formats/vil_bmp_file_header
file_formats/vil_bmp_core_header
file_formats/vil_bmp_info_header
file_formats/vil_bmp
file_formats/vil_gif

# Operations
vil_image_as
vil_new
vil_load
vil_save
vil_copy
vil_crop
vil_skip
vil_resample
vil_warp
#vil_warp
vil_flipud
vil_flip_components
#vil_convolve
#vil_convolve_simple
#vil_convolve_1d
#vil_convolve_1d_x
#vil_convolve_1d_y
vil_smooth
vil_clamp
vil_scale_intensities
#vil_interpolate
#vil_resample_image
#vil_ncc
#vil_ssd

# Stuff
vil_pyramid
#vil_rgb
#vil_rgba
vil_rgb_byte
vil_colour_space
vil_memory_image_window

)

# vil_file_formats.cxx uses -DTIFF, etc, so we need to do this here.
#
INCLUDE(${CMAKE_SOURCE_DIR}/config.cmake/Modules/FindJPEG.cmake)
INCLUDE(${CMAKE_SOURCE_DIR}/config.cmake/Modules/FindPNG.cmake)
INCLUDE(${CMAKE_SOURCE_DIR}/config.cmake/Modules/FindTIFF.cmake)

IF(HAS_PNG)
  SOURCE_FILES( vil_sources
    file_formats/vil_png
  )
ENDIF(HAS_PNG)

IF(HAS_JPEG)
  SOURCE_FILES( vil_sources
    file_formats/vil_jpeg 
    file_formats/vil_jpeg_source_mgr
    file_formats/vil_jpeg_decompressor
    file_formats/vil_jpeg_destination_mgr
    file_formats/vil_jpeg_compressor )
ENDIF(HAS_JPEG)

IF(HAS_TIFF)
  SOURCE_FILES( vil_sources
    file_formats/vil_tiff
  )
ENDIF(HAS_TIFF)

AUX_SOURCE_DIRECTORY(Templates vil_sources)

#SUBDIRS(algo)
SUBDIRS(io)

IF( BUILD_TESTS)
  SUBDIRS(tests)
  SUBDIRS(examples)
ENDIF( BUILD_TESTS)
