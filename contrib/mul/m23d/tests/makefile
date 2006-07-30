USES += contrib/mul core
include ../config.mk
include $(VXLROOT)/core/testlib/config.mk

TESTMAIN := test_driver.cxx
TESTS += test_correction_matrix_error.cxx
TESTS += test_make_ortho_projection.cxx
TESTS += test_ortho_flexible_builder.cxx
TESTS += test_ortho_rigid_builder.cxx
TESTS += test_rotation_from_ortho_projection.cxx
TESTS += test_rotation_matrix.cxx

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
