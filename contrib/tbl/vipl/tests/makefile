USES := vxl tbl mul

IULIBS += -lmil -lmbl
include ${IUEROOT}/tbl/vipl/config.mk
include ${IUEROOT}/vxl/vnl/algo/config.mk
include ${IUEROOT}/vxl/vnl/io/config.mk
include ${IUEROOT}/vxl/vgl/io/config.mk
include ${IUEROOT}/vxl/testlib/config.mk

# Do not instantiate templates in the library but implicitly, on use:
USE_IMPLICIT_TEMPLATES := 1

TESTMAIN := test_driver.cxx
TESTS += vipl_test_threshold.cxx
TESTS += vipl_test_erode_disk.cxx
TESTS += vipl_test_gaussian_convolution.cxx
TESTS += vipl_test_histogram.cxx

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
