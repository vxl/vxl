#   DESCRIPTION:
#
#   This is the vipl/examples directory, containing examples programs for the
# vipl (image processing) library.
#
# This should make it clear how to use the filter classes with any 2D storage
# class of your choice.  Examples using those filter classes, templated on
# either vil_image, vnl_matrix<T>, section<T,2> or vbl_array_2d<T> are in this
# directory.  - PVr.
#
#   DEPENDENCIES:  vipl (including template instances for groups B, C, D)
#                  vil  (for image load/save and for group A)

USES += contrib/tbl core
include ../config.mk
include ${IUEROOT}/core/vpl/config.mk # for all
include ${IUEROOT}/core/vnl/config.mk # for B.
include $(IUEROOT)/core/vbl/config.mk # for C.

# Do not instantiate templates in the library but implicitly, on use:
USE_IMPLICIT_TEMPLATES := 1

# A. using vil_image as 2D storage class:
MINI_PROG_SOURCES += example_convert.cxx
MINI_PROG_SOURCES += example_add_random_noise.cxx
MINI_PROG_SOURCES += example_std_dev.cxx
MINI_PROG_SOURCES += example1_threshold.cxx
MINI_PROG_SOURCES += example4_threshold.cxx

# B. using vnl_matrix<T> as 2D storage class:
MINI_PROG_SOURCES += example3_threshold.cxx
MINI_PROG_SOURCES += example_median.cxx
MINI_PROG_SOURCES += example_gaussian_convolution.cxx

# C. using vbl_array_2d<T> as 2D storage class:
MINI_PROG_SOURCES += example_gradient_mag.cxx
MINI_PROG_SOURCES += example_sobel.cxx

# D. using section<T,2> as 2D storage class:
MINI_PROG_SOURCES += example_dilate_disk.cxx
MINI_PROG_SOURCES += example_erode_disk.cxx
MINI_PROG_SOURCES += example2_threshold.cxx
MINI_PROG_SOURCES += example_x_gradient.cxx
MINI_PROG_SOURCES += example_histogram.cxx
MINI_PROG_SOURCES += example_histogram2.cxx

################################
include ${IUEROOT}/config/top.mk
################################
