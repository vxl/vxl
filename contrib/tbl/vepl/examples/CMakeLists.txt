#   DESCRIPTION:
#
#   This is the vepl/examples directory, containing examples programs for the
# vepl (image processing) library.
#
# This should make it clear how to use the filter classes with any 2D storage
# class of your choice.  Examples using those filter classes, templated on
# either vil_image, vnl_matrix<T>, section<T,2> or vbl_array_2d<T> are in this
# directory.  - PVr.
#
#   DEPENDENCIES:  vepl vipl vil

INCLUDE( ${allvxl_SOURCE_DIR}/tbl/vepl/CMakeListsLink.txt )

ADD_EXECUTABLE(example_convert          example_convert.cxx)
ADD_EXECUTABLE(example_add_random_noise example_add_random_noise.cxx)
ADD_EXECUTABLE(example_std_dev          example_std_dev.cxx)
ADD_EXECUTABLE(example1_threshold       example1_threshold.cxx)
ADD_EXECUTABLE(example4_threshold       example4_threshold.cxx)
ADD_EXECUTABLE(example3_threshold       example3_threshold.cxx)
ADD_EXECUTABLE(example_median           example_median.cxx)
ADD_EXECUTABLE(example_gradient_mag     example_gradient_mag.cxx)
ADD_EXECUTABLE(example_sobel            example_sobel.cxx)
ADD_EXECUTABLE(example_dilate_disk      example_dilate_disk.cxx)
ADD_EXECUTABLE(example_erode_disk       example_erode_disk.cxx)
ADD_EXECUTABLE(example2_threshold       example2_threshold.cxx)
ADD_EXECUTABLE(example_x_gradient       example_x_gradient.cxx)
ADD_EXECUTABLE(example_histogram        example_histogram.cxx)
