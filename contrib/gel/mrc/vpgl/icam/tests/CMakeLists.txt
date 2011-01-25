# vpgl/icam/tests/CMakeLists.txt

ADD_EXECUTABLE( icam_test_all
  test_driver.cxx
  test_minimizer.cxx
  test_icam_transform.cxx
  test_icam_view_sphere.cxx
)

TARGET_LINK_LIBRARIES( icam_test_all icam testlib vul vnl vgl)
ADD_TEST( icam_test_minimizer         icam_test_all test_minimizer )
ADD_TEST( icam_test_transform         icam_test_all test_icam_transform )
ADD_TEST( icam_view_sphere            icam_test_all test_icam_view_sphere )

ADD_EXECUTABLE( icam_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( icam_test_include icam)
