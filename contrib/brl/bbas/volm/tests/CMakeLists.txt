# bbas/volm/tests/CMakeLists.txt

add_executable( volm_test_all
  test_driver.cxx
  test_spherical_container.cxx
  test_tile.cxx
  test_spherical_shell_container.cxx
  test_loc_hyp.cxx
  test_query.cxx
  test_index.cxx
  test_camera_space.cxx
  test_io.cxx
  test_region_query.cxx
  test_region_index.cxx
  test_spherical_region.cxx
  test_geo_index2.cxx
  test_osm_parser.cxx
  test_category_io.cxx
  test_osm_object.cxx
  test_candidate_region_parser.cxx
  test_utils.cxx
  test_find_overlapping.cxx
)

target_link_libraries( volm_test_all volm brad ${VXL_LIB_PREFIX}testlib ${VXL_LIB_PREFIX}vpl )

add_test( NAME volm_test_spherical_container COMMAND $<TARGET_FILE:volm_test_all> test_spherical_container)
add_test( NAME volm_test_tile COMMAND $<TARGET_FILE:volm_test_all> test_tile)
add_test( NAME volm_test_spherical_shell_container COMMAND $<TARGET_FILE:volm_test_all> test_spherical_shell_container)
add_test( NAME volm_test_loc COMMAND $<TARGET_FILE:volm_test_all> test_loc_hyp)
add_test( NAME volm_test_query COMMAND $<TARGET_FILE:volm_test_all> test_query)
add_test( NAME volm_test_index COMMAND $<TARGET_FILE:volm_test_all> test_index)
add_test( NAME volm_test_camera_space COMMAND $<TARGET_FILE:volm_test_all> test_camera_space)
add_test( NAME volm_test_io COMMAND $<TARGET_FILE:volm_test_all> test_io)
add_test( NAME volm_test_region_query COMMAND $<TARGET_FILE:volm_test_all> test_region_query)
add_test( NAME volm_test_region_index COMMAND $<TARGET_FILE:volm_test_all> test_region_index)
add_test( NAME volm_test_spherical_region COMMAND $<TARGET_FILE:volm_test_all> test_spherical_region)
add_test( NAME volm_test_volm_geo_index2 COMMAND $<TARGET_FILE:volm_test_all> test_geo_index2)
add_test( NAME volm_test_category_io COMMAND $<TARGET_FILE:volm_test_all> test_category_io)
add_test( NAME volm_test_osm_parser COMMAND $<TARGET_FILE:volm_test_all> test_osm_parser)
add_test( NAME volm_test_candidate_region_parser COMMAND $<TARGET_FILE:volm_test_all> test_candidate_region_parser)
add_test( NAME volm_test_osm_object COMMAND $<TARGET_FILE:volm_test_all> test_osm_object)
add_test( NAME volm_test_utils COMMAND $<TARGET_FILE:volm_test_all> test_utils)
add_test( NAME volm_test_overlapping_resources COMMAND $<TARGET_FILE:volm_test_all> test_overlapping_resources)
add_test( NAME volm_test_intersecting_resources COMMAND $<TARGET_FILE:volm_test_all> test_intersecting_resources)
add_test( NAME volm_test_compute_intersection COMMAND $<TARGET_FILE:volm_test_all> test_compute_intersection)

add_executable(volm_test_include test_include.cxx)
target_link_libraries(volm_test_include volm)

