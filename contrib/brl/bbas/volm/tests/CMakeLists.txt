# bbas/volm/tests/CMakeLists.txt

ADD_EXECUTABLE( volm_test_all
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

TARGET_LINK_LIBRARIES( volm_test_all volm brad testlib vpl )

ADD_TEST(volm_test_spherical_container ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/volm_test_all test_spherical_container)
ADD_TEST(volm_test_tile ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/volm_test_all test_tile)
ADD_TEST(volm_test_spherical_shell_container ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/volm_test_all test_spherical_shell_container)
ADD_TEST(volm_test_loc ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/volm_test_all test_loc_hyp)
ADD_TEST(volm_test_query ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/volm_test_all test_query)
ADD_TEST(volm_test_index ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/volm_test_all test_index)
ADD_TEST(volm_test_camera_space ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/volm_test_all test_camera_space)
ADD_TEST(volm_test_io ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/volm_test_all test_io)
ADD_TEST(volm_test_region_query ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/volm_test_all test_region_query)
ADD_TEST(volm_test_region_index ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/volm_test_all test_region_index)
ADD_TEST(volm_test_spherical_region ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/volm_test_all test_spherical_region)
ADD_TEST(volm_test_volm_geo_index2 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/volm_test_all test_geo_index2)
ADD_TEST(volm_test_category_io ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/volm_test_all test_category_io)
ADD_TEST(volm_test_osm_parser ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/volm_test_all test_osm_parser)
ADD_TEST(volm_test_candidate_region_parser ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/volm_test_all test_candidate_region_parser)
ADD_TEST(volm_test_osm_object ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/volm_test_all test_osm_object)
ADD_TEST(volm_test_utils ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/volm_test_all test_utils)
ADD_TEST(volm_test_overlapping_resources ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/volm_test_all test_overlapping_resources)
ADD_TEST(volm_test_intersecting_resources ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/volm_test_all test_intersecting_resources)
ADD_TEST(volm_test_compute_intersection ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/volm_test_all test_compute_intersection)

ADD_EXECUTABLE(volm_test_include test_include.cxx)
TARGET_LINK_LIBRARIES(volm_test_include volm)

