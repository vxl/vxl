# DESCRIPTION:
#
#   This is the vepl1 library, containing the `easy' image processing
# algorithm interface classes based on vipl, specialised for vil1.
#
# These classes are derived from the vipl classes with a similar name.
#
# The examples subdirectory contains example programs showing how to use the
# filter classes with vil1 images.

USES += contrib/tbl core
# Make sure all template instantiations are available in this library:
USE_IMPLICIT_TEMPLATES := 1

LIBRARY := vepl1
VERSION := 1.0

# A. Point operators:
MANPAGE_SOURCES += vepl1_convert.txx
SOURCES += vepl1_threshold.cxx
SOURCES += vepl1_add_random_noise.cxx
SOURCES += vepl1_monadic.cxx
SOURCES += vepl1_dyadic.cxx

# B. Convolutions:
SOURCES += vepl1_erode_disk.cxx
SOURCES += vepl1_dilate_disk.cxx
SOURCES += vepl1_sobel.cxx
SOURCES += vepl1_gaussian_convolution.cxx
SOURCES += vepl1_x_gradient.cxx
SOURCES += vepl1_y_gradient.cxx
SOURCES += vepl1_gradient_mag.cxx
SOURCES += vepl1_gradient_dir.cxx
SOURCES += vepl1_median.cxx

# C. Moments:
SOURCES += vepl1_moment.cxx

# Z. Miscellaneous:
SOURCES += vepl1_histogram.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
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
include ${VXLROOT}/config/top-rules.mk
######################################

ifeq "$(TJ_GCC30)" "1"
# gcc 3.0 needs too much virtual memory without optimisation
FLAGS_vepl1_threshold.cxx := -O2
FLAGS_vepl1_add_random_noise.cxx := -O1
FLAGS_vepl1_dyadic.cxx := -O3
FLAGS_vepl1_dilate_disk.cxx := -O1
FLAGS_vepl1_erode_disk.cxx := -O1
FLAGS_vepl1_median.cxx := -O3
FLAGS_vepl1_moment.cxx := -O1
FLAGS_vepl1_histogram.cxx := -O0
FLAGS_Templates/vepl1_convert+uchar-.cxx := -O1
FLAGS_Templates/vepl1_convert+short-.cxx := -O1
FLAGS_Templates/vepl1_convert+float-.cxx := -O1
FLAGS_Templates/vepl1_convert+vil_rgb+uchar--.cxx := -O1
endif
