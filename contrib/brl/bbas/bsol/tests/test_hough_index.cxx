// This is brl/bbas/bsol/tests/test_hough_index.cxx
#include <vcl_iostream.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <bsol/bsol_hough_line_index.h>
#define popen _popen
#define pclose _pclose
#define Assert(x) { vcl_cout << #x "\t\t\t test "; \
  if (x) { ++success; vcl_cout << "PASSED\n"; } else { ++failures; vcl_cout << "FAILED\n"; } }

int main(int, char **)
{
  int success=0, failures=0;

  // we want to test the methods bsol_hough_index
  vcl_cout << "Testing bsol_hough_line_index\n";
  vcl_cout << "Testing basic insertion and removal\n";
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
  vcl_cout << "indexed =" << indexed << "\n";
  vcl_cout << "found_l = " << found_l << "found_l1 " << found_l1 << "\n";
  Assert(indexed&&found_l&&!found_l1);
  bool remove_l = hl.remove(l);
  found_l = hl.find(l);
  vcl_cout << "remove_l=" << remove_l << "\n";
  vcl_cout << "found_l = " << found_l << "\n";
  Assert(remove_l&&!found_l);
  vcl_cout << "Testing collinearity\n";
  hl.index(l);
  vcl_vector<vsol_line_2d_sptr> colinear = hl.lines_in_interval(l4, 1, 2.0);
  vcl_cout << "colinear lines size " << colinear.size() << "\n";
  vcl_cout << "the colinear line x00" << colinear[0]->p0()->x() << "\n";
  Assert(colinear.size()==1&&colinear[0]->p0()->x()==0);

  vcl_cout << "Testing parallel\n";
  hl.index(l3);
  vcl_vector<vsol_line_2d_sptr> parallel = hl.parallel_lines(45, 0.5);
  vcl_vector<vsol_line_2d_sptr> non_parallel = hl.parallel_lines(40, 0.5);
  vcl_cout << "parallel lines size " << parallel.size() << "\n";
  vcl_cout << "non_parallel lines size " << non_parallel.size() << "\n";
  Assert(parallel.size()==2&&non_parallel.size()==0)
  vcl_cout << "finished testing bsol_hough_line_index\n";
  vcl_cout << "Test Summary: " << success << " tests succeeded, "
           << failures << " tests failed" << (failures?"\t***\n":"\n");
    return failures;
}
