// This is brl/bbas/bsol/tests/test_hough_index.cxx
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <bsol/bsol_hough_line_index.h>
#include <testlib/testlib_test.h>

#define popen _popen
#define pclose _pclose

void test_hough_index()
{
  // we want to test the methods bsol_hough_index
  std::cout << "Testing bsol_hough_line_index\n"
           << "Testing basic insertion and removal\n";
  bsol_hough_line_index hl(0.0, 0.0, 512.0, 512.0, 180.0, 5.0);
  vsol_point_2d_sptr p0 = new vsol_point_2d(0,0);
  vsol_point_2d_sptr p1 = new vsol_point_2d(1,1);
  vsol_point_2d_sptr p2 = new vsol_point_2d(1,2);
  vsol_point_2d_sptr p3 = new vsol_point_2d(2,3);
  vsol_point_2d_sptr p4 = new vsol_point_2d(3,3);
  vsol_line_2d_sptr l = new vsol_line_2d(p0, p1);
  vsol_line_2d_sptr l1 = new vsol_line_2d(p0, p2);
  vsol_line_2d_sptr l3 = new vsol_line_2d(p2, p3);
  vsol_line_2d_sptr l4 = new vsol_line_2d(p1, p4);
  bool indexed = hl.index(l);
  bool found_l = hl.find(l);
  bool found_l1 = hl.find(l1);
  std::cout << "indexed =" << indexed << '\n'
           << "found_l = " << found_l << "found_l1 " << found_l1 << '\n';
  TEST("...", indexed&&found_l&&!found_l1, true);
  bool remove_l = hl.remove(l);
  found_l = hl.find(l);
  std::cout << "remove_l=" << remove_l << '\n'
           << "found_l = " << found_l << '\n';
  TEST("...", remove_l&&!found_l, true);
  std::cout << "Testing collinearity\n";
  hl.index(l);
  std::vector<vsol_line_2d_sptr> collinear = hl.lines_in_interval(l4, 1, 2.0);
  std::cout << "" << collinear.size() << '\n'
           << "" << collinear[0]->p0()->x() << '\n';
  TEST("collinear lines size = 1", collinear.size(), 1);
  TEST("the collinear line x00...", collinear[0]->p0()->x(), 0);

  std::cout << "Testing parallel\n";
  hl.index(l3);
  std::vector<vsol_line_2d_sptr> parallel = hl.parallel_lines(45, 0.5);
  std::vector<vsol_line_2d_sptr> non_parallel = hl.parallel_lines(40, 0.5);
  std::cout << "parallel lines size " << parallel.size() << '\n'
           << "non_parallel lines size " << non_parallel.size() << '\n';
  TEST("...", parallel.size(), 2);
  TEST("...", non_parallel.size(), 0);
}

TESTLIB_DEFINE_MAIN(test_hough_index);
