# This is brl/bbas/bvgl/algo/tests/CMakeLists.txt

ADD_EXECUTABLE( bvgl_algo_test_all
  test_driver.cxx
  test_2d_geo_index.cxx
  test_biarc.cxx
  test_eulerspiral.cxx
)

TARGET_LINK_LIBRARIES( bvgl_algo_test_all bvgl_algo vsl vnl_algo bnl testlib )

ADD_TEST( bvgl_algo_test_2d_geo_index ${EXECUTABLE_OUTPUT_PATH}/bvgl_algo_test_all test_2d_geo_index )
ADD_TEST( bvgl_algo_test_biarc         ${EXECUTABLE_OUTPUT_PATH}/bvgl_algo_test_all test_biarc )
ADD_TEST( bvgl_algo_test_eulerspiral   ${EXECUTABLE_OUTPUT_PATH}/bvgl_algo_test_all test_eulerspiral )

ADD_EXECUTABLE( bvgl_algo_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bvgl_algo_test_include bvgl_algo )
