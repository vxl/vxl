###############################################################################
# File name: makefile
# Description: Project file for vsol
# -----------------------------------------------------------------------------
# Version |Date      | Author                   |Comment
# --------+----------+--------------------------+------------------------------
# 1.0     |2000/04/26| François BERTEL          |Creation
###############################################################################

USES += contrib/gel core

LIBRARY := vsol
VERSION := 1.0

# dimensionless virtual classes
SOURCES += vsol_spatial_object.cxx
SOURCES += vsol_box.cxx
SOURCES += vsol_flags_id.cxx

# embedding dimension 2
SOURCES += vsol_spatial_object_2d.cxx
SOURCES += vsol_box_2d.cxx
SOURCES += vsol_group_2d.cxx
# 0-D
SOURCES += vsol_point_2d.cxx
# 1-D
SOURCES += vsol_curve_2d.cxx
SOURCES += vsol_line_2d.cxx
SOURCES += vsol_conic_2d.cxx
SOURCES += vsol_polyline_2d.cxx
SOURCES += vsol_digital_curve_2d.cxx
# 2-D
MANPAGE_SOURCES += vsol_region_2d.h
SOURCES += vsol_polygon_2d.cxx
SOURCES += vsol_triangle_2d.cxx
SOURCES += vsol_rectangle_2d.cxx

# embedding dimension 3
SOURCES += vsol_spatial_object_3d.cxx
SOURCES += vsol_box_3d.cxx
SOURCES += vsol_group_3d.cxx
# 0-D
SOURCES += vsol_point_3d.cxx
# 1-D
SOURCES += vsol_curve_3d.cxx
SOURCES += vsol_line_3d.cxx
SOURCES += vsol_polyline_3d.cxx
SOURCES += vsol_digital_curve_3d.cxx
# 2-D
MANPAGE_SOURCES += vsol_surface_3d.h
MANPAGE_SOURCES += vsol_region_3d.h
SOURCES += vsol_polygon_3d.cxx
SOURCES += vsol_triangle_3d.cxx
SOURCES += vsol_rectangle_3d.cxx
# 3-D
MANPAGE_SOURCES += vsol_volume_3d.h
SOURCES += vsol_polyhedron.cxx
SOURCES += vsol_tetrahedron.cxx
SOURCES += vsol_cylinder.cxx

# smart pointer header files
MANPAGE_SOURCES += vsol_box_sptr.h
MANPAGE_SOURCES += vsol_box_2d_sptr.h
MANPAGE_SOURCES += vsol_box_3d_sptr.h
MANPAGE_SOURCES += vsol_conic_2d_sptr.h
MANPAGE_SOURCES += vsol_curve_2d_sptr.h
MANPAGE_SOURCES += vsol_curve_3d_sptr.h
MANPAGE_SOURCES += vsol_digital_curve_2d_sptr.h
MANPAGE_SOURCES += vsol_digital_curve_3d_sptr.h
MANPAGE_SOURCES += vsol_flags_id_sptr.h
MANPAGE_SOURCES += vsol_group_2d_sptr.h
MANPAGE_SOURCES += vsol_group_3d_sptr.h
MANPAGE_SOURCES += vsol_line_2d_sptr.h
MANPAGE_SOURCES += vsol_line_3d_sptr.h
MANPAGE_SOURCES += vsol_point_2d_sptr.h
MANPAGE_SOURCES += vsol_point_3d_sptr.h
MANPAGE_SOURCES += vsol_polygon_2d_sptr.h
MANPAGE_SOURCES += vsol_polygon_3d_sptr.h
MANPAGE_SOURCES += vsol_polyhedron_sptr.h
MANPAGE_SOURCES += vsol_polyline_2d_sptr.h
MANPAGE_SOURCES += vsol_polyline_3d_sptr.h
MANPAGE_SOURCES += vsol_rectangle_2d_sptr.h
MANPAGE_SOURCES += vsol_rectangle_3d_sptr.h
MANPAGE_SOURCES += vsol_region_2d_sptr.h
MANPAGE_SOURCES += vsol_region_3d_sptr.h
MANPAGE_SOURCES += vsol_spatial_object_sptr.h
MANPAGE_SOURCES += vsol_spatial_object_2d_sptr.h
MANPAGE_SOURCES += vsol_spatial_object_3d_sptr.h
MANPAGE_SOURCES += vsol_surface_3d_sptr.h
MANPAGE_SOURCES += vsol_tetrahedron_sptr.h
MANPAGE_SOURCES += vsol_triangle_2d_sptr.h
MANPAGE_SOURCES += vsol_triangle_3d_sptr.h
MANPAGE_SOURCES += vsol_volume_3d_sptr.h
MANPAGE_SOURCES += vsol_cylinder_sptr.h

IMMEDIATE_LIBS += -lvgl-algo -lvgl -lvnl-algo -lvnl -lvbl -lvul

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
SUBDIRS += examples
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
