#   DESCRIPTION:
#
#   This is the vepl2/examples directory, containing examples programs for the
# vepl2 (image processing) library.
#
# This should make it clear how to use the filter classes with any 2D storage
# class of your choice.  Examples using those filter classes, templated on
# either vil_image, vnl_matrix<T>, section<T,2> or vbl_array_2d<T> are in this
# directory.  - PVr.
#
#   DEPENDENCIES:  vepl2 vipl vil

LINK_LIBRARIES( vepl2 )

ADD_DEFINITIONS(-DINSTANTIATE_TEMPLATES)

ADD_EXECUTABLE(vepl2_example_convert          example_convert.cxx)
ADD_EXECUTABLE(vepl2_example_add_random_noise example_add_random_noise.cxx)
ADD_EXECUTABLE(vepl2_example_std_dev          example_std_dev.cxx)
ADD_EXECUTABLE(vepl2_example1_threshold       example1_threshold.cxx)
ADD_EXECUTABLE(vepl2_example4_threshold       example4_threshold.cxx)
ADD_EXECUTABLE(vepl2_example3_threshold       example3_threshold.cxx)
ADD_EXECUTABLE(vepl2_example_median           example_median.cxx)
ADD_EXECUTABLE(vepl2_example_gradient_mag     example_gradient_mag.cxx)
ADD_EXECUTABLE(vepl2_example_sobel            example_sobel.cxx)
ADD_EXECUTABLE(vepl2_example_dilate_disk      example_dilate_disk.cxx)
ADD_EXECUTABLE(vepl2_example_erode_disk       example_erode_disk.cxx)
ADD_EXECUTABLE(vepl2_example2_threshold       example2_threshold.cxx)
ADD_EXECUTABLE(vepl2_example_x_gradient       example_x_gradient.cxx)
ADD_EXECUTABLE(vepl2_example_histogram        example_histogram.cxx)
