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

add_definitions(-DINSTANTIATE_TEMPLATES)

# A. using vil_image_view<T> as 2D storage class:
if(VXL_BUILD_CORE_IMAGING)
  add_executable(vipl_example_convert example_convert.cxx)
  target_link_libraries( vipl_example_convert vipl ${VXL_LIB_PREFIX}vil )

  add_executable(vipl_example_add_random_noise example_add_random_noise.cxx)
  target_link_libraries( vipl_example_add_random_noise vipl ${VXL_LIB_PREFIX}vil )

  add_executable(vipl_example_std_dev example_std_dev.cxx)
  target_link_libraries( vipl_example_std_dev vipl ${VXL_LIB_PREFIX}vil )

  add_executable(vipl_example1_threshold example1_threshold.cxx)
  target_link_libraries( vipl_example1_threshold vipl ${VXL_LIB_PREFIX}vil )
endif()

# C. using vnl_matrix<T> as 2D storage class:
if(VXL_BUILD_CORE_NUMERICS)
  add_executable(vipl_example3_threshold example3_threshold.cxx)
  target_link_libraries( vipl_example3_threshold vipl ${VXL_LIB_PREFIX}vnl )

  add_executable(vipl_example_median example_median.cxx)
  target_link_libraries( vipl_example_median vipl ${VXL_LIB_PREFIX}vnl )

  add_executable(vipl_example_gaussian_convolution example_gaussian_convolution.cxx)
  target_link_libraries( vipl_example_gaussian_convolution vipl ${VXL_LIB_PREFIX}vnl )
endif()

# D. using vbl_array_2d<T> as 2D storage class:
if(VXL_BUILD_CORE_UTILITIES)
  add_executable(vipl_example_gradient_mag example_gradient_mag.cxx)
  target_link_libraries( vipl_example_gradient_mag vipl ${VXL_LIB_PREFIX}vbl )

  add_executable(vipl_example_sobel example_sobel.cxx)
  target_link_libraries( vipl_example_sobel vipl ${VXL_LIB_PREFIX}vbl )
endif()

# E. using section<T,2> as 2D storage class:
add_executable(vipl_example_dilate_disk example_dilate_disk.cxx)
target_link_libraries( vipl_example_dilate_disk vipl )

add_executable(vipl_example_erode_disk example_erode_disk.cxx)
target_link_libraries( vipl_example_erode_disk vipl )

add_executable(vipl_example2_threshold example2_threshold.cxx)
target_link_libraries( vipl_example2_threshold vipl )

add_executable(vipl_example_x_gradient example_x_gradient.cxx)
target_link_libraries( vipl_example_x_gradient vipl )

add_executable(vipl_example_histogram example_histogram.cxx)
target_link_libraries( vipl_example_histogram vipl )

add_executable(vipl_example_histogram2 example_histogram2.cxx)
target_link_libraries( vipl_example_histogram2 vipl )
