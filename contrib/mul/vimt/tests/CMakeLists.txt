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
  test_v2i.cxx
  test_reflect.cxx
)
TARGET_LINK_LIBRARIES( vimt_test_all vimt_algo vgl vnl vil vpl vsl vul testlib )

ADD_TEST( NAME vimt_test_image_2d_of COMMAND $<TARGET_FILE:vimt_test_all> test_image_2d_of )
ADD_TEST( NAME vimt_test_sample_profile_bilin COMMAND $<TARGET_FILE:vimt_test_all> test_sample_profile_bilin )
ADD_TEST( NAME vimt_test_sample_grid_bilin COMMAND $<TARGET_FILE:vimt_test_all> test_sample_grid_bilin )
ADD_TEST( NAME vimt_test_transform_2d COMMAND $<TARGET_FILE:vimt_test_all> test_transform_2d )
ADD_TEST( NAME vimt_test_gaussian_pyramid_builder_2d COMMAND $<TARGET_FILE:vimt_test_all> test_gaussian_pyramid_builder_2d )
ADD_TEST( NAME vimt_test_dog_pyramid_builder_2d COMMAND $<TARGET_FILE:vimt_test_all> test_dog_pyramid_builder_2d )
ADD_TEST( NAME vimt_test_scale_pyramid_builder_2d COMMAND $<TARGET_FILE:vimt_test_all> test_scale_pyramid_builder_2d )
ADD_TEST( NAME vimt_test_gaussian_pyramid_builder_2d_general COMMAND $<TARGET_FILE:vimt_test_all> test_gaussian_pyramid_builder_2d_general )
ADD_TEST( NAME vimt_test_find_peaks COMMAND $<TARGET_FILE:vimt_test_all> test_find_peaks )
ADD_TEST( NAME vimt_test_find_troughs COMMAND $<TARGET_FILE:vimt_test_all> test_find_troughs )
ADD_TEST( NAME vimt_test_correlate_2d COMMAND $<TARGET_FILE:vimt_test_all> test_correlate_2d )
ADD_TEST( NAME vimt_test_resample_bilin COMMAND $<TARGET_FILE:vimt_test_all> test_resample_bilin )
ADD_TEST( NAME vimt_test_image_bounds_and_centre_2d COMMAND $<TARGET_FILE:vimt_test_all> test_image_bounds_and_centre_2d )
ADD_TEST( NAME vimt_test_v2i COMMAND $<TARGET_FILE:vimt_test_all> test_v2i )
ADD_TEST( NAME vimt_test_reflect COMMAND $<TARGET_FILE:vimt_test_all> test_reflect )

ADD_EXECUTABLE( vimt_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vimt_test_include vimt_algo vimt )
ADD_EXECUTABLE( vimt_test_template_include test_template_include.cxx )
TARGET_LINK_LIBRARIES( vimt_test_template_include vimt_algo vimt )
