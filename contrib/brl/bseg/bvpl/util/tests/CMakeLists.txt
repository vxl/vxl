
add_executable( bvpl_util_test_all
  test_driver.cxx
  test_corner_finder.cxx
 )

target_link_libraries( bvpl_util_test_all testlib bvpl_util bvxm_grid_io vgl vul)

add_test( NAME bvpl_util_test_corner_finder COMMAND $<TARGET_FILE:bvpl_util_test_all> test_corner_finder  )

add_executable( bvpl_util_test_include test_include.cxx )
target_link_libraries( bvpl_util_test_include bvpl_util )
