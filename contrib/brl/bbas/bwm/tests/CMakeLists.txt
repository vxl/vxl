# brl/bbas/bwm/tests/CMakeLists.txt

add_executable( bwm_test_all
                test_driver.cxx
                test_move_proj_plane.cxx
                test_delaunay.cxx
              )
target_link_libraries(bwm_test_all bwm bwm_algo ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}testlib)

add_test( NAME bwm_test_move_proj_plane COMMAND $<TARGET_FILE:bwm_test_all> test_move_proj_plane ${CMAKE_CURRENT_SOURCE_DIR} )
add_test( NAME bwm_test_delaunay COMMAND $<TARGET_FILE:bwm_test_all> test_delaunay)

add_executable(bwm_test_include test_include.cxx)
target_link_libraries(bwm_test_include bwm bwm_algo)
