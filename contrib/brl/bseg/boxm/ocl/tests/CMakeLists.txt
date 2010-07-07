
SET(boxm_ocl_test_sources
    test_driver.cxx
    octree_test_driver.h octree_test_driver.txx
    open_cl_test_data.h  open_cl_test_data.cxx
    test_octree.cxx
    test_octree_stack.cxx
    test_octree_image.cxx
    test_backproject_ray.cxx
    test_expected_image.cxx
    ray_bundle_test_driver.h ray_bundle_test_driver.txx
    test_ray_bundle.cxx
    stat_test_driver.h stat_test_driver.txx
    test_statistics.cxx
    test_refine.cxx
    test_update.cxx
    test_file_io.cxx
    online_update_test_manager.h     online_update_test_manager.txx
    test_scene_io.cxx
   )

AUX_SOURCE_DIRECTORY(Templates boxm_ocl_test_sources)

ADD_EXECUTABLE(boxm_ocl_test_all ${boxm_ocl_test_sources})
TARGET_LINK_LIBRARIES( boxm_ocl_test_all boxm_ocl boxm boxm_util boxm_algo_rt boxm_sample boxm_sample_algo testlib vpgl vgl_xio vnl vil vgl vbl vpl vul)

ADD_TEST( test_octree          ${EXECUTABLE_OUTPUT_PATH}/boxm_ocl_test_all test_octree         )
ADD_TEST( test_octree_stack         ${EXECUTABLE_OUTPUT_PATH}/boxm_ocl_test_all test_octree_stack         )
ADD_TEST( test_octree_image         ${EXECUTABLE_OUTPUT_PATH}/boxm_ocl_test_all test_octree_image         )

ADD_TEST( test_backproject_ray ${EXECUTABLE_OUTPUT_PATH}/boxm_ocl_test_all test_backproject_ray)
ADD_TEST( test_expected_image  ${EXECUTABLE_OUTPUT_PATH}/boxm_ocl_test_all test_expected_image )
ADD_TEST( test_ray_bundle  ${EXECUTABLE_OUTPUT_PATH}/boxm_ocl_test_all test_ray_bundle )
ADD_TEST( test_statistics  ${EXECUTABLE_OUTPUT_PATH}/boxm_ocl_test_all test_statistics )
ADD_TEST( test_refine ${EXECUTABLE_OUTPUT_PATH}/boxm_ocl_test_all test_refine )
ADD_TEST( test_update ${EXECUTABLE_OUTPUT_PATH}/boxm_ocl_test_all test_update )
ADD_TEST( test_file_io ${EXECUTABLE_OUTPUT_PATH}/boxm_ocl_test_all test_file_io )
ADD_TEST( test_scene_io ${EXECUTABLE_OUTPUT_PATH}/boxm_ocl_test_all test_scene_io )

ADD_EXECUTABLE( boxm_ocl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( boxm_ocl_test_include boxm_ocl boxm)
