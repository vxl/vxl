# bpgl/acal/CMakeLists.txt
# affine camera calibration library
project("acal")

if (WIN32)
  add_definitions(
      -D_USE_MATH_DEFINES -DNOMINMAX
      -D_CRT_SECURE_NO_WARNINGS
  )
endif ()

include_directories( ${BRL_INCLUDE_DIR}/bbas )
include_directories( ${BRL_INCLUDE_DIR}/bbas/bpgl )

set(ACAL_SOURCES
    acal_f_utils.h                  acal_f_utils.cxx
    acal_intertile_graph_solver.h   acal_intertile_graph_solver.cxx
    acal_match_utils.h              acal_match_utils.cxx
    acal_match_tree.h               acal_match_tree.cxx
    acal_match_graph.h              acal_match_graph.cxx
    acal_match_tree_solver.h        acal_match_tree_solver.cxx
    acal_metadata.h
    acal_planar_feature_matcher.h   acal_planar_feature_matcher.cxx
    acal_solution_error.h
    acal_tile_data_manager.h        acal_tile_data_manager.cxx
)

vxl_add_library(LIBRARY_NAME acal LIBRARY_SOURCES ${ACAL_SOURCES})

target_link_libraries(acal brip vpgl vpgl_algo bjson bvgl vbl vgl vil_algo vil vcl)

add_subdirectory(io)

if( BUILD_TESTING)
  add_subdirectory(tests)
endif()
