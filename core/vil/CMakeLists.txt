# core/vil/CMakeLists.txt
project(vil)

doxygen_add_library(core/vil
  DEPENDS core/vnl core/vsl
  PACKAGE core
  DESCRIPTION "Core image library"
)

# By default, vil contains wrappers around various image readers.
# Setting this flag to OFF enables a version to be built which
# does not include any third-party I/O functions.
# This is useful when building APIs where the caller is responsible
# for loading/saving images, reducing the number of dependencies of the library.
option(VXL_VIL_INCLUDE_IMAGE_IO "Include image file format readers?" ON)
mark_as_advanced(VXL_VIL_INCLUDE_IMAGE_IO)

# List of all file_format reader sources.
# Collected into one list so that we can easily build a version
# without any image I/O if required (by setting the VXL_VIL_INCLUDE_IMAGE_IO to OFF)
set( vil_ff_sources
  # file format readers/writers (see below for conditional ones)
  file_formats/vil_pnm.cxx              file_formats/vil_pnm.h

  file_formats/vil_bmp_file_header.cxx  file_formats/vil_bmp_file_header.h
  file_formats/vil_bmp_core_header.cxx  file_formats/vil_bmp_core_header.h
  file_formats/vil_bmp_info_header.cxx  file_formats/vil_bmp_info_header.h
  file_formats/vil_bmp.cxx              file_formats/vil_bmp.h

  file_formats/vil_sgi.cxx              file_formats/vil_sgi.h
  file_formats/vil_sgi_file_header.cxx  file_formats/vil_sgi_file_header.h

  file_formats/vil_ras.cxx              file_formats/vil_ras.h

  file_formats/vil_iris.cxx             file_formats/vil_iris.h

  file_formats/vil_mit.cxx              file_formats/vil_mit.h


  file_formats/vil_viff.cxx             file_formats/vil_viff.h
  file_formats/vil_viffheader.cxx       file_formats/vil_viffheader.h

  ################################
  # NITF 2.0 and 2.1 reading code
  ################################
  # The actual vil_image_resource subclass
  file_formats/vil_nitf2_image.h                    file_formats/vil_nitf2_image.cxx

  # For reading the NITF file headers as well as the image headers
  file_formats/vil_nitf2_header.h                   file_formats/vil_nitf2_header.cxx
  file_formats/vil_nitf2_image_subheader.h          file_formats/vil_nitf2_image_subheader.cxx
  file_formats/vil_nitf2_des.h                      file_formats/vil_nitf2_des.cxx

  # Stuff shared by all NITF2 classes
  file_formats/vil_nitf2.h                          file_formats/vil_nitf2.cxx

  # Generic framework for reading NITF tagged record extensions (TREs)
  file_formats/vil_nitf2_array_field.h              file_formats/vil_nitf2_array_field.cxx
  file_formats/vil_nitf2_classification.h           file_formats/vil_nitf2_classification.cxx
  file_formats/vil_nitf2_compound_field_value.h     file_formats/vil_nitf2_compound_field_value.cxx
  file_formats/vil_nitf2_data_mask_table.h          file_formats/vil_nitf2_data_mask_table.cxx
  file_formats/vil_nitf2_field.h                    file_formats/vil_nitf2_field.cxx
  file_formats/vil_nitf2_field_definition.h         file_formats/vil_nitf2_field_definition.cxx
  file_formats/vil_nitf2_field_formatter.h          file_formats/vil_nitf2_field_formatter.cxx
  file_formats/vil_nitf2_field_functor.h            file_formats/vil_nitf2_field_functor.cxx
  file_formats/vil_nitf2_field_sequence.h           file_formats/vil_nitf2_field_sequence.cxx
  file_formats/vil_nitf2_index_vector.h
  file_formats/vil_nitf2_scalar_field.h             file_formats/vil_nitf2_scalar_field.cxx
  file_formats/vil_nitf2_tagged_record.h            file_formats/vil_nitf2_tagged_record.cxx
  file_formats/vil_nitf2_tagged_record_definition.h file_formats/vil_nitf2_tagged_record_definition.cxx
  file_formats/vil_nitf2_typed_array_field.h
  file_formats/vil_nitf2_typed_scalar_field.h
  file_formats/vil_nitf2_typed_field_formatter.h    file_formats/vil_nitf2_typed_field_formatter.cxx

  file_formats/vil_pyramid_image_list.h             file_formats/vil_pyramid_image_list.cxx
)

