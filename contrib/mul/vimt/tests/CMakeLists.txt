# This is mul/vimt/tests/CMakeLists.txt

add_executable( vimt_test_all
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
target_link_libraries( vimt_test_all vimt_algo ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vpl ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}testlib )

add_test( NAME vimt_test_image_2d_of COMMAND $<TARGET_FILE:vimt_test_all> test_image_2d_of )
add_test( NAME vimt_test_sample_profile_bilin COMMAND $<TARGET_FILE:vimt_test_all> test_sample_profile_bilin )
add_test( NAME vimt_test_sample_grid_bilin COMMAND $<TARGET_FILE:vimt_test_all> test_sample_grid_bilin )
add_test( NAME vimt_test_transform_2d COMMAND $<TARGET_FILE:vimt_test_all> test_transform_2d )
add_test( NAME vimt_test_gaussian_pyramid_builder_2d COMMAND $<TARGET_FILE:vimt_test_all> test_gaussian_pyramid_builder_2d )
add_test( NAME vimt_test_dog_pyramid_builder_2d COMMAND $<TARGET_FILE:vimt_test_all> test_dog_pyramid_builder_2d )
add_test( NAME vimt_test_scale_pyramid_builder_2d COMMAND $<TARGET_FILE:vimt_test_all> test_scale_pyramid_builder_2d )
add_test( NAME vimt_test_gaussian_pyramid_builder_2d_general COMMAND $<TARGET_FILE:vimt_test_all> test_gaussian_pyramid_builder_2d_general )
add_test( NAME vimt_test_find_peaks COMMAND $<TARGET_FILE:vimt_test_all> test_find_peaks )
add_test( NAME vimt_test_find_troughs COMMAND $<TARGET_FILE:vimt_test_all> test_find_troughs )
add_test( NAME vimt_test_correlate_2d COMMAND $<TARGET_FILE:vimt_test_all> test_correlate_2d )
add_test( NAME vimt_test_resample_bilin COMMAND $<TARGET_FILE:vimt_test_all> test_resample_bilin )
add_test( NAME vimt_test_image_bounds_and_centre_2d COMMAND $<TARGET_FILE:vimt_test_all> test_image_bounds_and_centre_2d )
add_test( NAME vimt_test_v2i COMMAND $<TARGET_FILE:vimt_test_all> test_v2i )
add_test( NAME vimt_test_reflect COMMAND $<TARGET_FILE:vimt_test_all> test_reflect )

add_executable( vimt_test_include test_include.cxx )
target_link_libraries( vimt_test_include vimt_algo vimt )
add_executable( vimt_test_template_include test_template_include.cxx )
target_link_libraries( vimt_test_template_include vimt_algo vimt )
