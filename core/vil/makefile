USES := vxl mul

LIBRARY := vil2
VERSION := 1.0

SOURCES += vil2_image_data.cxx
SOURCES += vil2_image_view.cxx
MANPAGE_SOURCES += vil2_image_view.txx
SOURCES += vil2_file_format.cxx
SOURCES += vil2_pixel_format.cxx
SOURCES += file_formats/vil2_pnm.cxx
MANPAGE_SOURCES += vil2_smart_ptr.txx
SOURCES += vil2_memory_chunk.cxx
SOURCES += vil2_image_view_base.cxx
SOURCES += vil2_load.cxx
SOURCES += vil2_image_view_functions.cxx
MANPAGE_SOURCES += vil2_image_view_functions.txx
MANPAGE_SOURCES += vil2_reg_image.h
MANPAGE_SOURCES += vil2_reg_image_2d.h
SOURCES += vil2_crop.cxx
SOURCES += vil2_memory_image.cxx
SOURCES += vil2_new.cxx
SOURCES += vil2_save.cxx
MANPAGE_SOURCES += vil2_bilin_interp.h
MANPAGE_SOURCES += vil2_sample_profile_bilin.txx
MANPAGE_SOURCES += vil2_byte.h
MANPAGE_SOURCES += vil2_pixel_traits.h
MANPAGE_SOURCES += vil2_sample_grid_bilin.txx

SUBDIRS += algo

ifndef NOTEST
 SUBDIRS += examples tests
endif

################################
include ${IUEROOT}/config/top.mk
################################
