#   DESCRIPTION:
#
#   This is the vepl2/examples directory, containing examples programs for the
# vepl2 ("easy" image processing with vil) library.  - PVr.
#
#   DEPENDENCIES:  vepl2 vipl vil

LINK_LIBRARIES( vepl2 vil )

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

IF(CMAKE_COMPILER_IS_GNUCXX)
  SET_SOURCE_FILES_PROPERTIES(example_convert.cxx PROPERTIES COMPILE_FLAGS -O0)
  SET_SOURCE_FILES_PROPERTIES(example_std_dev.cxx PROPERTIES COMPILE_FLAGS -O0)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)
