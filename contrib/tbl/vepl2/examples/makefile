#   DESCRIPTION:
#
#   This is the vepl2/examples directory, containing examples programs for the
# vepl2 (image processing) library.
#
# This should make it clear how to use the vipl filter classes with The vil2
# image classes.  Examples using those filter classes are in this directory.
# - PVr.
#
#   DEPENDENCIES:  vepl2 vipl vil2

USES += contrib/tbl core
include ../config.mk
include ${IUEROOT}/core/vpl/config.mk

# Do not instantiate templates in the library but implicitly, on use:
USE_IMPLICIT_TEMPLATES := 1

MINI_PROG_SOURCES += example_convert.cxx
MINI_PROG_SOURCES += example_add_random_noise.cxx
MINI_PROG_SOURCES += example_std_dev.cxx
MINI_PROG_SOURCES += example1_threshold.cxx
MINI_PROG_SOURCES += example4_threshold.cxx
MINI_PROG_SOURCES += example3_threshold.cxx
MINI_PROG_SOURCES += example_median.cxx
MINI_PROG_SOURCES += example_gradient_mag.cxx
MINI_PROG_SOURCES += example_sobel.cxx
MINI_PROG_SOURCES += example_dilate_disk.cxx
MINI_PROG_SOURCES += example_erode_disk.cxx
MINI_PROG_SOURCES += example2_threshold.cxx
MINI_PROG_SOURCES += example_x_gradient.cxx
MINI_PROG_SOURCES += example_histogram.cxx

################################
include ${IUEROOT}/config/top.mk
################################

ifeq "$(TJ_GCC30)" "1"
# gcc 3.0 needs too much virtual memory with -O2
FLAGS_example_convert.cxx := -O0
FLAGS_example_std_dev.cxx := -O0
endif
