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
  test_index_query_matcher.cxx
  test_spherical_region.cxx
  test_geo_index2.cxx
  test_osm_parser.cxx
  test_category_io.cxx
  test_osm_object.cxx
)

TARGET_LINK_LIBRARIES( volm_test_all volm testlib vpl )

ADD_TEST(volm_test_spherical_container ${EXECUTABLE_OUTPUT_PATH}/volm_test_all test_spherical_container)
ADD_TEST(volm_test_tile ${EXECUTABLE_OUTPUT_PATH}/volm_test_all test_tile)
ADD_TEST(volm_test_spherical_shell_container ${EXECUTABLE_OUTPUT_PATH}/volm_test_all test_spherical_shell_container)
ADD_TEST(volm_test_loc ${EXECUTABLE_OUTPUT_PATH}/volm_test_all test_loc_hyp)
ADD_TEST(volm_test_query ${EXECUTABLE_OUTPUT_PATH}/volm_test_all test_query)
ADD_TEST(volm_test_index ${EXECUTABLE_OUTPUT_PATH}/volm_test_all test_index)
ADD_TEST(volm_test_camera_space ${EXECUTABLE_OUTPUT_PATH}/volm_test_all test_camera_space)
ADD_TEST(volm_test_io ${EXECUTABLE_OUTPUT_PATH}/volm_test_all test_io)
ADD_TEST(volm_test_region_query ${EXECUTABLE_OUTPUT_PATH}/volm_test_all test_region_query)
ADD_TEST(volm_test_region_index ${EXECUTABLE_OUTPUT_PATH}/volm_test_all test_region_index)
ADD_TEST(volm_test_index_query_matcher ${EXECUTABLE_OUTPUT_PATH}/volm_test_all test_index_query_matcher)
ADD_TEST(volm_test_spherical_region ${EXECUTABLE_OUTPUT_PATH}/volm_test_all test_spherical_region)
ADD_TEST(volm_test_volm_geo_index2 ${EXECUTABLE_OUTPUT_PATH}/volm_test_all test_geo_index2)
ADD_TEST(volm_test_osm_parser ${EXECUTABLE_OUTPUT_PATH}/volm_test_all test_osm_parser)
ADD_TEST(volm_test_category_io ${EXECUTABLE_OUTPUT_PATH}/volm_test_all test_category_io)
ADD_TEST(volm_test_osm_object ${EXECUTABLE_OUTPUT_PATH}/volm_test_all test_osm_object)

ADD_EXECUTABLE(volm_test_include test_include.cxx)
TARGET_LINK_LIBRARIES(volm_test_include volm)

