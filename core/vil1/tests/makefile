USES = vxl
include ${IUEROOT}/vxl/vil/config.mk

TESTS += vil_test_all.cxx
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
TESTS += test_flipud.cxx
TESTS += test_colour_space.cxx
TESTS += test_copy.cxx
TESTS += test_file_format_read.cxx
PARAMS_test_copy := square.pgm
PARAMS_test_file_format_read := file_read_data

IULIBS = -lvil -lvpl -lvul

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
