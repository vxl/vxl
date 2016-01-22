# This is core/vcsl/tests/CMakeLists.txt

LINK_LIBRARIES( vcsl vnl testlib )

ADD_EXECUTABLE( vcsl_test_all
  test_driver.cxx
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

ADD_TEST( NAME vcsl_test_meter COMMAND $<TARGET_FILE:vcsl_test_all> test_meter)
ADD_TEST( NAME vcsl_test_radian COMMAND $<TARGET_FILE:vcsl_test_all> test_radian)
ADD_TEST( NAME vcsl_test_degree COMMAND $<TARGET_FILE:vcsl_test_all> test_degree)
ADD_TEST( NAME vcsl_test_cartesian_2d COMMAND $<TARGET_FILE:vcsl_test_all> test_cartesian_2d)
ADD_TEST( NAME vcsl_test_cartesian_3d COMMAND $<TARGET_FILE:vcsl_test_all> test_cartesian_3d)
ADD_TEST( NAME vcsl_test_polar COMMAND $<TARGET_FILE:vcsl_test_all> test_polar)
ADD_TEST( NAME vcsl_test_cylindrical COMMAND $<TARGET_FILE:vcsl_test_all> test_cylindrical)
ADD_TEST( NAME vcsl_test_spherical COMMAND $<TARGET_FILE:vcsl_test_all> test_spherical)
ADD_TEST( NAME vcsl_test_transformation_graph COMMAND $<TARGET_FILE:vcsl_test_all> test_transformation_graph)
ADD_TEST( NAME vcsl_test_composition COMMAND $<TARGET_FILE:vcsl_test_all> test_composition)
ADD_TEST( NAME vcsl_test_scale COMMAND $<TARGET_FILE:vcsl_test_all> test_scale)
ADD_TEST( NAME vcsl_test_translation COMMAND $<TARGET_FILE:vcsl_test_all> test_translation)
ADD_TEST( NAME vcsl_test_rotation COMMAND $<TARGET_FILE:vcsl_test_all> test_rotation)
ADD_TEST( NAME vcsl_test_displacement COMMAND $<TARGET_FILE:vcsl_test_all> test_displacement)

ADD_EXECUTABLE( vcsl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vcsl_test_include vcsl )
