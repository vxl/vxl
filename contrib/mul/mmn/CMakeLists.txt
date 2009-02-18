# mul/mmn/CMakeLists.txt

SET(mmn_sources
  mmn_arc.h
  mmn_triplet.h
  mmn_analyze_graph.h
  mmn_dependancy.h
  mmn_graph_rep1.h           mmn_graph_rep1.cxx

  mmn_solver.h               mmn_solver.cxx
  mmn_dp_solver.h            mmn_dp_solver.cxx
  mmn_lbp_solver.h           mmn_lbp_solver.cxx
  mmn_make_tri_tree.h        mmn_make_tri_tree.cxx
  mmn_solve.h
  mmn_order_cost.h           mmn_order_cost.cxx
  mmn_csp_solver.h           mmn_csp_solver.cxx
  mmn_diffusion_solver.h     mmn_diffusion_solver.cxx

  mmn_add_all_loaders.h      mmn_add_all_loaders.cxx
)
AUX_SOURCE_DIRECTORY(Templates mmn_sources)

ADD_LIBRARY(mmn ${mmn_sources})
TARGET_LINK_LIBRARIES(mmn mbl vnl vil vsl)

IF (BUILD_TESTING)
  SUBDIRS(tests)
ENDIF (BUILD_TESTING)

IF(BUILD_MUL_TOOLS)
#  SUBDIRS(tools)
ENDIF(BUILD_MUL_TOOLS)
