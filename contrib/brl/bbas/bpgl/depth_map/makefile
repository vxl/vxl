USES += contrib/brl contrib/brl/bbas core

LIBRARY := bpgl_depth_map
VERSION := 1.0

SOURCES += depth_map_region.cxx
SOURCES += depth_map_scene.cxx
MANPAGE_SOURCES += depth_map_defs.h

MANPAGE_SOURCES += depth_map_region_sptr.h
MANPAGE_SOURCES += depth_map_scene_sptr.h

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
