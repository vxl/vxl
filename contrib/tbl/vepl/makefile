# DESCRIPTION:
#
#   This is the vepl library, containing the `easy' image processing
# algorithm interface classes based on vipl, specialised for vil.
#
# These classes are derived from the vipl classes with a similar name.
#
# The examples subdirectory contains example programs showing how to use the
# filter classes with vil images.

USES += contrib/tbl core
# Make sure all template instantiations are available in this library:
USE_IMPLICIT_TEMPLATES := 1

LIBRARY := vepl
VERSION := 1.0

# A. Point operators:
MANPAGE_SOURCES += vepl_convert.txx
SOURCES += vepl_threshold.cxx
SOURCES += vepl_add_random_noise.cxx
SOURCES += vepl_monadic.cxx
SOURCES += vepl_dyadic.cxx

# B. Convolutions:
SOURCES += vepl_erode_disk.cxx
SOURCES += vepl_dilate_disk.cxx
SOURCES += vepl_sobel.cxx
SOURCES += vepl_gaussian_convolution.cxx
SOURCES += vepl_x_gradient.cxx
SOURCES += vepl_y_gradient.cxx
SOURCES += vepl_gradient_mag.cxx
SOURCES += vepl_gradient_dir.cxx
SOURCES += vepl_median.cxx

# C. Moments:
SOURCES += vepl_moment.cxx

# Z. Miscellaneous:
SOURCES += vepl_histogram.cxx

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

# No need to compile the files in Templates when implicitly instantiating:
ifeq ($(USE_IMPLICIT_TEMPLATES),1)
  TEMPLATE_SOURCES :=
endif

ifndef NOTEST
SUBDIRS += examples
SUBDIRS += tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################

ifeq "$(TJ_GCC30)$(optimize)" "1-O2"
# gcc 3.0 needs too much virtual memory with -O2
FLAGS_vepl_threshold.cxx := -O1
FLAGS_vepl_add_random_noise.cxx := -O1
FLAGS_vepl_dyadic.cxx := -O1
FLAGS_vepl_dilate_disk.cxx := -O1
FLAGS_vepl_erode_disk.cxx := -O1
FLAGS_vepl_median.cxx := -O3
FLAGS_vepl_moment.cxx := -O1
FLAGS_vepl_histogram.cxx := -O0
FLAGS_Templates/vepl_convert+uchar-.cxx := -O1
FLAGS_Templates/vepl_convert+short-.cxx := -O1
FLAGS_Templates/vepl_convert+float-.cxx := -O1
FLAGS_Templates/vepl_convert+vil_rgb+uchar--.cxx := -O1
endif
