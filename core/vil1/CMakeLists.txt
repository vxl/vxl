# vxl/vil1/CMakeLists.txt

set( vil1_sources
  vil1_fwd.h

  # basic things
  vil1_pixel.cxx                             vil1_pixel.h
  vil1_image_impl.cxx                        vil1_image_impl.h
  vil1_image.cxx                             vil1_image.h
  vil1_stream.cxx                            vil1_stream.h
  vil1_file_format.cxx                       vil1_file_format.h
  vil1_byte_swap.cxx                         vil1_byte_swap.h
  vil1_16bit.cxx                             vil1_16bit.h
  vil1_32bit.cxx                             vil1_32bit.h
  vil1_rgb.hxx                               vil1_rgb.h
  vil1_rgba.hxx                              vil1_rgba.h
  vil1_rgb_byte.h
  vil1_property.h

  # various vil1_stream classes
  # from a file
  vil1_stream_fstream.cxx                    vil1_stream_fstream.h
  # in-core vil1_stream
  vil1_stream_core.cxx                       vil1_stream_core.h
  # present a section of a vil1_stream
  vil1_stream_section.cxx                    vil1_stream_section.h

  # classes derived from vil1_image
  vil1_file_image.cxx                        vil1_file_image.h
  vil1_memory_image.cxx                      vil1_memory_image.h
  vil1_image_proxy.cxx                       vil1_image_proxy.h
  vil1_memory_image_of.hxx                   vil1_memory_image_of.h
  vil1_memory_image_of_format.hxx

  # image adaptors
  vil1_memory_image_impl.cxx                 vil1_memory_image_impl.h
  vil1_crop_image_impl.cxx                   vil1_crop_image_impl.h
  vil1_skip_image_impl.cxx                   vil1_skip_image_impl.h
  vil1_resample_image_impl.cxx               vil1_resample_image_impl.h
  vil1_flipud_impl.cxx                       vil1_flipud_impl.h
  vil1_flip_components_impl.cxx              vil1_flip_components_impl.h
  vil1_block_cache_image_impl.cxx            vil1_block_cache_image_impl.h
  vil1_clamp_image_impl.cxx                  vil1_clamp_image_impl.h
  # Helper specializations for vil1_clamp_image_impl
  vil1_clamp_image.cxx                       vil1_clamp_image.h
  vil1_scale_intensities_image_impl.cxx      vil1_scale_intensities_image_impl.h
  # Helper specializations for               vil1_scale_intensities_image_impl
  vil1_scale_intensities_image.cxx           vil1_scale_intensities_image.h
  # Wrapper classes for vil/vil1 conversion
  vil1_vil.h

  # file format readers/writers (see below for conditional ones)
  file_formats/vil1_gen.cxx                  file_formats/vil1_gen.h
  file_formats/vil1_pnm.cxx                  file_formats/vil1_pnm.h
  file_formats/vil1_viff.cxx                 file_formats/vil1_viff.h
  file_formats/vil1_viff_support.c           file_formats/vil1_viff_support.h file_formats/vil1_viffheader.h
  file_formats/vil1_iris.cxx                 file_formats/vil1_iris.h
  file_formats/vil1_mit.cxx                  file_formats/vil1_mit.h
  file_formats/vil1_bmp_file_header.cxx      file_formats/vil1_bmp_file_header.h
  file_formats/vil1_bmp_core_header.cxx      file_formats/vil1_bmp_core_header.h
  file_formats/vil1_bmp_info_header.cxx      file_formats/vil1_bmp_info_header.h
  file_formats/vil1_bmp.cxx                  file_formats/vil1_bmp.h
  file_formats/vil1_gif.cxx                  file_formats/vil1_gif.h
  file_formats/vil1_ras.cxx                  file_formats/vil1_ras.h

  # Operations
  vil1_image_as.cxx                          vil1_image_as.h
  vil1_new.cxx                               vil1_new.h
  vil1_open.cxx                              vil1_open.h
  vil1_load.cxx                              vil1_load.h
  vil1_save.cxx                              vil1_save.h
  vil1_copy.cxx                              vil1_copy.h
  vil1_crop.cxx                              vil1_crop.h
  vil1_skip.cxx                              vil1_skip.h
  vil1_resample.cxx                          vil1_resample.h
  vil1_warp.hxx  vil1_warp.cxx               vil1_warp.h
  vil1_flipud.cxx                            vil1_flipud.h
  vil1_flip_components.cxx                   vil1_flip_components.h
  vil1_smooth.cxx                            vil1_smooth.h
  vil1_clamp.cxx                             vil1_clamp.h
  vil1_scale_intensities.cxx                 vil1_scale_intensities.h
  vil1_convolve.hxx                          vil1_convolve.h
  vil1_convolve_1d.hxx
  vil1_convolve_1d_x.hxx
  vil1_convolve_1d_y.hxx
  vil1_convolve_simple.hxx                   vil1_convolve_simple.h
  vil1_interpolate.hxx                       vil1_interpolate.h
  vil1_ncc.hxx                               vil1_ncc.h
  vil1_resample_image.hxx                    vil1_resample_image.h
  vil1_ssd.hxx                               vil1_ssd.h

  # Stuff
  vil1_pyramid.cxx                           vil1_pyramid.h
  vil1_colour_space.cxx                      vil1_colour_space.h
  vil1_memory_image_window.cxx               vil1_memory_image_window.h
  vil1_ip_traits.h
)

