USES := vxl mul

LIBRARY := vil2
VERSION := 1.0

SOURCES += vil2_image_resource.cxx
SOURCES += vil2_image_view.cxx
MANPAGE_SOURCES += vil2_image_view.txx
SOURCES += vil2_file_format.cxx
SOURCES += vil2_pixel_format.cxx
SOURCES += file_formats/vil2_pnm.cxx
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
MANPAGE_SOURCES += vil2_pixel_traits.h
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

SUBDIRS += algo io

ifndef NOTEST
 SUBDIRS += tests examples
endif

################################
include ${IUEROOT}/config/top.mk
################################
