# DESCRIPTION:
#
#   This is the vipl library, containing the (templated) image processing
# algorithm implementations for vxl, but completely independent from vil
# and thus also usable by other image representations, e.g., TargetJr or IUE.
#
# Three subdirectories, vipl_with_vnl_matrix, vipl_with_section and
# vipl_with_vbl_array_2d, create libraries with specialisations and
# instantiations for using other classes (instead of vil_image) as
# template arguments to the filter classes.
# The subdirectory Templates contains the instantiations for using vil_image.
# Those (and only those) go into the "standard" library vipl.
#
# The examples subdirectory contains example programs showing how to use the
# filter classes with any 2D storage class of your choice.
#
ifneq (,$(wildcard $(IUEROOT)/Image/ImageProcessingBasics))
SUBDIRS += vipl_with_section
endif
SUBDIRS += vipl_with_vnl_matrix
SUBDIRS += vipl_with_vbl_array_2d
SUBDIRS += examples

USES += vxl

LIBRARY := vipl
VERSION := 1.0

# A. Point operators:
MANPAGE_SOURCES += vipl_convert.txx
MANPAGE_SOURCES += vipl_threshold.txx
MANPAGE_SOURCES += vipl_add_random_noise.txx
MANPAGE_SOURCES += vipl_monadic.txx
MANPAGE_SOURCES += vipl_dyadic.txx

# B. Convolutions:
MANPAGE_SOURCES += vipl_erode_disk.txx
MANPAGE_SOURCES += vipl_dilate_disk.txx
MANPAGE_SOURCES += vipl_sobel.txx
MANPAGE_SOURCES += vipl_gaussian_convolution.txx
MANPAGE_SOURCES += vipl_x_gradient.txx
MANPAGE_SOURCES += vipl_y_gradient.txx
MANPAGE_SOURCES += vipl_gradient_mag.txx
MANPAGE_SOURCES += vipl_gradient_dir.txx
MANPAGE_SOURCES += vipl_median.txx

# C. Moments:
MANPAGE_SOURCES += vipl_moment.txx

# Z. Miscellaneous:
MANPAGE_SOURCES += vipl_histogram.txx

################################
include ${IUEROOT}/config/top.mk
################################

# compile with -O2 for efficiency:
#debug = $(optimize)

