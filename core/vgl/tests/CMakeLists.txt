# ./vxl/vgl/tests/CMakeLists.txt

ADD_EXECUTABLE( vgl_test_all                      vgl_test_all.cxx )
ADD_EXECUTABLE( vgl_test_triangle_scan_iterator   test_triangle_scan_iterator.cxx )
ADD_EXECUTABLE( vgl_test_ellipse_scan_iterator    test_ellipse_scan_iterator.cxx )
ADD_EXECUTABLE( vgl_test_homg                     test_homg.cxx )
ADD_EXECUTABLE( vgl_test_cartesian                test_cartesian.cxx )
ADD_EXECUTABLE( vgl_test_conic                    test_conic.cxx )
ADD_EXECUTABLE( vgl_test_polygon                  test_polygon.cxx )
ADD_EXECUTABLE( vgl_test_distance                 test_distance.cxx )

ADD_TEST( vgl_test_all                            vgl_test_all )
ADD_TEST( vgl_test_triangle_scan_iterator         vgl_test_triangle_scan_iterator )
ADD_TEST( vgl_test_ellipse_scan_iterator          vgl_test_ellipse_scan_iterator )
ADD_TEST( vgl_test_homg                           vgl_test_homg )
ADD_TEST( vgl_test_cartesian                      vgl_test_cartesian )
ADD_TEST( vgl_test_conic                          vgl_test_conic )
ADD_TEST( vgl_test_polygon                        vgl_test_polygon )
ADD_TEST( vgl_test_distance                       vgl_test_distance )


# Immediate dependencies
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vgl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vgl/algo/CMakeListsLink.txt )
