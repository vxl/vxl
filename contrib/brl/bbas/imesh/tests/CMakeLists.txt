# brl/bbas/imesh/tests/CMakeLists.txt
# Tests for imesh library

# The following include path is needed for imesh/algo:
include_directories( ${GEL_INCLUDE_DIR}/mrc )

add_executable( imesh_test_all
  test_driver.cxx
  test_share.cxx          test_share.h
  test_intersect.cxx
  test_detect.cxx
  test_kd_tree.cxx
  test_imls_surface.cxx
)

target_link_libraries( imesh_test_all imesh imesh_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}testlib )

add_test( NAME imesh_test_intersect COMMAND $<TARGET_FILE:imesh_test_all> test_intersect )
add_test( NAME imesh_test_detect COMMAND $<TARGET_FILE:imesh_test_all> test_detect )
add_test( NAME imesh_test_kd_tree COMMAND $<TARGET_FILE:imesh_test_all> test_kd_tree )
add_test( NAME imesh_test_imls_surface COMMAND $<TARGET_FILE:imesh_test_all> test_imls_surface )

add_executable( imesh_test_include test_include.cxx )
target_link_libraries( imesh_test_include imesh )
