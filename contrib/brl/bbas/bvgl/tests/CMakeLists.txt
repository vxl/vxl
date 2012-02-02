# brl/bbas/bvgl/tests/CMakeLists.txt

ADD_EXECUTABLE(bvgl_test_all
  test_driver.cxx
  test_bvgl_changes.cxx
  test_bvgl_volume_of_intersection.cxx
  test_bvgl_ray_pyramid.cxx
  test_bvgl_intersection.cxx
)
TARGET_LINK_LIBRARIES(bvgl_test_all bvgl vgl vnl testlib)

ADD_TEST(bvgl_test_changes                ${EXECUTABLE_OUTPUT_PATH}/bvgl_test_all test_bvgl_changes)
ADD_TEST(bvgl_test_volume_of_intersection ${EXECUTABLE_OUTPUT_PATH}/bvgl_test_all test_bvgl_volume_of_intersection)
ADD_TEST(bvgl_test_ray_pyramid            ${EXECUTABLE_OUTPUT_PATH}/bvgl_test_all test_bvgl_ray_pyramid)
ADD_TEST(bvgl_test_intersection           ${EXECUTABLE_OUTPUT_PATH}/bvgl_test_all test_bvgl_intersection)

ADD_EXECUTABLE( bvgl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bvgl_test_include bvgl )
ADD_EXECUTABLE( bvgl_test_template_include test_template_include.cxx )
TARGET_LINK_LIBRARIES( bvgl_test_template_include bvgl )
