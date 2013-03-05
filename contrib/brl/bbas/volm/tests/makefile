USES += contrib/brl contrib/brl/bbas core
include ../config.mk
include ${VXLROOT}/core/testlib/config.mk

TESTMAIN := test_driver.cxx
TESTS += test_spherical_container.cxx
TESTS += test_spherical_shell_container.cxx
TESTS += test_loc_hyp.cxx
TESTS += test_tile.cxx
TESTS += test_query.cxx
TESTS += test_index.cxx
TESTS += test_region_query.cxx
TESTS += test_camera_space.cxx
TESTS += test_io.cxx
TESTS += test_index_query_matcher.cxx
TESTS += test_region_index.cxx
TESTS += test_spherical_region.cxx

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
