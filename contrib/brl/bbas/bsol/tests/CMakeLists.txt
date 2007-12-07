# brl/bbas/bsol/tests/CMakeLists.txt

INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR})

ADD_EXECUTABLE( bsol_test_all test_hough_index.cxx )
TARGET_LINK_LIBRARIES(bsol_test_all bsol vsol testlib )

ADD_TEST( bsol_test_hough_index  ${EXECUTABLE_OUTPUT_PATH}/bsol_test_all )

ADD_EXECUTABLE( bsol_test_include test_include.cxx )
TARGET_LINK_LIBRARIES(bsol_test_include bsol )
