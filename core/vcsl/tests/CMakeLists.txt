# vxl/vcsl/tests/CMakeLists.txt

LINK_LIBRARIES( vcsl vnl testlib )

ADD_EXECUTABLE(vcsl_test_all
  test_driver.cxx
  vcsl_test_all.cxx
  test_meter.cxx
  test_radian.cxx
  test_degree.cxx
  test_cartesian_2d.cxx
  test_cartesian_3d.cxx
  test_polar.cxx
  test_cylindrical.cxx
  test_spherical.cxx
  test_transformation_graph.cxx
  test_composition.cxx
  test_scale.cxx
  test_translation.cxx
  test_rotation.cxx
  test_displacement.cxx)

ADD_TEST(vcsl_test_meter                vcsl_test_all test_meter)
ADD_TEST(vcsl_test_radian               vcsl_test_all test_radian)
ADD_TEST(vcsl_test_degree               vcsl_test_all test_degree)
ADD_TEST(vcsl_test_cartesian_2d         vcsl_test_all test_cartesian_2d)
ADD_TEST(vcsl_test_cartesian_3d         vcsl_test_all test_cartesian_3d)
ADD_TEST(vcsl_test_polar                vcsl_test_all test_polar)
ADD_TEST(vcsl_test_cylindrical          vcsl_test_all test_cylindrical)
ADD_TEST(vcsl_test_spherical            vcsl_test_all test_spherical)
ADD_TEST(vcsl_test_transformation_graph vcsl_test_all test_transformation_graph)
ADD_TEST(vcsl_test_composition          vcsl_test_all test_composition)
ADD_TEST(vcsl_test_scale                vcsl_test_all test_scale)
ADD_TEST(vcsl_test_translation          vcsl_test_all test_translation)
ADD_TEST(vcsl_test_rotation             vcsl_test_all test_rotation)
ADD_TEST(vcsl_test_displacement         vcsl_test_all test_displacement)
