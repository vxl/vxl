# mul/mcal/tests/CMakeLists.txt

add_executable( mcal_test_all
  test_driver.cxx
  test_pca.cxx
  test_trivial_ca.cxx
  test_general_ca.cxx
  )
target_link_libraries( mcal_test_all mcal ${VXL_LIB_PREFIX}testlib )

add_test( NAME mcal_test_pca COMMAND $<TARGET_FILE:mcal_test_all> test_pca)
add_test( NAME mcal_test_trivial_ca COMMAND $<TARGET_FILE:mcal_test_all> test_trivial_ca)
add_test( NAME mcal_test_general_ca COMMAND $<TARGET_FILE:mcal_test_all> test_general_ca)

add_executable( mcal_test_include test_include.cxx )
target_link_libraries( mcal_test_include mcal )
