USES := vxl mul

LIBRARY := vimt
VERSION := 1.0

SOURCES += vimt_image.cxx
SOURCES += vimt_image_2d_of.cxx
SOURCES += vimt_image_pyramid.cxx
SOURCES += vimt_image_pyramid_builder.cxx
SOURCES += vimt_transform_2d.cxx
SOURCES += vimt_gaussian_pyramid_builder_2d.cxx
SOURCES += vimt_gaussian_pyramid_builder_2d_general.cxx
MANPAGE_SOURCES += vimt_gaussian_pyramid_builder_2d.txx
MANPAGE_SOURCES += vimt_gaussian_pyramid_builder_2d_general.txx
MANPAGE_SOURCES += vimt_image_2d.h
MANPAGE_SOURCES += vimt_image_2d_of.txx
MANPAGE_SOURCES += vimt_image_pyramid.txx
MANPAGE_SOURCES += vimt_sample_profile_bilin.txx
MANPAGE_SOURCES += vimt_bilin_interp.h
MANPAGE_SOURCES += vimt_resample_bilin.h
MANPAGE_SOURCES += vimt_sample_grid_bilin.txx

SUBDIRS += algo
ifndef NOTEST
SUBDIRS += examples tests
endif

################################
include ${IUEROOT}/config/top.mk
################################
