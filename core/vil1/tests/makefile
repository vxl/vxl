USES = vxl
# The following three are needed, as libvil uses tiff, jpeg, png if available:
TRY_TIFF := 1
TRY_JPEG := 1
TRY_PNG  := 1

## vil_stream_url uses tcp sockets.
USE_SOCKETS := 1

TESTS += test_all.cxx
TESTS += test_assign.cxx

# general reader/saver tester
TESTS += test_save_load_image.cxx

# specific testers:
# pnm:
TESTS += test_load.cxx
#memory_image:
TESTS += test_memory_image.cxx
TESTS += test_memory_image_of.cxx
TESTS += test_write_endian.cxx
TESTS += test_interpolate.cxx

IULIBS = -lvil -lvpl

include ../vil_config.mk

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
