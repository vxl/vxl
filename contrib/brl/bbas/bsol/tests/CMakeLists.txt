# brl/bbas/bsol/tests/CMakeLists.txt

LINK_LIBRARIES( bsol vsol vgl vnl testlib )

ADD_EXECUTABLE(bsol_test_all test_hough_index.cxx)
ADD_TEST(bsol_test_hough_index  bsol_test_all)
