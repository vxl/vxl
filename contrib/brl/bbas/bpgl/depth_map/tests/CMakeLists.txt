# bbas/bpgl/depth_map/tests/CMakeLists.txt

add_executable( depth_map_test_all
  test_driver.cxx
  test_depth_map.cxx
)

target_link_libraries( depth_map_test_all depth_map ${VXL_LIB_PREFIX}testlib ${VXL_LIB_PREFIX}vpl )

add_test( NAME depth_map_test_depth_map COMMAND $<TARGET_FILE:depth_map_test_all> test_depth_map)

add_executable(bpgl_depth_map_test_include test_include.cxx)
target_link_libraries(bpgl_depth_map_test_include depth_map)
