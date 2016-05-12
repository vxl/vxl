# This is brl/bbas/bvgl/algo/tests/CMakeLists.txt

add_executable( bvgl_algo_test_all
  test_driver.cxx
  test_2d_geo_index.cxx
  test_biarc.cxx
  test_eulerspiral.cxx
)

target_link_libraries( bvgl_algo_test_all bvgl_algo ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}vnl_algo bnl ${VXL_LIB_PREFIX}testlib )

add_test( NAME bvgl_algo_test_2d_geo_index COMMAND $<TARGET_FILE:bvgl_algo_test_all> test_2d_geo_index )
add_test( NAME bvgl_algo_test_biarc COMMAND $<TARGET_FILE:bvgl_algo_test_all> test_biarc )
add_test( NAME bvgl_algo_test_eulerspiral COMMAND $<TARGET_FILE:bvgl_algo_test_all> test_eulerspiral )

add_executable( bvgl_algo_test_include test_include.cxx )
target_link_libraries( bvgl_algo_test_include bvgl_algo )
