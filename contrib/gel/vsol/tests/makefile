###############################################################################
# File name: makefile
# Description: Project file for vsol tests
# -----------------------------------------------------------------------------
# Version |Date      | Author                   |Comment
# --------+----------+--------------------------+------------------------------
# 1.0     |2000/04/26| François BERTEL          |Creation
# 1.1     |2003/01/08| Peter Vanroose           |Now using testlib
###############################################################################

USES += contrib/gel core
include ../config.mk
include ${IUEROOT}/core/testlib/config.mk

TESTMAIN := test_driver.cxx

TESTS += test_vsol_point_2d.cxx
TESTS += test_vsol_group_2d.cxx
TESTS += test_vsol_line_2d.cxx
TESTS += test_vsol_conic_2d.cxx
TESTS += test_vsol_triangle_2d.cxx
TESTS += test_vsol_rectangle_2d.cxx
TESTS += test_vsol_polygon_2d.cxx

TESTS += test_vsol_point_3d.cxx
TESTS += test_vsol_group_3d.cxx
TESTS += test_vsol_line_3d.cxx
TESTS += test_vsol_triangle_3d.cxx
TESTS += test_vsol_rectangle_3d.cxx
TESTS += test_vsol_polygon_3d.cxx
TESTS += test_vsol_tetrahedron.cxx

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
