# This is mul/mmn/tests/CMakeLists.txt

ADD_EXECUTABLE( mmn_test_all
  test_driver.cxx

  test_graph_rep1.cxx
  test_dp_solver.cxx
)
TARGET_LINK_LIBRARIES( mmn_test_all mmn testlib )

ADD_TEST( mmn_test_graph_rep1       ${EXECUTABLE_OUTPUT_PATH}/mmn_test_all test_graph_rep1 )
ADD_TEST( mmn_test_dp_solver   ${EXECUTABLE_OUTPUT_PATH}/mmn_test_all test_dp_solver )

ADD_EXECUTABLE( mmn_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( mmn_test_include mmn )
