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

USES += vxl tbl mul

LIBRARY := vipl
VERSION := 1.0

# Do not instantiate templates in the library but implicitly, on use:
USE_IMPLICIT_TEMPLATES := 1

SOURCES += filter/vipl_filter_abs.cxx
MANPAGE_SOURCES += filter/vipl_filter_helper.h
MANPAGE_SOURCES += filter/vipl_trivial_pixeliter.h
MANPAGE_SOURCES += section/vipl_filterable_section_container_generator_vil_image.txx
MANPAGE_SOURCES += vipl_with_section/vipl_filterable_section_container_generator_section.txx
MANPAGE_SOURCES += vipl_with_vbl_array_2d/vipl_filterable_section_container_generator_vbl_array_2d.txx
MANPAGE_SOURCES += vipl_with_vnl_matrix/vipl_filterable_section_container_generator_vnl_matrix.txx

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

# Support classes
MANPAGE_SOURCES += filter/vipl_filter.txx
MANPAGE_SOURCES += filter/vipl_filter_2d.txx
MANPAGE_SOURCES += section/vipl_section_container.txx
MANPAGE_SOURCES += section/vipl_section_descriptor.txx
MANPAGE_SOURCES += section/vipl_section_descriptor_2d.txx
MANPAGE_SOURCES += section/vipl_section_iterator.txx

# Image accessors (getpixel, setpixel)
MANPAGE_SOURCES += accessors/vipl_accessors_vil_image.txx
MANPAGE_SOURCES += vipl_with_section/accessors/vipl_accessors_section.txx
MANPAGE_SOURCES += vipl_with_vbl_array_2d/accessors/vipl_accessors_vbl_array_2d.txx
MANPAGE_SOURCES += vipl_with_vnl_matrix/accessors/vipl_accessors_vnl_matrix.txx

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifeq "$(TJ_GCC30)$(optimize)" "1-O2"
# gcc 3.0 needs too much virtual memory with -O2
FLAGS_vipl_with_section/Templates/vipl_filter_2d+section+vil_rgb+uchar-.2-.section+vil_rgb+uchar-.2-.vil_rgb+uchar-.vil_rgb+uchar--.cxx += -O1
endif

# No need to compile the files in Templates when implicitly instantiating:
ifeq ($(USE_IMPLICIT_TEMPLATES),1)
  TEMPLATE_SOURCES :=
else
  # These 3 subdirectories are only needed for template instantiation,
  # used in the examples; leave them out if you don't need those examples,
  # or if you are using implicit instantiation:
  ifndef NOTEST
    AUX_SOURCE_DIRECTORY += vipl_with_vbl_array_2d/Templates
    AUX_SOURCE_DIRECTORY += vipl_with_vnl_matrix/Templates
    AUX_SOURCE_DIRECTORY += vipl_with_section/Templates
    AUX_SOURCE_DIRECTORY += vipl_with_mil/Templates
  endif
endif

ifndef NOTEST
SUBDIRS += examples
SUBDIRS += tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################

# compile with -O2 for efficiency:
#debug = $(optimize)
