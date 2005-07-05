USES += core

LIBRARY := vil
VERSION := 1.0

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

# This is where configure will put vil/vil_config.h
INCDIRS += $(RELOBJDIR)/include

# basic things
MANPAGE_SOURCES += vil_config.h.in
MANPAGE_SOURCES += vil_exception.h

SOURCES += vil_image_resource.cxx
MANPAGE_SOURCES += vil_image_resource_sptr.h
SOURCES += vil_image_resource_plugin.cxx
MANPAGE_SOURCES += vil_image_view.txx
MANPAGE_SOURCES += vil_image_view_base.h
SOURCES += vil_memory_chunk.cxx

SOURCES += vil_file_format.cxx
SOURCES += vil_memory_image.cxx

# file format readers/writers
SOURCES += file_formats/vil_pnm.cxx
SOURCES += file_formats/vil_ras.cxx
SOURCES += file_formats/vil_iris.cxx
SOURCES += file_formats/vil_mit.cxx
SOURCES += file_formats/vil_viff.cxx
SOURCES += file_formats/vil_viff_support.c
MANPAGE_SOURCES += file_formats/vil_viffheader.h
ifeq ($(HAS_DCMTK),1)
USE_DCMTK := 1
SOURCES += file_formats/vil_dicom.cxx
SOURCES += file_formats/vil_dicom_header.cxx
SOURCES += file_formats/vil_dicom_stream.cxx
endif
ifeq ($(HAS_PNG),1)
USE_PNG := 1
SOURCES += file_formats/vil_png.cxx
endif
ifeq ($(HAS_JPEG),1)
USE_JPEG := 1
SOURCES += file_formats/vil_jpeg.cxx
SOURCES += file_formats/vil_jpeg_compressor.cxx
SOURCES += file_formats/vil_jpeg_decompressor.cxx
SOURCES += file_formats/vil_jpeg_destination_mgr.cxx
SOURCES += file_formats/vil_jpeg_source_mgr.cxx
MANPAGE_SOURCES += file_formats/vil_jpeglib.h
endif
SOURCES += file_formats/vil_bmp.cxx
SOURCES += file_formats/vil_bmp_core_header.cxx
SOURCES += file_formats/vil_bmp_file_header.cxx
SOURCES += file_formats/vil_bmp_info_header.cxx
ifeq ($(HAS_TIFF),1)
USE_TIFF := 1
SOURCES += file_formats/vil_tiff.cxx
endif

#SOURCES += file_formats/vil_nitf.cxx
#SOURCES += file_formats/vil_nitf_extended_header.cxx
#SOURCES += file_formats/vil_nitf_header.cxx
#SOURCES += file_formats/vil_nitf_image_subheader.cxx
#SOURCES += file_formats/vil_nitf_image_subheader_v20.cxx
#SOURCES += file_formats/vil_nitf_message_header.cxx
#SOURCES += file_formats/vil_nitf_message_header_v20.cxx
#SOURCES += file_formats/vil_nitf_util.cxx
#SOURCES += file_formats/vil_nitf_version.cxx
#SOURCES += file_formats/vil_nitf_version_v20.cxx
#MANPAGE_SOURCES += file_formats/vil_nitf_blocking_info.h
#MANPAGE_SOURCES += file_formats/vil_nitf_header_v20.h
#MANPAGE_SOURCES += file_formats/vil_nitf_macro_defs.h
#MANPAGE_SOURCES += file_formats/vil_nitf_typeinfo.h

# image operations
SOURCES += vil_crop.cxx
SOURCES += vil_clamp.cxx
SOURCES += vil_transpose.cxx
SOURCES += vil_flip.cxx
SOURCES += vil_plane.cxx
MANPAGE_SOURCES += vil_math.h
MANPAGE_SOURCES += vil_view_as.h
MANPAGE_SOURCES += vil_fill.h
MANPAGE_SOURCES += vil_transform.h
MANPAGE_SOURCES += vil_convert.h
SOURCES += vil_decimate.cxx
MANPAGE_SOURCES += vil_rotate.txx
MANPAGE_SOURCES += vil_nearest_neighbour_interp.h
MANPAGE_SOURCES += vil_warp.h

SOURCES += vil_load.cxx
SOURCES += vil_save.cxx
SOURCES += vil_copy.cxx
MANPAGE_SOURCES += vil_copy.txx
SOURCES += vil_new.cxx
SOURCES += vil_print.cxx

# Sampling Operations
MANPAGE_SOURCES += vil_bilin_interp.h
MANPAGE_SOURCES += vil_sample_profile_bilin.txx
MANPAGE_SOURCES += vil_sample_grid_bilin.txx
MANPAGE_SOURCES += vil_resample_bilin.txx
MANPAGE_SOURCES += vil_bicub_interp.txx
MANPAGE_SOURCES += vil_sample_profile_bicub.txx
MANPAGE_SOURCES += vil_sample_grid_bicub.txx
MANPAGE_SOURCES += vil_resample_bicub.txx

# Stuff
MANPAGE_SOURCES += vil_smart_ptr.txx
MANPAGE_SOURCES += vil_property.h
SOURCES += vil_pixel_format.cxx
MANPAGE_SOURCES += vil_pixel_traits.h
MANPAGE_SOURCES += vil_rgb.txx
MANPAGE_SOURCES += vil_rgba.txx
MANPAGE_SOURCES += vil_fwd.h

# Streams
SOURCES += vil_stream.cxx
SOURCES += vil_stream_fstream.cxx
SOURCES += vil_stream_core.cxx
SOURCES += vil_stream_section.cxx
SOURCES += vil_stream_url.cxx
SOURCES += vil_stream_read.cxx
SOURCES += vil_stream_write.cxx
SOURCES += vil_open.cxx

ifndef NOTEST
 SUBDIRS += tests examples
endif

# configure stuff:
xall:: $(RELOBJDIR)/include/vil/vil_config.h
$(RELOBJDIR)/include/vil/vil_config.h: vil_config.h.in
	mkdir -p $(RELOBJDIR)/include/vil
	sed -e 's/@HAS_.*@/1/' -e 's/[._]in//g' $^ > $@

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
