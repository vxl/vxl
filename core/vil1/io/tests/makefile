USES += vxl
include ${IUEROOT}/vxl/vil/config.mk
IULIBS := -lvil-io -lvil -lvsl -lvul -ltestlib

TESTMAIN = test_driver.cxx

TESTS += test_include.cxx
TESTS += test_memory_image_format_io.cxx
TESTS += test_memory_image_impl_io.cxx
TESTS += test_memory_image_of_io.cxx
TESTS += test_rgb_io.cxx
TESTS += test_rgba_io.cxx
TESTS += golden_test_vil_io.cxx

################################
#include ${IUEROOT}/config/top.mk
include ${IUEROOT}/config/static-tests.mk
################################
