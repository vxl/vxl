USES += contrib/brl contrib/brl/bbas core

LIBRARY := volm
VERSION := 1.0

MANPAGE_SOURCES += volm_char_codes.h
SOURCES += volm_spherical_container.cxx
SOURCES += volm_query.cxx
SOURCES += volm_io.cxx
SOURCES += volm_tile.cxx
SOURCES += volm_spherical_shell_container.cxx
SOURCES += volm_loc_hyp.cxx
SOURCES += volm_spherical_region_query.cxx
SOURCES += volm_spherical_layers.cxx
SOURCES += volm_spherical_query_region.cxx
SOURCES += volm_geo_index.cxx
SOURCES += volm_camera_space.cxx

MANPAGE_SOURCES += volm_query_sptr.h
MANPAGE_SOURCES += volm_spherical_container_sptr.h
MANPAGE_SOURCES += volm_spherical_shell_container_sptr.h
MANPAGE_SOURCES += volm_loc_hyp_sptr.h
MANPAGE_SOURCES += volm_geo_index_sptr.h
MANPAGE_SOURCES += volm_camera_space_sptr.h

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests exe
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
