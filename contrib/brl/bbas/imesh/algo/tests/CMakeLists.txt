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
ADD_TEST( imesh_test_generate_mesh ${EXECUTABLE_OUTPUT_PATH}/imesh_algo_test_all test_generate_mesh)
ENDIF (NETLIB_FOUND)