include(${VXL_CMAKE_DIR}/FindPNG.cmake)
if(PNG_FOUND)
  include_directories( ${PNG_INCLUDE_DIR})
  add_definitions( ${PNG_DEFINITIONS} )
  set( HAS_PNG 1 )
  set( vil_ff_sources ${vil_ff_sources}
    file_formats/vil_png.cxx file_formats/vil_png.h
  )
else()
  set( HAS_PNG 0 )
endif()

include(${VXL_CMAKE_DIR}/FindJPEG.cmake)
if(JPEG_FOUND)
  include_directories( ${JPEG_INCLUDE_DIR} )
  set( HAS_JPEG 1 )
  set( vil_ff_sources ${vil_ff_sources}
    file_formats/vil_jpeglib.h
    file_formats/vil_jpeg.cxx                 file_formats/vil_jpeg.h
    file_formats/vil_jpeg_source_mgr.cxx      file_formats/vil_jpeg_source_mgr.h
    file_formats/vil_jpeg_decompressor.cxx    file_formats/vil_jpeg_decompressor.h
    file_formats/vil_jpeg_destination_mgr.cxx file_formats/vil_jpeg_destination_mgr.h
    file_formats/vil_jpeg_compressor.cxx      file_formats/vil_jpeg_compressor.h
  )
else()
  set( HAS_JPEG 0 )
endif()

################################
# JPEG 2000
################################
include( ${VXL_CMAKE_DIR}/NewCMake/FindECW.cmake )
if(ECW_FOUND)
  set( vil_ff_sources ${vil_ff_sources}
    file_formats/vil_j2k_image.h           file_formats/vil_j2k_image.cxx
    file_formats/NCSJPCVilIOStream.h       file_formats/NCSJPCVilIOStream.cxx
    file_formats/vil_j2k_nitf2_pyramid_image_resource.h
    file_formats/vil_j2k_nitf2_pyramid_image_resource.cxx
    file_formats/vil_j2k_pyramid_image_resource.h
    file_formats/vil_j2k_pyramid_image_resource.cxx
  )
  include_directories(${ECW_INCLUDE_DIR})
  set( HAS_J2K 1 )
else()
  set( HAS_J2K 0 )
endif()

##############################
# JPEG2000 via OpenJPEG
##############################
include( ${VXL_CMAKE_DIR}/FindOpenJPEG2.cmake )
if(OPENJPEG2_FOUND)
  include_directories(${OPENJPEG2_INCLUDE_DIR})
  add_definitions( ${OPENJPEG2_DEFINITIONS} )
  set( vil_ff_sources ${vil_ff_sources}
    file_formats/vil_openjpeg.h  file_formats/vil_openjpeg.cxx
    file_formats/vil_openjpeg_pyramid_image_resource.h
    file_formats/vil_openjpeg_pyramid_image_resource.cxx
  )
  add_definitions( ${OPENJPEG2_DEFINITIONS} )
  set( HAS_OPENJPEG2 1 )
else()
  set( HAS_OPENJPEG2 0 )
endif()

set( HAS_TIFF 0 )
set( HAS_GEOTIFF 0 )
include(${VXL_CMAKE_DIR}/FindTIFF.cmake)
if(TIFF_FOUND)
  include_directories(${TIFF_INCLUDE_DIR})
  set( HAS_TIFF 1 )
  set( vil_ff_sources ${vil_ff_sources}
    file_formats/vil_tiff.cxx file_formats/vil_tiff.h
    file_formats/vil_tiff_header.cxx file_formats/vil_tiff_header.h
  )

  option(VXL_USE_GEOTIFF "Use the GEOTIFF Library if available" True)
  mark_as_advanced(VXL_USE_GEOTIFF)
  include( ${VXL_CMAKE_DIR}/FindGEOTIFF.cmake)
  if(GEOTIFF_FOUND)
    set(HAS_GEOTIFF 1 )
    include_directories(${GEOTIFF_INCLUDE_DIR})
    set( vil_ff_sources ${vil_ff_sources}
      file_formats/vil_geotiff_header.cxx file_formats/vil_geotiff_header.h
    )
  endif()
endif()

# Let users decide if they should include DCMTK since we don't build against all versions.
option(VXL_USE_DCMTK "" TRUE)
mark_as_advanced(VXL_USE_DCMTK)
set( HAS_DCMTK 0 )
if (VXL_USE_DCMTK)
  include(${VXL_CMAKE_DIR}/FindDCMTK.cmake)
  if(DCMTK_FOUND)
    include_directories(${DCMTK_INCLUDE_DIR})
    set( HAS_DCMTK 1 )
    set( vil_ff_sources ${vil_ff_sources}
      file_formats/vil_dicom.cxx        file_formats/vil_dicom.h
      file_formats/vil_dicom_stream.cxx file_formats/vil_dicom_stream.h
      file_formats/vil_dicom_header.cxx file_formats/vil_dicom_header.h
      )
  endif()
