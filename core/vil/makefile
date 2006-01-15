USES += core

LIBRARY := vil
VERSION := 1.0

#######################################
include ${VXLROOT}/config/top-params.mk
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
SOURCES += file_formats/vil_tiff_header.cxx
endif

  ################################
  # NITF 2.0 and 2.1 reading code
  ################################
# The actual vil_image_resource subclass
SOURCES += file_formats/vil_nitf2_image.cxx

# For reading the NITF file headers as well as the image headers
SOURCES += file_formats/vil_nitf2_header.cxx
SOURCES += file_formats/vil_nitf2_image_subheader.cxx
SOURCES += file_formats/vil_nitf2_des.cxx

# Stuff shared by all NITF2 classes
MANPAGE_SOURCES += file_formats/vil_nitf2.h

# Generic framework for reading NITF tagged record extensions (TREs)
SOURCES += file_formats/vil_nitf2_array_field.cxx
SOURCES += file_formats/vil_nitf2_classification.cxx
SOURCES += file_formats/vil_nitf2_compound_field_value.cxx
SOURCES += file_formats/vil_nitf2_data_mask_table.cxx
SOURCES += file_formats/vil_nitf2_field.cxx
SOURCES += file_formats/vil_nitf2_field_definition.cxx
SOURCES += file_formats/vil_nitf2_field_formatter.cxx
SOURCES += file_formats/vil_nitf2_field_functor.cxx
SOURCES += file_formats/vil_nitf2_field_sequence.cxx
MANPAGE_SOURCES += file_formats/vil_nitf2_index_vector.h
SOURCES += file_formats/vil_nitf2_scalar_field.cxx
SOURCES += file_formats/vil_nitf2_tagged_record.cxx
SOURCES += file_formats/vil_nitf2_tagged_record_definition.cxx
MANPAGE_SOURCES += file_formats/vil_nitf2_typed_array_field.h
MANPAGE_SOURCES += file_formats/vil_nitf2_typed_scalar_field.h
SOURCES += file_formats/vil_nitf2_typed_field_formatter.cxx

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
MANPAGE_SOURCES += vil_color_table.h
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
ifeq ($(OS),win32)
ifeq ($(USE_LFS),1)
#This is a hack since win32 doesn't have transparent Large File Support.
DEFINES += -DVIL_USE_FSTREAM64
SOURCES += vil_stream_fstream64.cxx
endif
endif


ifndef NOTEST
 SUBDIRS += tests examples
endif

# configure stuff:
xall:: $(RELOBJDIR)/include/vil/vil_config.h
$(RELOBJDIR)/include/vil/vil_config.h: vil_config.h.in
	mkdir -p $(RELOBJDIR)/include/vil
	sed -e 's/@HAS_.*@/1/' -e 's/[._]in//g' $^ > $@

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
