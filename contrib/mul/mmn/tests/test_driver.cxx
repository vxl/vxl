#include <testlib/testlib_register.h>

DECLARE( test_graph_rep1 );
DECLARE( test_dp_solver );
DECLARE( test_lbp_solver );
DECLARE( test_order_cost );
DECLARE( test_csp_solver );
DECLARE( test_diffusion_solver );
DECLARE( test_make_tri_tree );
DECLARE( test_parse_arcs );

void register_tests()
{
  REGISTER( test_graph_rep1 );
  REGISTER( test_dp_solver );
  REGISTER( test_lbp_solver );
  REGISTER( test_order_cost );
  REGISTER( test_csp_solver );
  REGISTER( test_diffusion_solver );
  REGISTER( test_make_tri_tree );
  REGISTER( test_parse_arcs );
}

DEFINE_MAIN;