endif()

option(VIL_CONFIG_ENABLE_SSE2_ROUNDING
       "Enable Streaming SIMD Extensions 2 implementation of rounding (hardware dependant)."
       ${VXL_HAS_SSE2_HARDWARE_SUPPORT} )
mark_as_advanced( VIL_CONFIG_ENABLE_SSE2_ROUNDING )
if( VIL_CONFIG_ENABLE_SSE2_ROUNDING )
  if( NOT VXL_HAS_SSE2_HARDWARE_SUPPORT )
    if( VXL_SSE2_HARDWARE_SUPPORT_POSSIBLE )
      message( ${VXL_SSE2_HARDWARE_SUPPORT_POSSIBLE_HELP} )
    endif()
    message( SEND_ERROR "Cannot have VIL_CONFIG_ENABLE_SSE2_ROUNDING because"
                        " there is no SSE2 hardware or no compiler support enabled" )
    set(VIL_CONFIG_ENABLE_SSE2_ROUNDING 0)
  endif()
endif()


if(VIL_CONFIG_ENABLE_SSE2_ROUNDING)
  set(VIL_CONFIG_ENABLE_SSE2_ROUNDING 1)
else()
  set(VIL_CONFIG_ENABLE_SSE2_ROUNDING 0)
endif()


# Store the external library configurations in to the config file
vxl_configure_file( ${CMAKE_CURRENT_LIST_DIR}/vil_config.h.in
                    ${PROJECT_BINARY_DIR}/vil_config.h include/vxl/core/vil)


# Create empty list
set( vil_sources )

if (VXL_VIL_INCLUDE_IMAGE_IO)
  set( vil_sources ${vil_ff_sources} )
endif()

set( vil_sources ${vil_sources}
  # Basic things
  vil_memory_chunk.cxx                  vil_memory_chunk.h
  vil_image_view_base.h
  vil_chord.h
  vil_image_view.h                      vil_image_view.hxx
  vil_image_resource.cxx                vil_image_resource.h
                                        vil_image_resource_sptr.h
  vil_blocked_image_resource.cxx        vil_blocked_image_resource.h
                                        vil_blocked_image_resource_sptr.h
  vil_blocked_image_facade.cxx          vil_blocked_image_facade.h
  vil_file_format.cxx                   vil_file_format.h
  vil_memory_image.cxx                  vil_memory_image.h
  vil_block_cache.cxx                   vil_block_cache.h
  vil_cached_image_resource.cxx         vil_cached_image_resource.h
  vil_pyramid_image_resource.cxx        vil_pyramid_image_resource.h
                                        vil_pyramid_image_resource_sptr.h
  vil_pyramid_image_view.hxx            vil_pyramid_image_view.h
  vil_image_list.cxx                    vil_image_list.h

  # image operations
  vil_crop.cxx                          vil_crop.h
  vil_clamp.cxx                         vil_clamp.h
  vil_transpose.cxx                     vil_transpose.h
  vil_flip.cxx                          vil_flip.h
  vil_plane.cxx                         vil_plane.h
  vil_math.cxx                          vil_math.h
  vil_view_as.h
  vil_convert.h
  vil_fill.h
  vil_transform.h
  vil_decimate.cxx                      vil_decimate.h
  vil_load.cxx                          vil_load.h
  vil_save.cxx                          vil_save.h
  vil_copy.hxx       vil_copy.cxx       vil_copy.h
  vil_new.cxx                           vil_new.h
  vil_print.cxx                         vil_print.h
  vil_warp.h
  vil_flatten.h

  # Bilinear Sampling Operations
  vil_bilin_interp.h
  vil_sample_profile_bilin.hxx          vil_sample_profile_bilin.h
  vil_sample_grid_bilin.hxx             vil_sample_grid_bilin.h
  vil_resample_bilin.hxx                vil_resample_bilin.h
  vil_rotate.hxx                        vil_rotate.h

  # Bicubic Sampling Operations
  vil_bicub_interp.hxx                  vil_bicub_interp.h
  vil_sample_profile_bicub.hxx          vil_sample_profile_bicub.h
  vil_sample_grid_bicub.hxx             vil_sample_grid_bicub.h
  vil_resample_bicub.hxx                vil_resample_bicub.h

  # Nearest Neighbour Sampling Operations
  vil_nearest_interp.h
  vil_resample_nearest.hxx              vil_resample_nearest.h

  # Stuff
  vil_border.h
  vil_smart_ptr.hxx                     vil_smart_ptr.h
  vil_property.h
  vil_pixel_format.cxx                  vil_pixel_format.h
  vil_exception.h
  vil_rgb.hxx                           vil_rgb.h
  vil_rgba.hxx                          vil_rgba.h
  vil_color_table.h                        vil_color_table.cxx
  vil_fwd.h
  vil_round.h
  vil_na.cxx                            vil_na.h

  # Streams
  vil_stream.cxx                        vil_stream.h
  vil_stream_fstream.cxx                vil_stream_fstream.h
  vil_stream_core.cxx                   vil_stream_core.h
  vil_stream_section.cxx                vil_stream_section.h
  vil_open.cxx                          vil_open.h
  vil_stream_read.cxx                   vil_stream_read.h
  vil_stream_write.cxx                  vil_stream_write.h

  # Plugin for image loading
  vil_image_resource_plugin.cxx         vil_image_resource_plugin.h
)
if ( VXL_HAS_SSE2_HARDWARE_SUPPORT )
  set( vil_sources ${vil_sources}
    vil_math_sse.h vil_math_sse.hxx
    )
