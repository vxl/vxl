# brl/bbas/bmsh3d/vis/tests/CMakeLists.txt

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )

ADD_EXECUTABLE( bmsh3d_vis_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bmsh3d_vis_test_include bmsh3d_vis )
