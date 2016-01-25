# brl/bbas/imesh/tests/CMakeLists.txt
# Tests for imesh library

# The following include path is needed for imesh/algo:
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )

ADD_EXECUTABLE( imesh_test_all
  test_driver.cxx
  test_share.cxx          test_share.h
  test_intersect.cxx
  test_detect.cxx
  test_kd_tree.cxx
  test_imls_surface.cxx
)

TARGET_LINK_LIBRARIES( imesh_test_all imesh imesh_algo vnl vgl testlib )

ADD_TEST( NAME imesh_test_intersect COMMAND $<TARGET_FILE:imesh_test_all> test_intersect )
ADD_TEST( NAME imesh_test_detect COMMAND $<TARGET_FILE:imesh_test_all> test_detect )
ADD_TEST( NAME imesh_test_kd_tree COMMAND $<TARGET_FILE:imesh_test_all> test_kd_tree )
ADD_TEST( NAME imesh_test_imls_surface COMMAND $<TARGET_FILE:imesh_test_all> test_imls_surface )

ADD_EXECUTABLE( imesh_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( imesh_test_include imesh )
