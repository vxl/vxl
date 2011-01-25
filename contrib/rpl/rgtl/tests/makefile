USES += contrib/rpl core
include ../config.mk
include ${VXLROOT}/core/vul/config.mk
include ${VXLROOT}/core/testlib/config.mk

TESTMAIN := test_driver.cxx
TESTS += test_compact_tree_data_fixed.cxx
TESTS += test_octree_data_fixed.cxx
TESTS += test_octree_dual_visit.cxx
TESTS += test_octree_objects.cxx
TESTS += test_sqt_base.cxx

MINI_PROG_SOURCES += test_include.cxx
MINI_PROG_SOURCES += test_template_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
