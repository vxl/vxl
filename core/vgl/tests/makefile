USES += core
include ../algo/config.mk
include ${IUEROOT}/core/vnl/algo/config.mk
include ${IUEROOT}/core/testlib/config.mk

TESTMAIN := test_driver.cxx

TESTS += test_homg.cxx
TESTS += test_cartesian.cxx
TESTS += test_conic.cxx 
TESTS += test_distance.cxx
TESTS += test_polygon.cxx
TESTS += test_polygon_scan_iterator.cxx
TESTS += test_triangle_scan_iterator.cxx
TESTS += test_ellipse_scan_iterator.cxx
TESTS += test_area.cxx
TESTS += test_clip.cxx

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
