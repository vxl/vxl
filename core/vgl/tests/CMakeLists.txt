# ./vxl/vgl/tests/CMakeLists.txt

ADD_EXECUTABLE( vgl_test_driver
  test_driver.cxx    test_driver.h

  test_includes.cxx
  test_cartesian.cxx
  test_distance.cxx
  test_conic.cxx
  test_homg.cxx
  test_polygon.cxx
  test_polygon_scan_iterator.cxx
  test_triangle_scan_iterator.cxx
  test_ellipse_scan_iterator.cxx
  test_area.cxx
  test_clip.cxx
)                 

ADD_TEST( vgl_test_includes                     vgl_test_driver test_includes )
ADD_TEST( vgl_test_cartesian                    vgl_test_driver test_cartesian )
ADD_TEST( vgl_test_distance                     vgl_test_driver test_distance )
ADD_TEST( vgl_test_conic                        vgl_test_driver test_conic )
ADD_TEST( vgl_test_homg                         vgl_test_driver test_homg )
ADD_TEST( vgl_test_polygon                      vgl_test_driver test_polygon )
ADD_TEST( vgl_test_polygon_scan_iterator        vgl_test_driver test_polygon_scan_iterator )
ADD_TEST( vgl_test_triangle_scan_iterator       vgl_test_driver test_triangle_scan_iterator )
ADD_TEST( vgl_test_ellipse_scan_iterator        vgl_test_driver test_ellipse_scan_iterator )
ADD_TEST( vgl_test_area                         vgl_test_driver test_area )
ADD_TEST( vgl_test_clip                         vgl_test_driver test_clip )

# Immediate dependencies
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vgl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vgl/algo/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/testlib/CMakeListsLink.txt )
