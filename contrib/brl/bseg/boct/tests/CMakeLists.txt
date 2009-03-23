ADD_EXECUTABLE( boct_test_all
  test_driver.cxx
  test_create_tree.cxx
  test_locate_point.cxx
 )

TARGET_LINK_LIBRARIES( boct_test_all boct testlib)

#ADD_TEST( bvxm_test_voxel_storage_mem         ${EXECUTABLE_OUTPUT_PATH}/bvxm_test_all   test_voxel_storage_mem )

ADD_EXECUTABLE( boct_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( boct_test_include boct)
