VXL := 1

USES = vxl

TESTS += test_assign.cxx

# general reader/saver tester
ifneq ($(strip $(IU_COMPILER)),CC-n32) 
#CC-n32 hangs when compiling this
TESTS += test_save_load_image.cxx
endif

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
