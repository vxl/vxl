USES := vxl tbl

include ${IUEROOT}/tbl/vepl/config.mk
include ${IUEROOT}/vxl/testlib/config.mk

# Do not instantiate templates in the library but implicitly, on use:
USE_IMPLICIT_TEMPLATES := 1

TESTMAIN := test_driver.cxx
TESTS += vepl_test_threshold.cxx
TESTS += vepl_test_erode_disk.cxx

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