endif ()

if(WIN32 AND VXL_USE_LFS AND NOT CMAKE_CL_64)
#This is a hack since win32 doesn't have transparent Large File Support.
  add_definitions( -DVIL_USE_FSTREAM64 )
  set( vil_sources ${vil_sources}
    vil_stream_fstream64.cxx        vil_stream_fstream64.h
  )
endif()

aux_source_directory(Templates vil_sources)

if(CMAKE_COMPILER_IS_GNUCXX)
  set_source_files_properties(vil_na.cxx PROPERTIES COMPILE_FLAGS -O1)
endif()


# Some versions of Solaris (at least 5.8) has a brain-dead mechanism
# for implementing DNS services, where the user of a library that uses
# gethostbyname should link to -lnsl *before* linking to the
# library. This creates a kind of "pre-dependency" that the CMake
# dependency analysis is not equipped to handle. We cope by isolating
# the offending code into its own library vil_network (under Solaris
# only, of course). We can then control the dependencies of vil to
# link nsl before this vil_network.
#
# This workaround also occurs in vul
#
set( vil_network_sources
  # load image from URL
  vil_stream_url.cxx vil_stream_url.h
)

vxl_add_library(LIBRARY_NAME ${VXL_LIB_PREFIX}vil LIBRARY_SOURCES ${vil_sources} ${vil_network_sources})

if(JPEG_FOUND)
  target_link_libraries( ${VXL_LIB_PREFIX}vil ${JPEG_LIBRARIES} )
endif()
if(TIFF_FOUND)
  target_link_libraries( ${VXL_LIB_PREFIX}vil ${TIFF_LIBRARIES})
endif()
if(HAS_GEOTIFF)
  target_link_libraries( ${VXL_LIB_PREFIX}vil ${GEOTIFF_LIBRARIES})
endif()
if(PNG_FOUND)
  target_link_libraries( ${VXL_LIB_PREFIX}vil ${PNG_LIBRARIES} )
endif()
if(DCMTK_FOUND)
  target_link_libraries( ${VXL_LIB_PREFIX}vil ${DCMTK_LIBRARIES} )
endif()
if(OPENJPEG2_FOUND)
  target_link_libraries( ${VXL_LIB_PREFIX}vil ${OPENJPEG2_LIBRARIES} )
endif()

target_link_libraries( ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vcl )

if(NOT UNIX)
  target_link_libraries( ${VXL_LIB_PREFIX}vil ws2_32 )
endif()

if(ECW_FOUND)
  target_link_libraries( ${VXL_LIB_PREFIX}vil ${ECW_LIBRARIES} )
endif()
if(OPENJPEG2_FOUND)
  target_link_libraries( ${VXL_LIB_PREFIX}vil ${OPENJPEG2_LIBRARIES} )
endif()

# Examples and tests use a lot of image I/O
if( VXL_BUILD_EXAMPLES AND VXL_VIL_INCLUDE_IMAGE_IO)
  add_subdirectory(examples)
endif()
if( BUILD_TESTING AND VXL_VIL_INCLUDE_IMAGE_IO)
  add_subdirectory(tests)
endif()

if (VXL_BUILD_OBJECT_LIBRARIES)
  add_library(vil-obj OBJECT ${vil_sources})
endif()
