USES += core
include ../config.mk
include ${VXLROOT}/core/vil/config.mk # only for test_vil.cxx
include ${VXLROOT}/core/vul/config.mk
include ${VXLROOT}/core/vpl/config.mk
include ${VXLROOT}/core/testlib/config.mk

TESTMAIN = test_driver.cxx

# general reader/saver tester
TESTS += test_save_load_image.cxx

# interface with vil:
TESTS += test_vil.cxx
PARAMS_test_vil := file_read_data

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
TESTS += test_assign.cxx
TESTS += test_load_gen.cxx
TESTS += test_resample.cxx
PARAMS_test_resample := square.pgm

#MINI_PROG_SOURCES += vil_vil1_compare_access_timings.cxx
#MINI_PROG_SOURCES += vil_vil1_compare_function_timings.cxx

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
