# gel/vtol/tests/CMakeLists.txt

ADD_EXECUTABLE( vtol_test_all
  test_driver.cxx
  test_vertex_2d.cxx
  test_zero_chain.cxx
  test_edge_2d.cxx
  test_one_chain.cxx
  test_face_2d.cxx
  test_two_chain.cxx
  test_block.cxx
  test_cycle_processor.cxx
  vtol_test_refcounting.cxx
  vtol_test_timing.cxx

  test_extract_topology.cxx
)
IF(CMAKE_COMPILER_IS_GNUCXX)
 SET_SOURCE_FILES_PROPERTIES(test_cycle_processor.cxx PROPERTIES COMPILE_FLAGS -O1)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

TARGET_LINK_LIBRARIES( vtol_test_all vtol_algo vtol vsol vnl testlib )

ADD_TEST(vtol_test_vertex_2d        vtol_test_all test_vertex_2d)
ADD_TEST(vtol_test_zero_chain       vtol_test_all test_zero_chain)
ADD_TEST(vtol_test_edge_2d          vtol_test_all test_edge_2d)
ADD_TEST(vtol_test_one_chain        vtol_test_all test_one_chain)
ADD_TEST(vtol_test_face_2d          vtol_test_all test_face_2d)
ADD_TEST(vtol_test_two_chain        vtol_test_all test_two_chain)
ADD_TEST(vtol_test_block            vtol_test_all test_block)
ADD_TEST(vtol_test_cycle_processor  vtol_test_all test_cycle_processor)
ADD_TEST(vtol_test_refcounting      vtol_test_all vtol_test_refcounting)
ADD_TEST(vtol_test_timing           vtol_test_all vtol_test_timing)
ADD_TEST(vtol_test_extract_topology vtol_test_all test_extract_topology)

ADD_EXECUTABLE( vtol_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vtol_test_include vtol_algo vtol )
