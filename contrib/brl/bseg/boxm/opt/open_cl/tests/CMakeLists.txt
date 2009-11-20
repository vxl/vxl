ADD_EXECUTABLE( boxm_opt_opencl_test_all
  test_driver.cxx
  octree_test_driver.h
  octree_test_driver.cxx
  open_cl_test_data.h open_cl_test_data.cxx
  test_octree.cxx
  test_ray_trace.cxx
)

TARGET_LINK_LIBRARIES( boxm_opt_opencl_test_all boxm_opt_opencl boxm_opt boxm boct testlib vnl vgl_xio vil vpl vul vpgl)

ADD_TEST( test_octree     ${EXECUTABLE_OUTPUT_PATH}/boxm_opt_opencl_test_all test_octree  )
ADD_TEST( test_ray_trace     ${EXECUTABLE_OUTPUT_PATH}/boxm_opt_opencl_test_all test_ray_trace  )

ADD_EXECUTABLE( open_cl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( open_cl_test_include open_cl )
