USES := vxl tbl
include ${IUEROOT}/vxl/vil/config.mk

# Do not instantiate templates in the library but implicitly, on use:
USE_IMPLICIT_TEMPLATES := 1

TESTMAIN := test_driver.cxx
TESTS += vepl_test_threshold.cxx
TESTS += vepl_test_erode_disk.cxx

IULIBS += -lvepl -lvipl -lvil -ltestlib

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
