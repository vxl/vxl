USES = vxl
include ${IUEROOT}/vxl/vil/config.mk

TESTMAIN = test_driver.cxx

# general reader/saver tester
SOURCES += test_save_load_image.cxx

# specific testers:

# pnm:
SOURCES += test_load.cxx
#memory_image:
SOURCES += test_memory_image.cxx
SOURCES += test_memory_image_of.cxx
#other:
SOURCES += test_write_endian.cxx
SOURCES += test_interpolate.cxx
SOURCES += test_flipud.cxx
SOURCES += test_colour_space.cxx
SOURCES += test_copy.cxx
SOURCES += test_file_format_read.cxx
SOURCES += test_include.cxx
SOURCES += test_assign.cxx

PARAMS_test_driver := test_save_load_image
PARAMS_test_copy := square.pgm
PARAMS_test_file_format_read := file_read_data

IULIBS = -lvil -lvpl -lvul -ltestlib

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
