# This is mul/vil3d/tests/CMakeLists.txt

ADD_EXECUTABLE( vil3d_test_driver
  test_driver.cxx

  test_image_view.cxx
  test_trilin_interp.cxx
  test_sample_profile_trilin.cxx
  test_gauss_reduce.cxx
)
TARGET_LINK_LIBRARIES( vil3d_test_driver vil3d vil3d_algo vil2 vil2_algo vpl vul testlib vcl )

ADD_TEST( vil3d_test_image_view             vil3d_test_driver test_image_view )
ADD_TEST( vil3d_test_trilin_interp          vil3d_test_driver test_trilin_interp )
ADD_TEST( test_sample_profile_trilin        vil3d_test_driver test_sample_profile_trilin )
ADD_TEST( test_gauss_reduce                 vil3d_test_driver test_gauss_reduce )
