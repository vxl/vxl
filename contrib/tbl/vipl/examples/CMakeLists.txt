#   DESCRIPTION:
#
#   This is the vipl/examples directory, containing examples programs for the
# vipl (image processing) library.
#
# This should make it clear how to use the filter classes with any 2D storage
# class of your choice.  Examples using those filter classes, templated on
# either vil_image_view<T>, vil1_image, vnl_matrix<T>, section<T,2> or
# vbl_array_2d<T> are in this directory.  - PVr.
#
#   DEPENDENCIES:  vipl
#                  vil  (for image load/save and for group A)
#                  vil1                         (for group B)
#                  vipl_with_vnl_matrix vnl     (for group C)
#                  vipl_with_vbl_array_2d vbl   (for group D)
#                  vipl_with_section            (for group E)

ADD_DEFINITIONS(-DINSTANTIATE_TEMPLATES)

# A. using vil_image_view<T> as 2D storage class:
ADD_EXECUTABLE(vipl_example_convert example_convert.cxx)
TARGET_LINK_LIBRARIES( vipl_example_convert vipl vil )

ADD_EXECUTABLE(vipl_example_add_random_noise example_add_random_noise.cxx)
TARGET_LINK_LIBRARIES( vipl_example_add_random_noise vipl vil )

ADD_EXECUTABLE(vipl_example_std_dev example_std_dev.cxx)
TARGET_LINK_LIBRARIES( vipl_example_std_dev vipl vil )

ADD_EXECUTABLE(vipl_example1_threshold example1_threshold.cxx)
TARGET_LINK_LIBRARIES( vipl_example1_threshold vipl vil )


# B. using vil1_image as 2D storage class:
ADD_EXECUTABLE(vipl_example4_threshold example4_threshold.cxx)
TARGET_LINK_LIBRARIES( vipl_example4_threshold vipl vil1 )


# C. using vnl_matrix<T> as 2D storage class:
ADD_EXECUTABLE(vipl_example3_threshold example3_threshold.cxx)
TARGET_LINK_LIBRARIES( vipl_example3_threshold vipl vnl )

ADD_EXECUTABLE(vipl_example_median example_median.cxx)
TARGET_LINK_LIBRARIES( vipl_example_median vipl vnl )

ADD_EXECUTABLE(vipl_example_gaussian_convolution example_gaussian_convolution.cxx)
TARGET_LINK_LIBRARIES( vipl_example_gaussian_convolution vipl vnl )


# D. using vbl_array_2d<T> as 2D storage class:
ADD_EXECUTABLE(vipl_example_gradient_mag example_gradient_mag.cxx)
TARGET_LINK_LIBRARIES( vipl_example_gradient_mag vipl vbl )

ADD_EXECUTABLE(vipl_example_sobel example_sobel.cxx)
TARGET_LINK_LIBRARIES( vipl_example_sobel vipl vbl )


# E. using section<T,2> as 2D storage class:
ADD_EXECUTABLE(vipl_example_dilate_disk example_dilate_disk.cxx)
TARGET_LINK_LIBRARIES( vipl_example_dilate_disk vipl )

ADD_EXECUTABLE(vipl_example_erode_disk example_erode_disk.cxx)
TARGET_LINK_LIBRARIES( vipl_example_erode_disk vipl )

ADD_EXECUTABLE(vipl_example2_threshold example2_threshold.cxx)
TARGET_LINK_LIBRARIES( vipl_example2_threshold vipl )

ADD_EXECUTABLE(vipl_example_x_gradient example_x_gradient.cxx)
TARGET_LINK_LIBRARIES( vipl_example_x_gradient vipl )

ADD_EXECUTABLE(vipl_example_histogram example_histogram.cxx)
TARGET_LINK_LIBRARIES( vipl_example_histogram vipl )

ADD_EXECUTABLE(vipl_example_histogram2 example_histogram2.cxx)
TARGET_LINK_LIBRARIES( vipl_example_histogram2 vipl )
