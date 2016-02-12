# vpgl/icam/tests/CMakeLists.txt

add_executable( icam_test_all
  test_driver.cxx
  test_minimizer.cxx
  test_icam_transform.cxx
  test_cylinder_map.cxx
  test_spherical_map.cxx
)

target_link_libraries( icam_test_all icam depth_map ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}testlib)

add_test( NAME icam_test_minimizer     COMMAND $<TARGET_FILE:icam_test_all> test_minimizer )
add_test( NAME icam_test_transform     COMMAND $<TARGET_FILE:icam_test_all> test_icam_transform )
add_test( NAME icam_test_cylinder_map  COMMAND $<TARGET_FILE:icam_test_all> test_cylinder_map )
add_test( NAME icam_test_spherical_map COMMAND $<TARGET_FILE:icam_test_all> test_spherical_map )

add_executable( icam_test_include test_include.cxx )
target_link_libraries( icam_test_include icam)
