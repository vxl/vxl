# brl/bbas/bxml/tests/CMakeLists.txt

LINK_LIBRARIES( bxml vtol vsol vnl testlib )

ADD_EXECUTABLE( bxml_test_all
  test_driver.cxx
  bxml_test_vsol_point_2d_input_converter.cxx
  bxml_test_vdgl_digital_curve_input_converter.cxx
  bxml_test_vtol_vertex_2d_input_converter.cxx
  bxml_test_vtol_zero_chain_2d_input_converter.cxx
  bxml_test_vtol_edge_2d_input_converter.cxx
  bxml_test_vector_vtol_edge_2d_input_conv.cxx
  bxml_test_map.cxx
)

ADD_TEST(bxml_test_vsol_point_2d_input_converter  bxml_test_all
         bxml_test_vsol_point_2d_input_converter ${CMAKE_CURRENT_SOURCE_DIR}/)
ADD_TEST(bxml_test_vdgl_digital_curve_input_converter  bxml_test_all
         bxml_test_vdgl_digital_curve_input_converter ${CMAKE_CURRENT_SOURCE_DIR}/)
ADD_TEST(bxml_test_vtol_vertex_2d_input_converter  bxml_test_all
         bxml_test_vtol_vertex_2d_input_converter ${CMAKE_CURRENT_SOURCE_DIR}/)
ADD_TEST(bxml_test_vtol_zero_chain_2d_input_converter  bxml_test_all
         bxml_test_vtol_zero_chain_2d_input_converter ${CMAKE_CURRENT_SOURCE_DIR}/)
ADD_TEST(bxml_test_vtol_edge_2d_input_converter  bxml_test_all
         bxml_test_vtol_edge_2d_input_converter ${CMAKE_CURRENT_SOURCE_DIR}/)
ADD_TEST(bxml_test_vector_vtol_edge_2d_input_conv  bxml_test_all
         bxml_test_vector_vtol_edge_2d_input_conv ${CMAKE_CURRENT_SOURCE_DIR}/)
ADD_TEST(bxml_test_map bxml_test_all
         bxml_test_map ${CMAKE_CURRENT_SOURCE_DIR}/)

ADD_EXECUTABLE( bxml_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bxml_test_include bxml )
