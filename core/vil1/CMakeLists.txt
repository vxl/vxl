# ./vxl/vil/CMakeLists.txt

ADD_LIBRARY(vil vil_sources)

SOURCE_FILES( vil_sources

  # basic things
  vil_pixel.cxx
  vil_image_impl.cxx
  vil_image.cxx
  vil_stream.cxx
  vil_file_format.cxx
  vil_byte_swap.cxx
  vil_16bit.cxx
  vil_32bit.cxx

  # various vil_stream classes
  # from a file
  vil_stream_fstream.cxx
  # in-core vil_stream
  vil_stream_core.cxx
  # present a section of a vil_stream
  vil_stream_section.cxx
  # load image from URL
  vil_stream_url.cxx

  # classes derived from vil_image
  vil_file_image.cxx
  vil_memory_image.cxx
  vil_image_proxy.cxx

  # image adaptors
  vil_memory_image_impl.cxx
  vil_crop_image_impl.cxx
  vil_skip_image_impl.cxx
  vil_resample_image_impl.cxx
  vil_flipud_impl.cxx
  vil_flip_components_impl.cxx
  vil_block_cache_image_impl.cxx
  vil_clamp_image_impl.cxx
  # Helper specializations for vil_clamp_image_impl
  vil_clamp_image.cxx
  vil_scale_intensities_image_impl.cxx
  # Helper specializations for vil_scale_intensities_image_impl
  vil_scale_intensities_image.cxx

  # file format readers/writers (see below for conditional ones)
  file_formats/vil_gen.cxx
  file_formats/vil_pnm.cxx
  file_formats/vil_viff.cxx
  file_formats/vil_viff_support.c
  file_formats/vil_iris.cxx
  file_formats/vil_mit.cxx
  file_formats/vil_bmp_file_header.cxx
  file_formats/vil_bmp_core_header.cxx
  file_formats/vil_bmp_info_header.cxx
  file_formats/vil_bmp.cxx
  file_formats/vil_gif.cxx
  file_formats/vil_ras.cxx

  # Operations
  vil_image_as.cxx
  vil_new.cxx
  vil_load.cxx
  vil_save.cxx
  vil_copy.cxx
  vil_crop.cxx
  vil_skip.cxx
  vil_resample.cxx
  vil_warp.cxx
  vil_flipud.cxx
  vil_flip_components.cxx
  vil_smooth.cxx
  vil_clamp.cxx
  vil_scale_intensities.cxx

  # Stuff
  vil_pyramid.cxx
  vil_rgb_byte.cxx
  vil_colour_space.cxx
  vil_memory_image_window.cxx
)

# vil_file_formats.cxx uses -DTIFF, etc, so we need to do this here.
#
INCLUDE(${CMAKE_SOURCE_DIR}/config.cmake/Modules/FindJPEG.cmake)
INCLUDE(${CMAKE_SOURCE_DIR}/config.cmake/Modules/FindPNG.cmake)
INCLUDE(${CMAKE_SOURCE_DIR}/config.cmake/Modules/FindTIFF.cmake)

IF(HAS_PNG)
  SOURCE_FILES( vil_sources
    file_formats/vil_png.cxx
  )
ENDIF(HAS_PNG)

IF(HAS_JPEG)
  SOURCE_FILES( vil_sources
    file_formats/vil_jpeg.cxx
    file_formats/vil_jpeg_source_mgr.cxx
    file_formats/vil_jpeg_decompressor.cxx
    file_formats/vil_jpeg_destination_mgr.cxx
    file_formats/vil_jpeg_compressor.cxx
  )
ENDIF(HAS_JPEG)

IF(HAS_TIFF)
  SOURCE_FILES( vil_sources
    file_formats/vil_tiff.cxx
  )
ENDIF(HAS_TIFF)

AUX_SOURCE_DIRECTORY(Templates vil_sources)

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE(${allvxl_SOURCE_DIR}/vxl/vil/CMakeListsHeaders.txt)
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)

#SUBDIRS(algo)
SUBDIRS(io)

IF( BUILD_EXAMPLES )
  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )
IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
