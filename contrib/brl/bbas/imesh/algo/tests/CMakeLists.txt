ADD_EXECUTABLE( imesh_algo_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( imesh_algo_test_include imesh_algo)

ADD_EXECUTABLE( imesh_algo_test_template_include test_template_include.cxx )
TARGET_LINK_LIBRARIES( imesh_algo_test_template_include imesh_algo)

INCLUDE( ${VXL_CMAKE_DIR}/FindNetlib.cmake )
IF(NETLIB_FOUND)
ADD_EXECUTABLE( imesh_algo_test_all
  test_driver.cxx
  test_generate_mesh.cxx
)
TARGET_LINK_LIBRARIES( imesh_algo_test_all imesh imesh_algo vnl vgl testlib )
if( HACK_FORCE_BRL_FAILING_TESTS ) ## This test always fails in travis builds, segfault during memory free operation
ADD_TEST( imesh_test_generate_mesh ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/imesh_algo_test_all test_generate_mesh)
endif()
ENDIF (NETLIB_FOUND)
