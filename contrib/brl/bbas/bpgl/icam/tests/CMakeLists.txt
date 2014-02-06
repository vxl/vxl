# vpgl/icam/tests/CMakeLists.txt

ADD_EXECUTABLE( icam_test_all
  test_driver.cxx
  test_minimizer.cxx
  test_icam_transform.cxx
  test_cylinder_map.cxx
  test_spherical_map.cxx	
)

TARGET_LINK_LIBRARIES( icam_test_all icam depth_map vgl_algo vgl vnl vil vul testlib)

ADD_TEST( icam_test_minimizer         icam_test_all test_minimizer )
ADD_TEST( icam_test_transform         icam_test_all test_icam_transform )
ADD_TEST( icam_test_cylinder_map  icam_test_all test_cylinder_map )
ADD_TEST( icam_test_spherical_map  icam_test_all test_spherical_map )

ADD_EXECUTABLE( icam_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( icam_test_include icam)
