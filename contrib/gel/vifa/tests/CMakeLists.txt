# gel/vifa/tests/CMakeLists.txt

INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bseg)

LINK_LIBRARIES( vbl vil vil1 vgl vnl vul testlib vifa gevd vdgl vsol vtol sdet )

ADD_EXECUTABLE( vifa_test_all
  test_ifa_factory.h
  test_ifa_factory.cxx
  test_driver.cxx
  test_coll_lines_params.cxx
  test_group_pgram_params.cxx
  test_norm_params.cxx
  test_parallel_params.cxx
  test_int_face_attr_common_params.cxx
  test_image_histogram.cxx
  test_region_proc.cxx
  test_int_face_attr.cxx
  test_int_faces_attr.cxx
  test_int_faces_adj_attr.cxx
)

ADD_TEST(vifa_test_coll_lines_params  vifa_test_all test_coll_lines_params)
ADD_TEST(vifa_test_group_pgram_params vifa_test_all test_group_pgram_params)
ADD_TEST(vifa_test_norm_params        vifa_test_all test_norm_params)
ADD_TEST(vifa_test_parallel_params    vifa_test_all test_parallel_params)
ADD_TEST(vifa_test_int_face_attr_common_params vifa_test_all test_int_face_attr_common_params)
ADD_TEST(vifa_test_image_histogram    vifa_test_all test_image_histogram ${CMAKE_CURRENT_SOURCE_DIR}/cam0013.tif)
ADD_TEST(vifa_test_region_proc        vifa_test_all test_region_proc)
ADD_TEST(vifa_test_int_face_attr      vifa_test_all test_int_face_attr)
ADD_TEST(vifa_test_int_faces_attr     vifa_test_all test_int_faces_attr)
ADD_TEST(vifa_test_int_faces_adj_attr vifa_test_all test_int_faces_adj_attr)

ADD_EXECUTABLE( vifa_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vifa_test_include vifa )
