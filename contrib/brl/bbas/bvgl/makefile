# Brown basic geometry library
USES += contrib/brl contrib/brl/bbas core

LIBRARY := bvgl
VERSION := 1.0

SOURCES += bvgl_articulated_poly.cxx
MANPAGE_SOURCES += bvgl_articulated_poly_sptr.h
SOURCES += bvgl_change_obj.cxx
MANPAGE_SOURCES += bvgl_change_obj_sptr.h
SOURCES += bvgl_changes.cxx
MANPAGE_SOURCES += bvgl_changes_sptr.h
MANPAGE_SOURCES += bvgl_point_3d_cmp.h
MANPAGE_SOURCES += bvgl_triangle_interpolation_iterator.txx
MANPAGE_SOURCES += bvgl_volume_of_intersection.txx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
