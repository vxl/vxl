#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

USES := vxl
USES += mul

LIBRARY := mil3d
VERSION := 1.0

MANPAGE_SOURCES += mil3d_image_3d.h
SOURCES += mil3d_image_3d_of.cxx
SOURCES += mil3d_transform_3d.cxx
MANPAGE_SOURCES += mil3d_image_3d_of.txx
MANPAGE_SOURCES += mil3d_sample_profile_3d.txx
MANPAGE_SOURCES += mil3d_trilin_interp_3d.h

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
