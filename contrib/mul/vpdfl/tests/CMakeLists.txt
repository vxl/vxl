# mul/vpdfl/tests/CMakeLists.txt

add_executable( vpdfl_test_all
  test_driver.cxx
  test_axis_gaussian.cxx
  test_gaussian.cxx
  test_mixture.cxx
  test_pc_gaussian.cxx
  )
target_link_libraries( vpdfl_test_all vpdfl mbl ${VXL_LIB_PREFIX}vnl_io ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}vpl ${VXL_LIB_PREFIX}testlib )

add_test( NAME vpdfl_test_axis_gaussian COMMAND $<TARGET_FILE:vpdfl_test_all> test_axis_gaussian )
add_test( NAME vpdfl_test_gaussian COMMAND $<TARGET_FILE:vpdfl_test_all> test_gaussian )
add_test( NAME vpdfl_test_mixture COMMAND $<TARGET_FILE:vpdfl_test_all> test_mixture )
add_test( NAME vpdfl_test_pc_gaussian COMMAND $<TARGET_FILE:vpdfl_test_all> test_pc_gaussian )

add_executable( vpdfl_test_include test_include.cxx )
target_link_libraries( vpdfl_test_include vpdfl )
