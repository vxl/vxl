USES := vxl mul
TRY_JPEG := 1

LIBRARY := vil2
VERSION := 1.0

SOURCES += vil2_image_resource.cxx
MANPAGE_SOURCES += vil2_image_view.txx
SOURCES += vil2_file_format.cxx
SOURCES += vil2_pixel_format.cxx
MANPAGE_SOURCES += vil2_smart_ptr.txx
SOURCES += vil2_memory_chunk.cxx
SOURCES += vil2_image_view_base.cxx
SOURCES += vil2_load.cxx
SOURCES += vil2_crop.cxx
SOURCES += vil2_memory_image.cxx
SOURCES += vil2_new.cxx
SOURCES += vil2_save.cxx
MANPAGE_SOURCES += vil2_bilin_interp.h
MANPAGE_SOURCES += vil2_sample_profile_bilin.txx
MANPAGE_SOURCES += vil2_sample_grid_bilin.txx
MANPAGE_SOURCES += vil2_copy.txx
MANPAGE_SOURCES += vil2_math.h
MANPAGE_SOURCES += vil2_property.h
SOURCES += vil2_clamp.cxx
SOURCES += vil2_transpose.cxx
SOURCES += vil2_flip.cxx
SOURCES += vil2_plane.cxx
SOURCES += vil2_print.cxx
MANPAGE_SOURCES += vil2_view_as.h
MANPAGE_SOURCES += vil2_fill.h
MANPAGE_SOURCES += vil2_convert.h
MANPAGE_SOURCES += vil2_transform.h
SOURCES += file_formats/vil2_pnm.cxx
SOURCES += file_formats/vil2_png.cxx
SOURCES += file_formats/vil2_jpeg.cxx
SOURCES += file_formats/vil2_jpeg_compressor.cxx
SOURCES += file_formats/vil2_jpeg_decompressor.cxx
SOURCES += file_formats/vil2_jpeg_destination_mgr.cxx
SOURCES += file_formats/vil2_jpeg_source_mgr.cxx
MANPAGE_SOURCES += file_formats/vil2_jpeglib.h
SOURCES += file_formats/vil2_bmp.cxx
SOURCES += file_formats/vil2_bmp_core_header.cxx
SOURCES += file_formats/vil2_bmp_file_header.cxx
SOURCES += file_formats/vil2_bmp_info_header.cxx
SOURCES += vil2_decimate.cxx
SOURCES += vil2_vil1.cxx
MANPAGE_SOURCES += vil2_resample_bilin.txx

SUBDIRS += algo io

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests examples
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
