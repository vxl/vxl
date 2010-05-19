
SET(boxm_ocl_test_sources 
  test_driver.cxx
  octree_test_driver.h octree_test_driver.txx
  open_cl_test_data.h  open_cl_test_data.cxx
  test_octree.cxx
  test_backproject_ray.cxx
  test_expected_image.cxx
)


AUX_SOURCE_DIRECTORY(Templates boxm_ocl_test_sources)
ADD_EXECUTABLE(boxm_ocl_test_all ${boxm_ocl_test_sources})
TARGET_LINK_LIBRARIES( boxm_ocl_test_all boxm_ocl boxm boxm_util boxm_algo_rt boxm_sample boxm_sample_algo testlib vnl vgl_xio vil vpl vul vpgl)

ADD_TEST( test_octree          ${EXECUTABLE_OUTPUT_PATH}/boxm_ocl_test_all test_octree         )
ADD_TEST( test_backproject_ray ${EXECUTABLE_OUTPUT_PATH}/boxm_ocl_test_all test_backproject_ray)
ADD_TEST( test_expected_image  ${EXECUTABLE_OUTPUT_PATH}/boxm_ocl_test_all test_expected_image )


ADD_EXECUTABLE( boxm_ocl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( boxm_ocl_test_include boxm_ocl boxm)
