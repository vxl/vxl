###############################################################################
# File name: makefile
# Description: Project file for vcsl
# -----------------------------------------------------------------------------
# Version |Date      | Author                   |Comment
# --------+----------+--------------------------+------------------------------
# 1.0     |2000/06/27| François BERTEL          |Creation
###############################################################################

USES := vxl

LIBRARY := vcsl
VERSION := 1.0

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

IMMEDIATE_LIBS += -lvbl

###############################################################################
include ${IUEROOT}/config/top-params.mk
###############################################################################

ifndef NOTEST
SUBDIRS += tests
SUBDIRS += examples
endif

###############################################################################
include ${IUEROOT}/config/top-rules.mk
###############################################################################
