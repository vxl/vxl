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

ADD_TEST(vcsl_test_meter                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vcsl_test_all test_meter)
ADD_TEST(vcsl_test_radian               ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vcsl_test_all test_radian)
ADD_TEST(vcsl_test_degree               ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vcsl_test_all test_degree)
ADD_TEST(vcsl_test_cartesian_2d         ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vcsl_test_all test_cartesian_2d)
ADD_TEST(vcsl_test_cartesian_3d         ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vcsl_test_all test_cartesian_3d)
ADD_TEST(vcsl_test_polar                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vcsl_test_all test_polar)
ADD_TEST(vcsl_test_cylindrical          ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vcsl_test_all test_cylindrical)
ADD_TEST(vcsl_test_spherical            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vcsl_test_all test_spherical)
ADD_TEST(vcsl_test_transformation_graph ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vcsl_test_all test_transformation_graph)
ADD_TEST(vcsl_test_composition          ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vcsl_test_all test_composition)
ADD_TEST(vcsl_test_scale                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vcsl_test_all test_scale)
ADD_TEST(vcsl_test_translation          ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vcsl_test_all test_translation)
ADD_TEST(vcsl_test_rotation             ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vcsl_test_all test_rotation)
ADD_TEST(vcsl_test_displacement         ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vcsl_test_all test_displacement)

ADD_EXECUTABLE( vcsl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vcsl_test_include vcsl )
