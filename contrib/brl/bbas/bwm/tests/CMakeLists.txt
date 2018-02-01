# brl/bbas/bwm/tests/CMakeLists.txt

add_executable( bwm_test_all
                test_driver.cxx

              )
target_link_libraries(bwm_test_all bwm bwm_algo ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}testlib)

add_executable(bwm_test_include test_include.cxx)
target_link_libraries(bwm_test_include bwm bwm_algo)
