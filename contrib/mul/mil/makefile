#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

USES := vxl
USES += mul

LIBRARY := mil
VERSION := 1.0

SOURCES += mil_transform_2d.cxx
SOURCES += mil_image.cxx
SOURCES += mil_image_data.cxx
SOURCES += mil_image_pyramid.cxx
SOURCES += mil_image_pyramid_builder.cxx
SOURCES += mil_image_2d_of.cxx
SOURCES += mil_convert_image_2d.cxx
SOURCES += mil_image_io.cxx
SOURCES += mil_byte_image_2d_io.cxx
SOURCES += mil_gauss_reduce_2d.cxx
SOURCES += mil_gaussian_pyramid_builder_2d_general.cxx
SOURCES += mil_gaussian_pyramid_builder_2d.cxx

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
