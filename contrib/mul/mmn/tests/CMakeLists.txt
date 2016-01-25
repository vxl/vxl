# This is mul/mmn/tests/CMakeLists.txt

ADD_EXECUTABLE( mmn_test_all
  test_driver.cxx
  test_graph_rep1.cxx
  test_dp_solver.cxx

  test_lbp_solver.cxx
  test_order_cost.cxx
  test_csp_solver.cxx
  test_diffusion_solver.cxx
  test_make_tri_tree.cxx
  test_parse_arcs.cxx
)
TARGET_LINK_LIBRARIES( mmn_test_all mmn vpdfl pdf1d vgl vsl testlib )

ADD_TEST( NAME mmn_test_graph_rep1 COMMAND $<TARGET_FILE:mmn_test_all> test_graph_rep1 )
ADD_TEST( NAME mmn_test_dp_solver COMMAND $<TARGET_FILE:mmn_test_all> test_dp_solver  )
ADD_TEST( NAME mmn_test_lbp_solver COMMAND $<TARGET_FILE:mmn_test_all> test_lbp_solver )
ADD_TEST( NAME mmn_test_order_cost COMMAND $<TARGET_FILE:mmn_test_all> test_order_cost )
ADD_TEST( NAME mmn_test_csp_solver COMMAND $<TARGET_FILE:mmn_test_all> test_csp_solver )
ADD_TEST( NAME mmn_test_make_tri_tree COMMAND $<TARGET_FILE:mmn_test_all> test_make_tri_tree )
ADD_TEST( NAME mmn_test_diffusion_solver COMMAND $<TARGET_FILE:mmn_test_all> test_diffusion_solver )
ADD_TEST( NAME mmn_test_parse_arcs COMMAND $<TARGET_FILE:mmn_test_all> test_parse_arcs )

ADD_EXECUTABLE( mmn_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( mmn_test_include mmn )
