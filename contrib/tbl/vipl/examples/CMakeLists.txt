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
#   DEPENDENCIES:  vipl
#                  vil  (for image load/save and for group A)
#                  vipl_with_vnl_matrix vnl     (for group B)
#                  vipl_with_vbl_array_2d vbl   (for group C)
#                  vipl_with_section            (for group D)

# A. using vil_image as 2D storage class:
ADD_EXECUTABLE(vipl_example_convert example_convert.cxx)
ADD_EXECUTABLE(vipl_example_add_random_noise example_add_random_noise.cxx)
ADD_EXECUTABLE(vipl_example_std_dev example_std_dev.cxx)
ADD_EXECUTABLE(vipl_example1_threshold example1_threshold.cxx)
ADD_EXECUTABLE(vipl_example4_threshold example4_threshold.cxx)

# B. using vnl_matrix<T> as 2D storage class:
ADD_EXECUTABLE(vipl_example3_threshold example3_threshold.cxx)
ADD_EXECUTABLE(vipl_example_median example_median.cxx)
ADD_EXECUTABLE(vipl_example_gaussian_convolution example_gaussian_convolution.cxx)

# C. using vbl_array_2d<T> as 2D storage class:
ADD_EXECUTABLE(vipl_example_gradient_mag example_gradient_mag.cxx)
ADD_EXECUTABLE(vipl_example_sobel example_sobel.cxx)

# D. using section<T,2> as 2D storage class:
ADD_EXECUTABLE(vipl_example_dilate_disk example_dilate_disk.cxx)
ADD_EXECUTABLE(vipl_example_erode_disk example_erode_disk.cxx)
ADD_EXECUTABLE(vipl_example2_threshold example2_threshold.cxx)
ADD_EXECUTABLE(vipl_example_x_gradient example_x_gradient.cxx)
ADD_EXECUTABLE(vipl_example_histogram example_histogram.cxx)

ADD_EXECUTABLE(vipl_example_histogram2 example_histogram2.cxx)

# For B:
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vnl/CMakeListsLink.txt )

# For C:
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vbl/CMakeListsLink.txt )

# For all:
INCLUDE( ${allvxl_SOURCE_DIR}/tbl/vipl/CMakeListsLink.txt )
