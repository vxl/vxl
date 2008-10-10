# brl/bbas/bwm/tests/CMakeLists.txt
INCLUDE_DIRECTORIES(${lems_SOURCE_DIR}/contrib )

ADD_EXECUTABLE( bwm_test_all
                test_driver.cxx
                test_move_proj_plane.cxx
                test_delaunay.cxx
              )
TARGET_LINK_LIBRARIES(bwm_test_all bwm bwm_algo testlib vgl)

ADD_TEST(test_move_proj_plane ${EXECUTABLE_OUTPUT_PATH}/bwm_test_all test_move_proj_plane)
ADD_TEST(test_delaunay ${EXECUTABLE_OUTPUT_PATH}/bwm_test_all test_delaunay)

ADD_EXECUTABLE(bwm_test_include test_include.cxx)
TARGET_LINK_LIBRARIES(bwm_test_include bwm bwm_algo)
