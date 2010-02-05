USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core
include ../config.mk
include ${VXLROOT}/core/testlib/config.mk

TESTMAIN := test_driver.cxx

TESTS += test_boxm_scene_crop.cxx
TESTS += test_boxm_scene_to_bvxm_grid.cxx
TESTS += test_boxm_fill_internal_cells.cxx

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
