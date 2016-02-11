# This is core/vcsl/tests/CMakeLists.txt

link_libraries( ${VXL_LIB_PREFIX}vcsl ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}testlib )

add_executable( vcsl_test_all
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

add_test( NAME vcsl_test_meter COMMAND $<TARGET_FILE:vcsl_test_all> test_meter)
add_test( NAME vcsl_test_radian COMMAND $<TARGET_FILE:vcsl_test_all> test_radian)
add_test( NAME vcsl_test_degree COMMAND $<TARGET_FILE:vcsl_test_all> test_degree)
add_test( NAME vcsl_test_cartesian_2d COMMAND $<TARGET_FILE:vcsl_test_all> test_cartesian_2d)
add_test( NAME vcsl_test_cartesian_3d COMMAND $<TARGET_FILE:vcsl_test_all> test_cartesian_3d)
add_test( NAME vcsl_test_polar COMMAND $<TARGET_FILE:vcsl_test_all> test_polar)
add_test( NAME vcsl_test_cylindrical COMMAND $<TARGET_FILE:vcsl_test_all> test_cylindrical)
add_test( NAME vcsl_test_spherical COMMAND $<TARGET_FILE:vcsl_test_all> test_spherical)
add_test( NAME vcsl_test_transformation_graph COMMAND $<TARGET_FILE:vcsl_test_all> test_transformation_graph)
add_test( NAME vcsl_test_composition COMMAND $<TARGET_FILE:vcsl_test_all> test_composition)
add_test( NAME vcsl_test_scale COMMAND $<TARGET_FILE:vcsl_test_all> test_scale)
add_test( NAME vcsl_test_translation COMMAND $<TARGET_FILE:vcsl_test_all> test_translation)
add_test( NAME vcsl_test_rotation COMMAND $<TARGET_FILE:vcsl_test_all> test_rotation)
add_test( NAME vcsl_test_displacement COMMAND $<TARGET_FILE:vcsl_test_all> test_displacement)

add_executable( vcsl_test_include test_include.cxx )
target_link_libraries( vcsl_test_include ${VXL_LIB_PREFIX}vcsl )
