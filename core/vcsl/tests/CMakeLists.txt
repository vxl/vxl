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

ADD_TEST(vcsl_test_meter                ${EXECUTABLE_OUTPUT_PATH}/vcsl_test_all test_meter)
ADD_TEST(vcsl_test_radian               ${EXECUTABLE_OUTPUT_PATH}/vcsl_test_all test_radian)
ADD_TEST(vcsl_test_degree               ${EXECUTABLE_OUTPUT_PATH}/vcsl_test_all test_degree)
ADD_TEST(vcsl_test_cartesian_2d         ${EXECUTABLE_OUTPUT_PATH}/vcsl_test_all test_cartesian_2d)
ADD_TEST(vcsl_test_cartesian_3d         ${EXECUTABLE_OUTPUT_PATH}/vcsl_test_all test_cartesian_3d)
ADD_TEST(vcsl_test_polar                ${EXECUTABLE_OUTPUT_PATH}/vcsl_test_all test_polar)
ADD_TEST(vcsl_test_cylindrical          ${EXECUTABLE_OUTPUT_PATH}/vcsl_test_all test_cylindrical)
ADD_TEST(vcsl_test_spherical            ${EXECUTABLE_OUTPUT_PATH}/vcsl_test_all test_spherical)
ADD_TEST(vcsl_test_transformation_graph ${EXECUTABLE_OUTPUT_PATH}/vcsl_test_all test_transformation_graph)
ADD_TEST(vcsl_test_composition          ${EXECUTABLE_OUTPUT_PATH}/vcsl_test_all test_composition)
ADD_TEST(vcsl_test_scale                ${EXECUTABLE_OUTPUT_PATH}/vcsl_test_all test_scale)
ADD_TEST(vcsl_test_translation          ${EXECUTABLE_OUTPUT_PATH}/vcsl_test_all test_translation)
ADD_TEST(vcsl_test_rotation             ${EXECUTABLE_OUTPUT_PATH}/vcsl_test_all test_rotation)
ADD_TEST(vcsl_test_displacement         ${EXECUTABLE_OUTPUT_PATH}/vcsl_test_all test_displacement)

ADD_EXECUTABLE( vcsl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vcsl_test_include vcsl )
