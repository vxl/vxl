# This is mul/vimt/tests/CMakeLists.txt

ADD_EXECUTABLE( vimt_test_all
  test_driver.cxx

  test_image_2d_of.cxx
  test_sample_profile_bilin.cxx
  test_sample_grid_bilin.cxx
  test_transform_2d.cxx
  test_gaussian_pyramid_builder_2d.cxx
  test_dog_pyramid_builder_2d.cxx
  test_scale_pyramid_builder_2d.cxx
  test_gaussian_pyramid_builder_2d_general.cxx
  test_find_peaks.cxx
  test_find_troughs.cxx
  test_correlate_2d.cxx
  test_resample_bilin.cxx
  test_image_bounds_and_centre_2d.cxx
)
TARGET_LINK_LIBRARIES( vimt_test_all vimt_algo vgl vnl vil vpl vsl testlib )

ADD_TEST( vimt_test_image_2d_of                  ${EXECUTABLE_OUTPUT_PATH}/vimt_test_all test_image_2d_of )
ADD_TEST( vimt_test_sample_profile_bilin         ${EXECUTABLE_OUTPUT_PATH}/vimt_test_all test_sample_profile_bilin )
ADD_TEST( vimt_test_sample_grid_bilin            ${EXECUTABLE_OUTPUT_PATH}/vimt_test_all test_sample_grid_bilin )
ADD_TEST( vimt_test_transform_2d                 ${EXECUTABLE_OUTPUT_PATH}/vimt_test_all test_transform_2d )
ADD_TEST( vimt_test_gaussian_pyramid_builder_2d  ${EXECUTABLE_OUTPUT_PATH}/vimt_test_all test_gaussian_pyramid_builder_2d )
ADD_TEST( vimt_test_dog_pyramid_builder_2d  ${EXECUTABLE_OUTPUT_PATH}/vimt_test_all test_dog_pyramid_builder_2d )
ADD_TEST( vimt_test_scale_pyramid_builder_2d     ${EXECUTABLE_OUTPUT_PATH}/vimt_test_all test_scale_pyramid_builder_2d )
ADD_TEST( vimt_test_gaussian_pyramid_builder_2d_general  ${EXECUTABLE_OUTPUT_PATH}/vimt_test_all test_gaussian_pyramid_builder_2d_general )
ADD_TEST( vimt_test_find_peaks                   ${EXECUTABLE_OUTPUT_PATH}/vimt_test_all test_find_peaks )
ADD_TEST( vimt_test_find_troughs                 ${EXECUTABLE_OUTPUT_PATH}/vimt_test_all test_find_troughs )
ADD_TEST( vimt_test_correlate_2d                 ${EXECUTABLE_OUTPUT_PATH}/vimt_test_all test_correlate_2d )
ADD_TEST( vimt_test_resample_bilin               ${EXECUTABLE_OUTPUT_PATH}/vimt_test_all test_resample_bilin )
ADD_TEST( vimt_test_image_bounds_and_centre_2d   ${EXECUTABLE_OUTPUT_PATH}/vimt_test_all test_image_bounds_and_centre_2d )

ADD_EXECUTABLE( vimt_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vimt_test_include vimt_algo vimt )
