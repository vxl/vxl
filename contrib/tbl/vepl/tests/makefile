USES := vxl tbl
include ${IUEROOT}/vxl/vil/config.mk

# Do not instantiate templates in the library but implicitly, on use:
USE_IMPLICIT_TEMPLATES := 1

TESTMAIN := test_driver.cxx
TESTS += test_threshold.cxx
TESTS += test_erode_disk.cxx

IULIBS += -lvepl -lvipl -lvil -ltestlib

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
