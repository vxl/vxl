USES := vxl mul

LIBRARY := vimt
VERSION := 1.0

SOURCES += vimt_image.cxx
SOURCES += vimt_image_2d_of.cxx
SOURCES += vimt_image_pyramid.cxx
SOURCES += vimt_image_pyramid_builder.cxx
SOURCES += vimt_transform_2d.cxx
SOURCES += vimt_gaussian_pyramid_builder_2d.cxx
MANPAGE_SOURCES += vimt_gaussian_pyramid_builder_2d.txx
MANPAGE_SOURCES += vimt_image_2d.h
MANPAGE_SOURCES += vimt_image_2d_of.txx
MANPAGE_SOURCES += vimt_image_pyramid.txx
MANPAGE_SOURCES += vimt_sample_profile_bilin.txx

ifndef NOTEST
SUBDIRS += examples
endif

################################
include ${IUEROOT}/config/top.mk
################################
