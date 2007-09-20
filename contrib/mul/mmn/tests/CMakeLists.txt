# This is mul/mmn/tests/CMakeLists.txt

ADD_EXECUTABLE( mmn_test_all
  test_driver.cxx

  test_graph_rep1.cxx
)
TARGET_LINK_LIBRARIES( mmn_test_all mmn testlib )

ADD_TEST( mmn_test_arc                  ${EXECUTABLE_OUTPUT_PATH}/mmn_test_all test_arc )
ADD_TEST( mmn_test_searcher             ${EXECUTABLE_OUTPUT_PATH}/mmn_test_all test_searcher )

ADD_EXECUTABLE( mmn_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( mmn_test_include mmn )
