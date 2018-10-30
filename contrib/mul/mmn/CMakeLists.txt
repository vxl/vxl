# mul/mmn/CMakeLists.txt

doxygen_add_library(contrib/mul/mmn
  DEPENDS contrib/mul/mbl
  PACKAGE contrib/mul
  DESCRIPTION "Manchester Markov Network library"
  )

set(mmn_sources
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
  mmn_parse_arcs.h      mmn_parse_arcs.cxx
)
aux_source_directory(Templates mmn_sources)

vxl_add_library(LIBRARY_NAME mmn LIBRARY_SOURCES ${mmn_sources})
target_link_libraries(mmn mbl)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

if(VXL_BUILD_MUL_TOOLS)
#  add_subdirectory(tools)
endif()
