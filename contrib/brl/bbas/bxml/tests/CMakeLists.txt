# brl/bbas/bxml/tests/CMakeLists.txt

LINK_LIBRARIES( bxml vtol vsol vnl )

ADD_EXECUTABLE(bxml_test_vsol_point_2d_input_converter
	       bxml_test_vsol_point_2d_input_converter.cxx)
ADD_TEST(bxml_test_vsol_point_2d_input converter 
         bxml_test_vsol_point_2d_input_converter)

ADD_EXECUTABLE(bxml_test_vdgl_digital_curve_input_converter
	       bxml_test_vdgl_digital_curve_input_converter.cxx)
ADD_TEST(bxml_test_vdgl_digital_curve_input converter 
         bxml_test_vdgl_digital_curve_input_converter)

ADD_EXECUTABLE(bxml_test_vtol_vertex_2d_input_converter
	       bxml_test_vtol_vertex_2d_input_converter.cxx)
ADD_TEST(bxml_test_vtol_vertex_2d_input_converter 
         bxml_test_vtol_vertex_2d_input_converter)

ADD_EXECUTABLE(bxml_test_vtol_zero_chain_2d_input_converter
	       bxml_test_vtol_zero_chain_2d_input_converter.cxx)
ADD_TEST(bxml_test_vtol_zero_chain_2d_input converter 
         bxml_test_vtol_zero_chain_2d_input_converter)

ADD_EXECUTABLE(bxml_test_vtol_edge_2d_input_converter
	       bxml_test_vtol_edge_2d_input_converter.cxx)
ADD_TEST(bxml_test_vtol_edge_2d_input_converter 
         bxml_test_vtol_edge_2d_input_converter)

ADD_EXECUTABLE(bxml_test_vector_vtol_edge_2d_input_conv
	       bxml_test_vector_vtol_edge_2d_input_conv.cxx)
ADD_TEST(bxml_test_vector_vtol_edge_2d_input_conv 
         bxml_test_vector_vtol_edge_2d_input_conv)

ADD_EXECUTABLE(bxml_test_map
	       bxml_test_map.cxx)
ADD_TEST(bxml_test_map
         bxml_test_map)
