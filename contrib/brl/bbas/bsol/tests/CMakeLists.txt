# brl/bbas/bsol/tests/CMakeLists.txt

include_directories(${BRL_INCLUDE_DIR})
include_directories(${BRL_INCLUDE_DIR})

add_executable( bsol_test_all test_hough_index.cxx )
target_link_libraries(bsol_test_all bsol vsol ${VXL_LIB_PREFIX}testlib )

add_test( NAME bsol_test_hough_index COMMAND $<TARGET_FILE:bsol_test_all> test_hough_index)

add_executable( bsol_test_include test_include.cxx )
target_link_libraries(bsol_test_include bsol )
