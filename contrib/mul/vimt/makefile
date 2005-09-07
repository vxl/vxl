USES += contrib/mul core

LIBRARY := vimt
VERSION := 1.0

MANPAGE_SOURCES += vimt_fwd.h

SOURCES += vimt_image.cxx
SOURCES += vimt_image_2d.cxx
MANPAGE_SOURCES += vimt_image_2d_of.txx
SOURCES += vimt_image_pyramid.cxx
SOURCES += vimt_image_pyramid_builder.cxx
SOURCES += vimt_transform_2d.cxx
SOURCES += vimt_add_all_binary_loaders.cxx
SOURCES += vimt_load_transform.cxx
MANPAGE_SOURCES += vimt_dog_pyramid_builder_2d.txx
MANPAGE_SOURCES += vimt_scale_pyramid_builder_2d.txx
MANPAGE_SOURCES += vimt_gaussian_pyramid_builder_2d.txx
MANPAGE_SOURCES += vimt_gaussian_pyramid_builder_2d_general.txx
MANPAGE_SOURCES += vimt_image_pyramid.txx
MANPAGE_SOURCES += vimt_sample_profile_bilin.txx
MANPAGE_SOURCES += vimt_bilin_interp.h
MANPAGE_SOURCES += vimt_resample_bilin.h
MANPAGE_SOURCES += vimt_sample_grid_bilin.txx
MANPAGE_SOURCES += vimt_sample_grid_bicub.txx
MANPAGE_SOURCES += vimt_crop.h

SUBDIRS += algo

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += examples tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
