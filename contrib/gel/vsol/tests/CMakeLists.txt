# gel/vsol/tests/CMakeLists.txt

LINK_LIBRARIES( vsol vgl testlib )

ADD_EXECUTABLE( vsol_test_all
  test_driver.cxx
  test_vsol_point_2d.cxx
  test_vsol_group_2d.cxx
  test_vsol_line_2d.cxx
  test_vsol_conic_2d.cxx
  test_vsol_triangle_2d.cxx
  test_vsol_rectangle_2d.cxx
  test_vsol_polygon_2d.cxx
  test_vsol_point_3d.cxx
  test_vsol_group_3d.cxx
  test_vsol_line_3d.cxx
  test_vsol_triangle_3d.cxx
  test_vsol_rectangle_3d.cxx
  test_vsol_polygon_3d.cxx
  test_vsol_tetrahedron.cxx
)

ADD_TEST(vsol_test_point_2d     vsol_test_all test_vsol_point_2d)
ADD_TEST(vsol_test_group_2d     vsol_test_all test_vsol_group_2d)
ADD_TEST(vsol_test_line_2d      vsol_test_all test_vsol_line_2d)
ADD_TEST(vsol_test_conic_2d     vsol_test_all test_vsol_conic_2d)
ADD_TEST(vsol_test_triangle_2d  vsol_test_all test_vsol_triangle_2d)
ADD_TEST(vsol_test_rectangle_2d vsol_test_all test_vsol_rectangle_2d)
ADD_TEST(vsol_test_polygon_2d   vsol_test_all test_vsol_polygon_2d)
ADD_TEST(vsol_test_point_3d     vsol_test_all test_vsol_point_3d)
ADD_TEST(vsol_test_group_3d     vsol_test_all test_vsol_group_3d)
ADD_TEST(vsol_test_line_3d      vsol_test_all test_vsol_line_3d)
ADD_TEST(vsol_test_triangle_3d  vsol_test_all test_vsol_triangle_3d)
ADD_TEST(vsol_test_rectangle_3d vsol_test_all test_vsol_rectangle_3d)
ADD_TEST(vsol_test_polygon_3d   vsol_test_all test_vsol_polygon_3d)
ADD_TEST(vsol_test_tetrahedron  vsol_test_all test_vsol_tetrahedron)

ADD_EXECUTABLE( vsol_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vsol_test_include vsol )
