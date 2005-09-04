USES += contrib/prip core

LIBRARY := vpyr
VERSION := 1.0

SOURCES += vpyr_2_pyramid_base.cxx
SOURCES += vpyr_2_tpyramid_base.cxx
MANPAGE_SOURCES += vpyr_2_pyramid.txx
MANPAGE_SOURCES += vpyr_2_pyramid_level.txx
MANPAGE_SOURCES += vpyr_2_pyramid_level_elts.h
MANPAGE_SOURCES += vpyr_2_tpyramid.txx
MANPAGE_SOURCES += vpyr_2_tpyramid_level.txx
MANPAGE_SOURCES += vpyr_2_tpyramid_level_elts.h
MANPAGE_SOURCES += vpyr_2_tpyramid_builder.h
MANPAGE_SOURCES += vpyr_top_tmap.txx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
SUBDIRS += examples/simple_segmentation
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
