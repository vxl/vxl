# core/vil/CMakeLists.txt

INCLUDE(${MODULE_PATH}/FindPNG.cmake)
IF(PNG_FOUND)
  INCLUDE_DIRECTORIES( ${PNG_INCLUDE_DIR})
  ADD_DEFINITIONS( ${PNG_DEFINITIONS} )
  SET( HAS_PNG 1 )
  SET( vil_sources ${vil_sources}
    file_formats/vil_png.cxx file_formats/vil_png.h
  )
ELSE(PNG_FOUND)
  SET( HAS_PNG 0 )
ENDIF(PNG_FOUND)

INCLUDE(${MODULE_PATH}/FindJPEG.cmake)
IF(JPEG_FOUND)
  INCLUDE_DIRECTORIES( ${JPEG_INCLUDE_DIR} )
  SET( HAS_JPEG 1 )
  SET( vil_sources ${vil_sources}
    file_formats/vil_jpeglib.h
    file_formats/vil_jpeg.cxx                 file_formats/vil_jpeg.h
    file_formats/vil_jpeg_source_mgr.cxx      file_formats/vil_jpeg_source_mgr.h
    file_formats/vil_jpeg_decompressor.cxx    file_formats/vil_jpeg_decompressor.h
    file_formats/vil_jpeg_destination_mgr.cxx file_formats/vil_jpeg_destination_mgr.h
    file_formats/vil_jpeg_compressor.cxx      file_formats/vil_jpeg_compressor.h
  )
ELSE(JPEG_FOUND)
  SET( HAS_JPEG 0 )
ENDIF(JPEG_FOUND)

INCLUDE(${MODULE_PATH}/FindTIFF.cmake)
IF(TIFF_FOUND)
  INCLUDE_DIRECTORIES(${TIFF_INCLUDE_DIR})
  SET( HAS_TIFF 1 )
  SET( vil_sources ${vil_sources}
    file_formats/vil_tiff.cxx file_formats/vil_tiff.h
  )
ELSE(TIFF_FOUND)
  SET( HAS_TIFF 0 )
ENDIF(TIFF_FOUND)

INCLUDE(${MODULE_PATH}/FindDCMTK.cmake)
IF(DCMTK_FOUND)
  INCLUDE_DIRECTORIES(${DCMTK_INCLUDE_DIR})
  SET( HAS_DCMTK 1 )
  SET( vil_sources ${vil_sources}
    file_formats/vil_dicom2.cxx        file_formats/vil_dicom2.h
    file_formats/vil_dicom2_stream.cxx file_formats/vil_dicom2_stream.h
  )
ELSE(DCMTK_FOUND)
  SET( HAS_DCMTK 0 )
ENDIF(DCMTK_FOUND)

# Store the external library configurations in to the config file
CONFIGURE_FILE( ${vxl_SOURCE_DIR}/core/vil/vil_config.h.in
                ${vxl_BINARY_DIR}/core/vil/vil_config.h @ONLY IMMEDIATE )
  

