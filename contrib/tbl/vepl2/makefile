# DESCRIPTION:
#
#   This is the vepl2 library, containing the `easy' image processing
# algorithm interface classes based on vipl, specialised for vil2.
#
# These classes are derived from the vipl classes with a similar name.
#
# The examples subdirectory contains example programs showing how to use the
# filter classes with vil2 images.
#
USES += vxl tbl mul
# Make sure all template instantiations are available in this library:
USE_IMPLICIT_TEMPLATES := 1

LIBRARY := vepl2
VERSION := 1.0

# A. Point operators:
MANPAGE_SOURCES += vepl2_convert.txx
SOURCES += vepl2_threshold.cxx
SOURCES += vepl2_add_random_noise.cxx
SOURCES += vepl2_monadic.cxx
SOURCES += vepl2_dyadic.cxx

# B. Convolutions:
SOURCES += vepl2_erode_disk.cxx
SOURCES += vepl2_dilate_disk.cxx
SOURCES += vepl2_sobel.cxx
SOURCES += vepl2_gaussian_convolution.cxx
SOURCES += vepl2_x_gradient.cxx
SOURCES += vepl2_y_gradient.cxx
SOURCES += vepl2_gradient_mag.cxx
SOURCES += vepl2_gradient_dir.cxx
SOURCES += vepl2_median.cxx

# C. Moments:
SOURCES += vepl2_moment.cxx

# Z. Miscellaneous:
SOURCES += vepl2_histogram.cxx

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifeq "$(TJ_GCC30)$(optimize)" "1-O2"
# gcc 3.0 needs too much virtual memory with -O2
FLAGS_vepl2_threshold.cxx += -O1
FLAGS_vepl2_add_random_noise.cxx += -O1
FLAGS_vepl2_dyadic.cxx += -O1
FLAGS_vepl2_dilate_disk.cxx += -O1
FLAGS_vepl2_erode_disk.cxx += -O1
FLAGS_vepl2_median.cxx += -O1
FLAGS_vepl2_moment.cxx += -O1
FLAGS_vepl2_histogram.cxx += -O1
FLAGS_Templates/vepl2_convert+uchar-.cxx += -O1
FLAGS_Templates/vepl2_convert+short-.cxx += -O1
FLAGS_Templates/vepl2_convert+float-.cxx += -O1
FLAGS_Templates/vepl2_convert+vil_rgb+uchar--.cxx += -O1
endif

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
