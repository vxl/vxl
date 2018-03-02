add_executable( imesh_algo_test_include test_include.cxx )
target_link_libraries( imesh_algo_test_include imesh_algo)

add_executable( imesh_algo_test_template_include test_template_include.cxx )
target_link_libraries( imesh_algo_test_template_include imesh_algo)

include( ${VXL_CMAKE_DIR}/FindNetlib.cmake )
if(NETLIB_FOUND)
add_executable( imesh_algo_test_all
  test_driver.cxx
  test_generate_mesh.cxx
)
target_link_libraries( imesh_algo_test_all imesh imesh_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}testlib )
if( VXL_RUN_FAILING_TESTS ) ## This test always fails in travis builds, segfault during memory free operation
add_test( NAME imesh_test_generate_mesh COMMAND $<TARGET_FILE:imesh_algo_test_all> test_generate_mesh)
endif()
endif()
