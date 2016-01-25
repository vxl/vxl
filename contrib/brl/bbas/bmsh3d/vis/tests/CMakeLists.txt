# brl/bbas/bmsh3d/vis/tests/CMakeLists.txt

include_directories( ${BRL_INCLUDE_DIR} )

add_executable( bmsh3d_vis_test_include test_include.cxx )
target_link_libraries( bmsh3d_vis_test_include bmsh3d_vis )
