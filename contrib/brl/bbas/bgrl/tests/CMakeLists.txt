# This brl/bbas/bgrl/tests/CMakeLists.txt

INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR} )

ADD_EXECUTABLE ( bgrl_test_all
  test_driver.cxx
  test_graph.cxx
  test_search.cxx
)
TARGET_LINK_LIBRARIES( bgrl_test_all bgrl vpl vbl_io vsl testlib )

ADD_TEST( bgrl_test_graph   ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/bgrl_test_all test_graph )
ADD_TEST( bgrl_test_search  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/bgrl_test_all test_search )

ADD_EXECUTABLE( bgrl_test_include test_include.cxx)
TARGET_LINK_LIBRARIES( bgrl_test_include bgrl)
