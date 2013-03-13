USES += contrib/brl contrib/brl/bbas core

LIBRARY := vsph
VERSION := 1.0

SOURCES += vsph_camera_bounds.cxx
SOURCES += vsph_spherical_coord.cxx
SOURCES += vsph_sph_point_3d.cxx
SOURCES += vsph_sph_point_2d.cxx
SOURCES += vsph_sph_box_2d.cxx
SOURCES += vsph_utils.cxx
SOURCES += vsph_unit_sphere.cxx
SOURCES += vsph_grid_index_2d.cxx
SOURCES += vsph_segment_sphere.cxx
SOURCES += vsph_sph_cover_2d.cxx

MANPAGE_SOURCES += vsph_defs.h
MANPAGE_SOURCES += vsph_view_point.h
MANPAGE_SOURCES += vsph_view_sphere.txx
MANPAGE_SOURCES += vsph_spherical_coord_sptr.h
MANPAGE_SOURCES += vsph_unit_sphere_sptr.h

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
