# vxl/vil/CMakeLists.txt


# vil_file_formats.cxx uses -DTIFF, etc, so we need to do this here.
#
INCLUDE(${MODULE_PATH}/FindJPEG.cmake)
INCLUDE(${MODULE_PATH}/FindPNG.cmake)
INCLUDE(${MODULE_PATH}/FindTIFF.cmake)

IF(HAS_PNG)
  INCLUDE_DIRECTORIES( ${PNG_INCLUDE_PATH} )
  SET( vil_sources ${vil_sources}
    file_formats/vil_png.cxx file_formats/vil_png.h
  )
ENDIF(HAS_PNG)

IF(JPEG_FOUND)
  INCLUDE_DIRECTORIES( ${JPEG_INCLUDE_PATH} )
  ADD_DEFINITIONS( ${JPEG_DEFINITIONS} )
  SET( vil_sources ${vil_sources}
    vil_jpeglib.h
    file_formats/vil_jpeg.cxx file_formats/vil_jpeg.h
    file_formats/vil_jpeg_source_mgr.cxx file_formats/vil_jpeg_source_mgr.h
    file_formats/vil_jpeg_decompressor.cxx file_formats/vil_jpeg_decompressor.h
    file_formats/vil_jpeg_destination_mgr.cxx file_formats/vil_jpeg_destination_mgr.h
    file_formats/vil_jpeg_compressor.cxx file_formats/vil_jpeg_compressor.h
  )
ENDIF(JPEG_FOUND)

IF(HAS_TIFF)
  INCLUDE_DIRECTORIES( ${TIFF_INCLUDE_PATH} )
  SET( vil_sources ${vil_sources}
    file_formats/vil_tiff.cxx file_formats/vil_tiff.h
  )
ENDIF(HAS_TIFF)



SET( vil_sources
  vil_fwd.h

  # basic things
  vil_pixel.cxx vil_pixel.h
  vil_image_impl.cxx vil_image_impl.h
  vil_image.cxx vil_image.h
  vil_stream.cxx vil_stream.h
  vil_file_format.cxx vil_file_format.h
  vil_byte.h
  vil_byte_swap.cxx vil_byte_swap.h
  vil_16bit.cxx vil_16bit.h
  vil_32bit.cxx vil_32bit.h
  vil_rgb.txx vil_rgb.h
  vil_rgba.txx vil_rgba.h
  vil_rgb_byte.cxx vil_rgb_byte.h
  vil_property.h

  # various vil_stream classes
  # from a file
  vil_stream_fstream.cxx vil_stream_fstream.h
  # in-core vil_stream
  vil_stream_core.cxx vil_stream_core.h
  # present a section of a vil_stream
  vil_stream_section.cxx vil_stream_section.h
  # load image from URL
  vil_stream_url.cxx vil_stream_url.h

  # classes derived from vil_image
  vil_file_image.cxx vil_file_image.h
  vil_memory_image.cxx vil_memory_image.h
  vil_image_proxy.cxx vil_image_proxy.h
  vil_memory_image_of.txx vil_memory_image_of.h
  vil_memory_image_of_format.txx

  # image adaptors
  vil_memory_image_impl.cxx vil_memory_image_impl.h
  vil_crop_image_impl.cxx vil_crop_image_impl.h
  vil_skip_image_impl.cxx vil_skip_image_impl.h
  vil_resample_image_impl.cxx vil_resample_image_impl.h
  vil_flipud_impl.cxx vil_flipud_impl.h
  vil_flip_components_impl.cxx vil_flip_components_impl.h
  vil_block_cache_image_impl.cxx vil_block_cache_image_impl.h
  vil_clamp_image_impl.cxx vil_clamp_image_impl.h
  # Helper specializations for vil_clamp_image_impl
  vil_clamp_image.cxx vil_clamp_image.h
  vil_scale_intensities_image_impl.cxx vil_scale_intensities_image_impl.h
  # Helper specializations for vil_scale_intensities_image_impl
  vil_scale_intensities_image.cxx vil_scale_intensities_image.h

  # file format readers/writers (see below for conditional ones)
  file_formats/vil_gen.cxx file_formats/vil_gen.h
  file_formats/vil_pnm.cxx file_formats/vil_pnm.h
  file_formats/vil_viff.cxx file_formats/vil_viff.h
  file_formats/vil_viff_support.c file_formats/vil_viffheader.h
  file_formats/vil_iris.cxx file_formats/vil_iris.h
  file_formats/vil_mit.cxx file_formats/vil_mit.h
  file_formats/vil_bmp_file_header.cxx file_formats/vil_bmp_file_header.h
  file_formats/vil_bmp_core_header.cxx file_formats/vil_bmp_core_header.h
  file_formats/vil_bmp_info_header.cxx file_formats/vil_bmp_info_header.h
  file_formats/vil_bmp.cxx file_formats/vil_bmp.h
  file_formats/vil_gif.cxx file_formats/vil_gif.h
  file_formats/vil_ras.cxx file_formats/vil_ras.h

  # Operations
  vil_image_as.cxx vil_image_as.h
  vil_new.cxx vil_new.h
  vil_open.cxx vil_open.h
  vil_load.cxx vil_load.h
  vil_save.cxx vil_save.h
  vil_copy.cxx vil_copy.h
  vil_crop.cxx vil_crop.h
  vil_skip.cxx vil_skip.h
  vil_resample.cxx vil_resample.h
  vil_warp.txx vil_warp.h vil_warp.cxx
  vil_flipud.cxx vil_flipud.h
  vil_flip_components.cxx vil_flip_components.h
  vil_smooth.cxx vil_smooth.h
  vil_clamp.cxx vil_clamp.h
  vil_scale_intensities.cxx vil_scale_intensities.h
  vil_convolve.txx vil_convolve.h
  vil_convolve_1d.txx
  vil_convolve_1d_x.txx
  vil_convolve_1d_y.txx
  vil_convolve_simple.txx vil_convolve_simple.h
  vil_interpolate.txx vil_interpolate.h
  vil_ncc.txx vil_ncc.h
  vil_resample_image.txx vil_resample_image.h
  vil_ssd.txx vil_ssd.h

  # Stuff
  vil_pyramid.cxx vil_pyramid.h
  vil_colour_space.cxx vil_colour_space.h
  vil_memory_image_window.cxx vil_memory_image_window.h
  vil_ip_traits.h
)


AUX_SOURCE_DIRECTORY(Templates vil_sources)

ADD_LIBRARY(vil ${vil_sources})

IF(HAS_JPEG)
  TARGET_LINK_LIBRARIES( vil ${JPEG_LIBRARIES} )
ENDIF(HAS_JPEG)
IF(HAS_TIFF)
  TARGET_LINK_LIBRARIES( vil ${TIFF_LIBRARIES} )
ENDIF(HAS_TIFF)
IF(HAS_PNG)
  TARGET_LINK_LIBRARIES( vil ${PNG_LIBRARIES} )
ENDIF(HAS_PNG)

TARGET_LINK_LIBRARIES( vil vcl )

IF (NOT UNIX)
  TARGET_LINK_LIBRARIES( vil ws2_32.lib )
ENDIF (NOT UNIX)

IF (SOLARIS)
  TARGET_LINK_LIBRARIES( vil socket )
ENDIF (SOLARIS)

SUBDIRS(io)

IF( BUILD_EXAMPLES )
  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )
IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
