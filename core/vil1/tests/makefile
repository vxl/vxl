USES = vxl
include ${IUEROOT}/vxl/vil/config.mk
include ${IUEROOT}/vxl/vul/config.mk
include ${IUEROOT}/vxl/vpl/config.mk
include ${IUEROOT}/vxl/testlib/config.mk

TESTMAIN = test_driver.cxx

# general reader/saver tester
TESTS += test_save_load_image.cxx

# specific testers:

# pnm:
TESTS += test_load.cxx
#memory_image:
TESTS += test_memory_image.cxx
TESTS += test_memory_image_of.cxx
#other:
TESTS += test_write_endian.cxx
TESTS += test_interpolate.cxx
TESTS += test_flipud.cxx
TESTS += test_colour_space.cxx
TESTS += test_copy.cxx
PARAMS_test_copy := square.pgm
TESTS += test_file_format_read.cxx
PARAMS_test_file_format_read := file_read_data
TESTS += test_include.cxx
TESTS += test_assign.cxx

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
