USES = v3p vxl
# The following three are needed, as libvil uses tiff, jpeg, png if available:
TRY_TIFF := 1
TRY_JPEG := 1
TRY_PNG  := 1

TESTS += test_assign.cxx

# general reader/saver tester
#ifneq ($(strip $(IU_COMPILER)),CC-n32) 
#CC-n32 hangs when compiling this
TESTS += test_save_load_image.cxx
#endif

# specific testers:
# pnm:
TESTS += test_load.cxx
#memory_image:
TESTS += test_memory_image.cxx
TESTS += test_memory_image_of.cxx
TESTS += test_write_endian.cxx

IULIBS = -lvil -lvcl

include ../vil_config.mk

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
