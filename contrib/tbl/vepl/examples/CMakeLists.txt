#   DESCRIPTION:
#
#   This is the vepl/examples directory, containing examples programs for the
#   vepl ("easy" image processing with vil) library.  - PVr.
#
#   DEPENDENCIES:  vepl vipl vil

link_libraries( vepl ${VXL_LIB_PREFIX}vil )

add_definitions(-DINSTANTIATE_TEMPLATES)

add_executable(vepl_example_convert          example_convert.cxx)
add_executable(vepl_example_add_random_noise example_add_random_noise.cxx)
add_executable(vepl_example_std_dev          example_std_dev.cxx)
add_executable(vepl_example1_threshold       example1_threshold.cxx)
add_executable(vepl_example4_threshold       example4_threshold.cxx)
add_executable(vepl_example3_threshold       example3_threshold.cxx)
add_executable(vepl_example_median           example_median.cxx)
add_executable(vepl_example_gradient_mag     example_gradient_mag.cxx)
add_executable(vepl_example_sobel            example_sobel.cxx)
add_executable(vepl_example_dilate_disk      example_dilate_disk.cxx)
add_executable(vepl_example_erode_disk       example_erode_disk.cxx)
add_executable(vepl_example2_threshold       example2_threshold.cxx)
add_executable(vepl_example_x_gradient       example_x_gradient.cxx)
add_executable(vepl_example_histogram        example_histogram.cxx)

if(CMAKE_COMPILER_IS_GNUCXX)
  set_source_files_properties(example_convert.cxx PROPERTIES COMPILE_FLAGS -O0)
  set_source_files_properties(example_std_dev.cxx PROPERTIES COMPILE_FLAGS -O0)
endif()
