# This brl/bbas/bgrl/tests/CMakeLists.txt

INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas )

ADD_EXECUTABLE ( bgrl_test_driver
  test_driver.cxx
  test_graph.cxx
)
TARGET_LINK_LIBRARIES( bgrl_test_driver bgrl testlib vpl )

ADD_TEST( bgrl_graph                 bgrl_test_driver test_graph)

#this is for test_includes
ADD_EXECUTABLE( bgrl_test_include test_include.cxx)
TARGET_LINK_LIBRARIES( bgrl_test_include bgrl)