# Some versions of Solaris (at least 5.8) has a brain-dead mechanism
# for implementing DNS services, where the user of a library that uses
# gethostbyname should link to -lnsl *before* linking to the
# library. This creates a kind of "pre-dependency" that the CMake
# dependency analysis is not equipped to handle. We cope by isolating
# the offending code into its own library vil1_network (under Solaris
# only, of course). We can then control the dependencies of vil1 to
# link nsl before this vil1_network.
#
set( vil1_network_sources
  # load image from URL
  vil1_stream_url.cxx                        vil1_stream_url.h
)

# vil1_stream_url not working on Apple MacOSX
# vil1_stream_url in separate library on Solaris
set(vil1_sources ${vil1_sources} ${vil1_network_sources})

# vil1_file_formats.cxx uses -DTIFF, etc, so we need to do this here.
include(${VXL_CMAKE_DIR}/FindJPEG.cmake)
include(${VXL_CMAKE_DIR}/FindPNG.cmake)
include(${VXL_CMAKE_DIR}/FindTIFF.cmake)

if(PNG_FOUND)
  include_directories( ${PNG_INCLUDE_DIR})
  add_definitions( ${PNG_DEFINITIONS}  -DHAS_PNG=1 )
  set( vil1_sources ${vil1_sources}
    file_formats/vil1_png.cxx                  file_formats/vil1_png.h
  )
else()
  add_definitions( -DHAS_PNG=0 )
endif()

if(JPEG_FOUND)
  include_directories( ${JPEG_INCLUDE_DIR} )
  add_definitions( ${JPEG_DEFINITIONS} -DHAS_JPEG=1 )
  set( vil1_sources ${vil1_sources}
    vil1_jpeglib.h
    file_formats/vil1_jpeg.cxx                  file_formats/vil1_jpeg.h
    file_formats/vil1_jpeg_source_mgr.cxx       file_formats/vil1_jpeg_source_mgr.h
    file_formats/vil1_jpeg_decompressor.cxx     file_formats/vil1_jpeg_decompressor.h
    file_formats/vil1_jpeg_destination_mgr.cxx  file_formats/vil1_jpeg_destination_mgr.h
    file_formats/vil1_jpeg_compressor.cxx       file_formats/vil1_jpeg_compressor.h
  )
else()
  add_definitions( -DHAS_JPEG=0 )
endif()

if(TIFF_FOUND)
  include_directories( ${TIFF_INCLUDE_DIR} )
  add_definitions( ${TIFF_DEFINITIONS} -DHAS_TIFF=1 )
  set( vil1_sources ${vil1_sources}
    file_formats/vil1_tiff.cxx                  file_formats/vil1_tiff.h
  )
else()
  add_definitions( -DHAS_TIFF=0 )
endif()

aux_source_directory(Templates vil1_sources)

if(CMAKE_COMPILER_IS_GNUCXX)
  set_source_files_properties(vil1_image_as.cxx PROPERTIES COMPILE_FLAGS -O0)
endif()

vxl_add_library(LIBRARY_NAME ${VXL_LIB_PREFIX}vil1 LIBRARY_SOURCES ${vil1_sources})

if(JPEG_FOUND)
  target_link_libraries( ${VXL_LIB_PREFIX}vil1 ${JPEG_LIBRARIES} )
endif()
if(TIFF_FOUND)
  target_link_libraries( ${VXL_LIB_PREFIX}vil1 ${TIFF_LIBRARIES} )
endif()
if(PNG_FOUND)
  target_link_libraries( ${VXL_LIB_PREFIX}vil1 ${PNG_LIBRARIES} )
endif()

target_link_libraries( ${VXL_LIB_PREFIX}vil1 ${VXL_LIB_PREFIX}vcl )

if(NOT UNIX)
  target_link_libraries( ${VXL_LIB_PREFIX}vil1 ws2_32 )
endif()

if( VXL_BUILD_EXAMPLES )
  add_subdirectory(examples)
endif()
if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