SET( vil_sources ${vil_sources}

  # basic things
  vil_memory_chunk.cxx                  vil_memory_chunk.h
  vil_image_view_base.cxx               vil_image_view_base.h
  vil_image_view.h                      vil_image_view.txx
  vil_image_resource.cxx                vil_image_resource.h
                                        vil_image_resource_sptr.h

  vil_file_format.cxx                   vil_file_format.h
  vil_memory_image.cxx                  vil_memory_image.h

  # file format readers/writers (see above for conditional ones)
  file_formats/vil_pnm.cxx              file_formats/vil_pnm.h

  file_formats/vil_bmp_file_header.cxx  file_formats/vil_bmp_file_header.h
  file_formats/vil_bmp_core_header.cxx  file_formats/vil_bmp_core_header.h
  file_formats/vil_bmp_info_header.cxx  file_formats/vil_bmp_info_header.h
  file_formats/vil_bmp.cxx              file_formats/vil_bmp.h

  file_formats/vil_ras.cxx              file_formats/vil_ras.h

  file_formats/vil_iris.cxx             file_formats/vil_iris.h

  file_formats/vil_mit.cxx              file_formats/vil_mit.h

  file_formats/vil_dicom.cxx            file_formats/vil_dicom.h
  file_formats/vil_dicom_header.cxx     file_formats/vil_dicom_header.h

  file_formats/vil_viff.cxx             file_formats/vil_viff.h
  file_formats/vil_viff_support.c       file_formats/vil_viff_support.h
  file_formats/vil_viffheader.h

  file_formats/vil_nitf.cxx             file_formats/vil_nitf.h
  file_formats/vil_nitf_blocking_info.h
  file_formats/vil_nitf_extended_header.cxx     file_formats/vil_nitf_extended_header.h
  file_formats/vil_nitf_header.cxx              file_formats/vil_nitf_header.h
  file_formats/vil_nitf_header_v20.h
  file_formats/vil_nitf_image_subheader.cxx     file_formats/vil_nitf_image_subheader.h
  file_formats/vil_nitf_image_subheader_v20.cxx file_formats/vil_nitf_image_subheader_v20.h
  file_formats/vil_nitf_message_header.cxx      file_formats/vil_nitf_message_header.h
  file_formats/vil_nitf_message_header_v20.cxx  file_formats/vil_nitf_message_header_v20.h
  file_formats/vil_nitf_version.cxx             file_formats/vil_nitf_version.h
  file_formats/vil_nitf_version_v20.cxx         file_formats/vil_nitf_version_v20.h
  file_formats/vil_nitf_macro_defs.h
  file_formats/vil_nitf_typeinfo.h
  file_formats/vil_nitf_util.cxx        file_formats/vil_nitf_util.h

  # image operations
  vil_crop.cxx                          vil_crop.h
  vil_clamp.cxx                         vil_clamp.h
  vil_transpose.cxx                     vil_transpose.h
  vil_flip.cxx                          vil_flip.h
  vil_plane.cxx                         vil_plane.h
  vil_math.h
  vil_view_as.h
  vil_convert.h
  vil_fill.h
  vil_transform.h
  vil_decimate.cxx                      vil_decimate.h
  vil_load.cxx                          vil_load.h
  vil_save.cxx                          vil_save.h
  vil_copy.txx       vil_copy.cxx       vil_copy.h
  vil_new.cxx                           vil_new.h
  vil_print.cxx                         vil_print.h
  vil_warp.h

  # Bilinear Sampling Operations
  vil_bilin_interp.h
  vil_sample_profile_bilin.txx          vil_sample_profile_bilin.h
  vil_sample_grid_bilin.txx             vil_sample_grid_bilin.h
  vil_resample_bilin.txx                vil_resample_bilin.h
  vil_rotate.txx                        vil_rotate.h

  # Bicubic Sampling Operations
  vil_bicub_interp.txx                  vil_bicub_interp.h
  vil_sample_profile_bicub.txx          vil_sample_profile_bicub.h
  vil_sample_grid_bicub.txx             vil_sample_grid_bicub.h
  vil_resample_bicub.txx                vil_resample_bicub.h

  # Nearest Neighbour Sampling Operations
  vil_nearest_neighbour_interp.h

  # Stuff
  vil_smart_ptr.txx                     vil_smart_ptr.h
  vil_property.h
  vil_pixel_format.cxx                  vil_pixel_format.h
  vil_pixel_traits.h
  vil_rgb.txx                           vil_rgb.h
  vil_rgba.txx                          vil_rgba.h
  vil_fwd.h

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

AUX_SOURCE_DIRECTORY(Templates vil_sources)

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
SET( vil_network_sources
  # load image from URL
  vil_stream_url.cxx vil_stream_url.h
)

# vil_stream_url not working on Apple MacOSX
IF(APPLE)
  ADD_LIBRARY(vil ${vil_sources})
ELSE(APPLE)
  IF(NOT SOLARIS)
    ADD_LIBRARY(vil ${vil_sources} ${vil_network_sources})
  ELSE(NOT SOLARIS)
    ADD_LIBRARY(vil ${vil_sources})
    ADD_LIBRARY(vil_network ${vil_network_sources})
  ENDIF(NOT SOLARIS)
ENDIF(APPLE)

IF(JPEG_FOUND)
  TARGET_LINK_LIBRARIES( vil ${JPEG_LIBRARIES} )
ENDIF(JPEG_FOUND)
IF(TIFF_FOUND)
  TARGET_LINK_LIBRARIES( vil ${TIFF_LIBRARIES} )
ENDIF(TIFF_FOUND)
IF(PNG_FOUND)
  TARGET_LINK_LIBRARIES( vil ${PNG_LIBRARIES} )
ENDIF(PNG_FOUND)
IF(DCMTK_FOUND)
  TARGET_LINK_LIBRARIES( vil ${DCMTK_LIBRARIES} )
ENDIF(DCMTK_FOUND)

TARGET_LINK_LIBRARIES( vil vcl )

IF (NOT UNIX)
  TARGET_LINK_LIBRARIES( vil ws2_32 )
ENDIF (NOT UNIX)

IF (SOLARIS)
  TARGET_LINK_LIBRARIES( vil nsl vil_network socket )
ENDIF (SOLARIS)

SUBDIRS(algo)

IF( BUILD_EXAMPLES )
  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )
IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
