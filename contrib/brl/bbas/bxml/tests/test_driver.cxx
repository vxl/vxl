#include <testlib/testlib_register.h>

DECLARE(bxml_test_map);
DECLARE(bxml_test_vdgl_digital_curve_input_converter);
DECLARE(bxml_test_vsol_point_2d_input_converter);
DECLARE(bxml_test_vtol_vertex_2d_input_converter);
DECLARE(bxml_test_vtol_zero_chain_2d_input_converter);
DECLARE(bxml_test_vtol_edge_2d_input_converter);
DECLARE(bxml_test_vector_vtol_edge_2d_input_conv);

void
register_tests()
{
  REGISTER(bxml_test_map);
  REGISTER(bxml_test_vdgl_digital_curve_input_converter);
  REGISTER(bxml_test_vsol_point_2d_input_converter);
  REGISTER(bxml_test_vtol_vertex_2d_input_converter);
  REGISTER(bxml_test_vtol_zero_chain_2d_input_converter);
  REGISTER(bxml_test_vtol_edge_2d_input_converter);
  REGISTER(bxml_test_vector_vtol_edge_2d_input_conv);
}

DEFINE_MAIN;
