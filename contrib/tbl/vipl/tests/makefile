USES := vxl tbl mul
include ${IUEROOT}/vxl/vil/config.mk
include ${IUEROOT}/vxl/vnl/algo/config.mk

# Do not instantiate templates in the library but implicitly, on use:
USE_IMPLICIT_TEMPLATES := 1

TESTMAIN := test_driver.cxx
TESTS += test_threshold.cxx
TESTS += test_erode_disk.cxx
TESTS += test_gaussian_convolution.cxx

IULIBS += -lvipl -lmil -lmbl -lvil -lvnl-algo -lvnl-io -lvnl -lvgl-io -lvgl -lvsl -ltestlib

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
