###############################################################################
# File name: makefile
# Description: Project file for vcsl
# -----------------------------------------------------------------------------
# Version |Date      | Author                   |Comment
# --------+----------+--------------------------+------------------------------
# 1.0     |2000/06/27| François BERTEL          |Creation
###############################################################################

USES += core

LIBRARY := vcsl
VERSION := 1.0

IMMEDIATE_LIBS += -lvbl -lvnl

SOURCES += vcsl_dimension.cxx
SOURCES += vcsl_length.cxx

SOURCES += vcsl_angle.cxx

SOURCES += vcsl_unit.cxx
SOURCES += vcsl_length_unit.cxx
SOURCES += vcsl_meter.cxx

SOURCES += vcsl_angle_unit.cxx
SOURCES += vcsl_radian.cxx
SOURCES += vcsl_degree.cxx

SOURCES += vcsl_axis.cxx
SOURCES += vcsl_coordinate_system.cxx

SOURCES += vcsl_cartesian.cxx

SOURCES += vcsl_spatial.cxx
SOURCES += vcsl_graph.cxx

SOURCES += vcsl_cartesian_2d.cxx
SOURCES += vcsl_cartesian_3d.cxx

SOURCES += vcsl_polar.cxx
SOURCES += vcsl_cylindrical.cxx
SOURCES += vcsl_spherical.cxx

SOURCES += vcsl_spheroid.cxx
SOURCES += vcsl_geographic.cxx
SOURCES += vcsl_utm.cxx
SOURCES += vcsl_geocentric.cxx
SOURCES += vcsl_geodetic.cxx
SOURCES += vcsl_lambertian.cxx

SOURCES += vcsl_spatial_transformation.cxx

SOURCES += vcsl_composition.cxx
SOURCES += vcsl_scale.cxx
SOURCES += vcsl_translation.cxx
SOURCES += vcsl_rotation.cxx
SOURCES += vcsl_displacement.cxx

SOURCES += vcsl_cylindrical_to_cartesian_3d.cxx

SOURCES += vcsl_perspective.cxx
SOURCES += vcsl_matrix.cxx

MANPAGE_SOURCES += vcsl_angle_sptr.h
MANPAGE_SOURCES += vcsl_angle_unit_sptr.h
MANPAGE_SOURCES += vcsl_axis_sptr.h
MANPAGE_SOURCES += vcsl_cartesian_2d_sptr.h
MANPAGE_SOURCES += vcsl_cartesian_3d_sptr.h
MANPAGE_SOURCES += vcsl_cartesian_sptr.h
MANPAGE_SOURCES += vcsl_composition_sptr.h
MANPAGE_SOURCES += vcsl_coordinate_system_sptr.h
MANPAGE_SOURCES += vcsl_cylindrical_sptr.h
MANPAGE_SOURCES += vcsl_cylindrical_to_cartesian_3d_sptr.h
MANPAGE_SOURCES += vcsl_degree_sptr.h
MANPAGE_SOURCES += vcsl_dimension_sptr.h
MANPAGE_SOURCES += vcsl_displacement_sptr.h
MANPAGE_SOURCES += vcsl_geocentric_sptr.h
MANPAGE_SOURCES += vcsl_geodetic_sptr.h
MANPAGE_SOURCES += vcsl_geographic_sptr.h
MANPAGE_SOURCES += vcsl_graph_sptr.h
MANPAGE_SOURCES += vcsl_lambertian_sptr.h
MANPAGE_SOURCES += vcsl_length_sptr.h
MANPAGE_SOURCES += vcsl_length_unit_sptr.h
MANPAGE_SOURCES += vcsl_matrix_param.h
MANPAGE_SOURCES += vcsl_matrix_param_sptr.h
MANPAGE_SOURCES += vcsl_matrix_sptr.h
MANPAGE_SOURCES += vcsl_meter_sptr.h
MANPAGE_SOURCES += vcsl_perspective_sptr.h
MANPAGE_SOURCES += vcsl_polar_sptr.h
MANPAGE_SOURCES += vcsl_radian_sptr.h
MANPAGE_SOURCES += vcsl_rotation_sptr.h
MANPAGE_SOURCES += vcsl_scale_sptr.h
MANPAGE_SOURCES += vcsl_spatial_sptr.h
MANPAGE_SOURCES += vcsl_spatial_transformation_sptr.h
MANPAGE_SOURCES += vcsl_spherical_sptr.h
MANPAGE_SOURCES += vcsl_spheroid_sptr.h
MANPAGE_SOURCES += vcsl_translation_sptr.h
MANPAGE_SOURCES += vcsl_unit_sptr.h
MANPAGE_SOURCES += vcsl_utm_sptr.h

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
SUBDIRS += examples
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
