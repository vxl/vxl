USES += contrib/mul core

LIBRARY := vimt3d
VERSION := 1.0

SOURCES += vimt3d_image_3d.cxx
MANPAGE_SOURCES += vimt3d_image_3d_of.txx
SOURCES += vimt3d_transform_3d.cxx
SOURCES += vimt3d_gaussian_pyramid_builder_3d.cxx
MANPAGE_SOURCES += vimt3d_gaussian_pyramid_builder_3d.txx
MANPAGE_SOURCES += vimt3d_sample_grid_trilin.txx
MANPAGE_SOURCES += vimt3d_sample_profile_trilin.h
MANPAGE_SOURCES += vimt3d_trilin_interp.h
MANPAGE_SOURCES += vimt3d_slice.h

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
