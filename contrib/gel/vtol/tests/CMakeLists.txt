# gel/vtol/tests/CMakeLists.txt

LINK_LIBRARIES( vtol vsol vnl )

ADD_EXECUTABLE(vtol_test_vertex_2d       test_vertex_2d.cxx)
ADD_TEST(      vtol_test_vertex_2d  vtol_test_vertex_2d)
ADD_EXECUTABLE(vtol_test_zero_chain      test_zero_chain.cxx)
ADD_TEST(      vtol_test_zero_chain vtol_test_zero_chain)
ADD_EXECUTABLE(vtol_test_edge_2d         test_edge_2d.cxx)
ADD_TEST(      vtol_test_edge_2d    vtol_test_edge_2d)
ADD_EXECUTABLE(vtol_test_one_chain       test_one_chain.cxx)
ADD_TEST(      vtol_test_one_chain  vtol_test_one_chain)
ADD_EXECUTABLE(vtol_test_face_2d         test_face_2d.cxx)
ADD_TEST(      vtol_test_face_2d    vtol_test_face_2d)
ADD_EXECUTABLE(vtol_test_two_chain       test_two_chain.cxx)
ADD_TEST(      vtol_test_two_chain  vtol_test_two_chain)
ADD_EXECUTABLE(vtol_test_block           test_block.cxx)
ADD_TEST(      vtol_test_block      vtol_test_block)
