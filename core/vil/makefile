USES += core

LIBRARY := vil2
VERSION := 1.0

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

# basic things
SOURCES += vil2_image_resource.cxx
SOURCES += vil2_image_resource_plugin.cxx
MANPAGE_SOURCES += vil2_image_view.txx
SOURCES += vil2_image_view_base.cxx
SOURCES += vil2_memory_chunk.cxx

SOURCES += vil2_file_format.cxx
SOURCES += vil2_memory_image.cxx

# file format readers/writers
SOURCES += file_formats/vil2_pnm.cxx
ifeq ($(HAS_PNG),1)
USE_PNG := 1
SOURCES += file_formats/vil2_png.cxx
endif
ifeq ($(HAS_JPEG),1)
USE_JPEG := 1
SOURCES += file_formats/vil2_jpeg.cxx
SOURCES += file_formats/vil2_jpeg_compressor.cxx
SOURCES += file_formats/vil2_jpeg_decompressor.cxx
SOURCES += file_formats/vil2_jpeg_destination_mgr.cxx
SOURCES += file_formats/vil2_jpeg_source_mgr.cxx
MANPAGE_SOURCES += file_formats/vil2_jpeglib.h
endif
SOURCES += file_formats/vil2_bmp.cxx
SOURCES += file_formats/vil2_bmp_core_header.cxx
SOURCES += file_formats/vil2_bmp_file_header.cxx
SOURCES += file_formats/vil2_bmp_info_header.cxx
ifeq ($(HAS_TIFF),1)
USE_TIFF := 1
SOURCES += file_formats/vil2_tiff.cxx
endif

# image operations
SOURCES += vil2_crop.cxx
SOURCES += vil2_clamp.cxx
SOURCES += vil2_transpose.cxx
SOURCES += vil2_flip.cxx
SOURCES += vil2_plane.cxx
MANPAGE_SOURCES += vil2_math.h
MANPAGE_SOURCES += vil2_view_as.h
MANPAGE_SOURCES += vil2_fill.h
MANPAGE_SOURCES += vil2_transform.h
MANPAGE_SOURCES += vil2_convert.h
SOURCES += vil2_decimate.cxx

SOURCES += vil2_load.cxx
SOURCES += vil2_save.cxx
MANPAGE_SOURCES += vil2_copy.txx
SOURCES += vil2_new.cxx
SOURCES += vil2_print.cxx

# Sampling Operations
MANPAGE_SOURCES += vil2_bilin_interp.h
MANPAGE_SOURCES += vil2_sample_profile_bilin.txx
MANPAGE_SOURCES += vil2_sample_grid_bilin.txx
MANPAGE_SOURCES += vil2_resample_bilin.txx

# Stuff
MANPAGE_SOURCES += vil2_smart_ptr.txx
MANPAGE_SOURCES += vil2_property.h
SOURCES += vil2_pixel_format.cxx
MANPAGE_SOURCES += vil2_pixel_traits.h
MANPAGE_SOURCES += vil2_rgb.txx
MANPAGE_SOURCES += vil2_rgba.txx
MANPAGE_SOURCES += vil2_fwd.h

# Streams
SOURCES += vil2_stream.cxx
SOURCES += vil2_stream_fstream.cxx
SOURCES += vil2_stream_core.cxx
SOURCES += vil2_stream_section.cxx
SOURCES += vil2_stream_url.cxx
SOURCES += vil2_stream_16bit.cxx
SOURCES += vil2_stream_32bit.cxx
SOURCES += vil2_open.cxx

# Conversion to/from vil1 images
SOURCES += vil2_vil1.cxx

SUBDIRS += algo io

ifndef NOTEST
 SUBDIRS += tests examples
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
