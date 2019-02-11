#include <vector>
#include <iostream>
#include <testlib/testlib_test.h>
#include <vcl_compiler.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_area.h>
#include <vgl/vgl_oriented_box_2d.h>
#include <vgl/algo/vgl_hough_index_2d.h>

static void test_hough_index_2d()
{
  vgl_hough_index_2d<double> hl(0.0, 0.0, 512.0, 512.0, 180.0, 5.0);
  vgl_point_2d<double> p0(0,0);
  vgl_point_2d<double> p1(1,1);
  vgl_point_2d<double> p2(1,2);
  vgl_point_2d<double> p3(2,3);
  vgl_point_2d<double> p4(3,3);
  vgl_line_segment_2d<double> l(p0, p1);
  vgl_line_segment_2d<double> l1(p0, p2);
  vgl_line_segment_2d<double> l3(p2, p3);
  vgl_line_segment_2d<double> l4(p1, p4);
  bool inserted = hl.insert(l);
  bool found_l = hl.find(l);
  bool found_l1 = hl.find(l1);
  std::cout << "indexed =" << inserted << '\n'
           << "found_l = " << found_l << "found_l1 " << found_l1 << '\n';
  TEST("...", inserted&&found_l&&!found_l1, true);
  bool remove_l = hl.remove(l);
  found_l = hl.find(l);
  std::cout << "remove_l=" << remove_l << '\n'
           << "found_l = " << found_l << '\n';
  TEST("...", remove_l&&!found_l, true);
  std::cout << "Testing collinearity\n";
  hl.insert(l);
  std::vector<vgl_line_segment_2d<double> > collinear = hl.lines_in_interval(l4, 1, 2.0);
  std::cout << "" << collinear.size() << '\n'
           << "" << collinear[0].point1().x() << '\n';
  TEST("collinear lines size = 1", collinear.size(), 1);
  TEST("the collinear line x00...", collinear[0].point1().x(), 0);

  std::cout << "Testing parallel\n";
  hl.insert(l3);
  std::vector<vgl_line_segment_2d<double> > parallel = hl.parallel_lines(45, 0.5);
  std::vector<vgl_line_segment_2d<double> > non_parallel = hl.parallel_lines(40, 0.5);
  std::cout << "parallel lines size " << parallel.size() << '\n'
           << "non_parallel lines size " << non_parallel.size() << '\n';
  TEST("...", parallel.size(), 2);
  TEST("...", non_parallel.size(), 0);

}

TESTMAIN(test_hough_index_2d);
