VXL := 1

USES = vxl

# general reader/saver tester
TESTS += test_save_load_image.cxx

# specific testers:
# pnm:
TESTS += test_load.cxx
#memory_image:
TESTS += test_memory_image.cxx
TESTS += test_memory_image_of.cxx

IULIBS = -lvil -lvcl

include ../vil_config.mk

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
