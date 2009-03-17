USES += contrib/brl contrib/brl/bbas core
include ../config.mk
include ../algo/config.mk
include ${VXLROOT}/core/testlib/config.mk

TESTMAIN := test_driver.cxx
TESTS += bmsh3d_test_face_geom.cxx
TESTS += test_mesh_break_face.cxx

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
