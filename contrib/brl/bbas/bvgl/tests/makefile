USES += contrib/brl contrib/brl/bbas core
include ../config.mk
include ${VXLROOT}/core/testlib/config.mk

TESTMAIN := test_driver.cxx
TESTS += test_bvgl_changes.cxx
TESTS += test_bvgl_volume_of_intersection.cxx
TESTS += test_bvgl_ray_pyramid.cxx
TESTS += test_bvgl_intersection.cxx
TESTS += test_bvgl_labelme_parser.cxx

MINI_PROG_SOURCES += test_include.cxx
MINI_PROG_SOURCES += test_template_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
