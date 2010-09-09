# brl/bbas/bwm/tests/CMakeLists.txt
IF (EXISTS ${lems_SOURCE_DIR}/contrib )
INCLUDE_DIRECTORIES(${lems_SOURCE_DIR}/contrib )
ENDIF (EXISTS ${lems_SOURCE_DIR}/contrib )

ADD_EXECUTABLE( bwm_test_all
                test_driver.cxx
                test_move_proj_plane.cxx
                test_delaunay.cxx
              )
TARGET_LINK_LIBRARIES(bwm_test_all bwm bwm_algo vnl_algo vnl vgl testlib)

ADD_TEST(bwm_test_move_proj_plane ${EXECUTABLE_OUTPUT_PATH}/bwm_test_all test_move_proj_plane ${CMAKE_CURRENT_SOURCE_DIR} )
ADD_TEST(bwm_test_delaunay ${EXECUTABLE_OUTPUT_PATH}/bwm_test_all test_delaunay)

ADD_EXECUTABLE(bwm_test_include test_include.cxx)
TARGET_LINK_LIBRARIES(bwm_test_include bwm bwm_algo)
