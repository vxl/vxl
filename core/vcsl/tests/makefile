###############################################################################
# File name: makefile
# Description: Project file for vcsl tests
# -----------------------------------------------------------------------------
# Version |Date      | Author                   |Comment
# --------+----------+--------------------------+------------------------------
# 1.0     |2000/06/16| François BERTEL          |Creation
# 1.1     |2002/11/13| Peter Vanroose           |Now using TESTMAIN
###############################################################################

USES := vxl
include ${IUEROOT}/vxl/vcsl/config.mk
include ${IUEROOT}/vxl/testlib/config.mk

TESTMAIN := test_driver.cxx

SOURCES += vcsl_test_all.cxx

TESTS += test_meter.cxx
TESTS += test_radian.cxx
TESTS += test_degree.cxx

TESTS += test_cartesian_2d.cxx
TESTS += test_cartesian_3d.cxx
TESTS += test_polar.cxx
TESTS += test_cylindrical.cxx
TESTS += test_spherical.cxx

TESTS += test_transformation_graph.cxx
TESTS += test_composition.cxx
TESTS += test_scale.cxx
TESTS += test_translation.cxx
TESTS += test_rotation.cxx
TESTS += test_displacement.cxx

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
