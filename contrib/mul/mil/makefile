USES := vxl
USES += mul

LIBRARY := mil
VERSION := 1.0

SOURCES += mil_transform_2d.cxx
SOURCES += mil_image.cxx
SOURCES += mil_image_data.cxx
MANPAGE_SOURCES += mil_image_data.txx
SOURCES += mil_image_pyramid.cxx
MANPAGE_SOURCES += mil_image_pyramid.txx
SOURCES += mil_image_pyramid_builder.cxx
SOURCES += mil_image_2d_of.cxx
MANPAGE_SOURCES += mil_image_2d_of.txx
SOURCES += mil_convert_image_2d.cxx
SOURCES += mil_image_io.cxx
SOURCES += mil_byte_image_2d_io.cxx
SOURCES += mil_gauss_reduce_2d.cxx
SOURCES += mil_gaussian_pyramid_builder_2d_general.cxx
MANPAGE_SOURCES += mil_gaussian_pyramid_builder_2d_general.txx
SOURCES += mil_gaussian_pyramid_builder_2d.cxx
MANPAGE_SOURCES += mil_gaussian_pyramid_builder_2d.txx
SOURCES += mil_normalise_image_2d.cxx
SOURCES += mil_invert_image_2d.cxx
SOURCES += mil_byte_image_2d_io_plugin.cxx
SOURCES += mil_scale_pyramid_builder_2d.cxx
MANPAGE_SOURCES += mil_scale_pyramid_builder_2d.txx
MANPAGE_SOURCES += mil_sample_grid_2d.txx

# Additional files to document:
MANPAGE_SOURCES += mil_all.h
MANPAGE_SOURCES += mil_bilin_interp_2d.h
MANPAGE_SOURCES += mil_convert_vil.h
MANPAGE_SOURCES += mil_image_2d.h
MANPAGE_SOURCES += mil_sample_profile_2d.txx
MANPAGE_SOURCES += mil_wrappers.h

SUBDIRS += algo

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
