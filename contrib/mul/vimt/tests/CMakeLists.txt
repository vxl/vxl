# This is mul/vimt/tests/CMakeLists.txt

ADD_EXECUTABLE( vimt_test_driver
  test_driver.cxx

  test_image_2d_of.cxx
  test_sample_profile_bilin.cxx
  test_sample_grid_bilin.cxx
  test_transform_2d.cxx
	test_gaussian_pyramid_builder_2d.cxx
)
TARGET_LINK_LIBRARIES( vimt_test_driver vimt vpl vul testlib vcl )

ADD_TEST( vimt_test_image_2d_of             vimt_test_driver test_image_2d_of )
ADD_TEST( vimt_test_sample_profile_bilin    vimt_test_driver test_sample_profile_bilin )
ADD_TEST( vimt_test_sample_grid_bilin       vimt_test_driver test_sample_grid_bilin )
ADD_TEST( vimt_test_transform_2d            vimt_test_driver test_transform_2d )
ADD_TEST( test_gaussian_pyramid_builder_2d  vimt_test_driver test_gaussian_pyramid_builder_2d )
