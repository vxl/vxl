# bbas/bpgl/depth_map/tests/CMakeLists.txt

ADD_EXECUTABLE( depth_map_test_all
  test_driver.cxx
  test_depth_map.cxx
)

TARGET_LINK_LIBRARIES( depth_map_test_all depth_map testlib vpl )

ADD_TEST( NAME depth_map_test_depth_map COMMAND $<TARGET_FILE:depth_map_test_all> test_depth_map)

ADD_EXECUTABLE(bpgl_depth_map_test_include test_include.cxx)
TARGET_LINK_LIBRARIES(bpgl_depth_map_test_include depth_map)
