# bseg/bsgm/tests/CMakeLists.txt

add_executable( bsgm_test_all
  test_driver.cxx
  test_error_checking.cxx
)

target_link_libraries( bsgm_test_all bsgm ${VXL_LIB_PREFIX}testlib)

add_test( NAME bsgm_test_error_checking COMMAND $<TARGET_FILE:bsgm_test_all> test_compute_invalid_map)

add_executable( bsgm_test_include test_include.cxx )
target_link_libraries( bsgm_test_include bsgm)
add_executable( bsgm_test_template_include test_template_include.cxx )
target_link_libraries( bsgm_test_template_include bsgm)
