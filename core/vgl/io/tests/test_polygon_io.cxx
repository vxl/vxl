// This is core/vgl/io/tests/test_polygon_io.cxx
#include <vcl_iostream.h>
#include <vgl/vgl_polygon.h>
#include <vsl/vsl_indent.h>
#include <vgl/io/vgl_io_polygon.h>
#include <testlib/testlib_test.h>
#include <vpl/vpl.h>

static void test_polygon_io()
{
  vcl_cout << "************************\n"
           << " Testing vgl_polygon io\n"
           << "************************\n";

  //// test constructors, accessors
  vgl_polygon p_out(1), p_in;
  p_out.push_back(1.1f, 1.2f);
  p_out.push_back(2.1f, 2.2f);
  p_out.push_back(3.1f, 3.2f);
  p_out.new_sheet();
  p_out.push_back(1.3f, 1.4f);
  p_out.push_back(2.3f, 2.4f);
  p_out.push_back(3.3f, 3.4f);

  vsl_b_ofstream bfs_out("vgl_polygon_test_io.bvl.tmp");
  TEST ("Created vgl_polygon_test_io.bvl.tmp for writing",
        (!bfs_out), false);
  vsl_b_write(bfs_out, p_out);
  bfs_out.close();

  vsl_b_ifstream bfs_in("vgl_polygon_test_io.bvl.tmp");
  TEST ("Opened vgl_polygon_test_io.bvl.tmp for reading", (!bfs_in), false);
  vsl_b_read(bfs_in, p_in);
  TEST ("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  vpl_unlink ("vgl_polygon_test_io.bvl.tmp");

  TEST ("p_out == p_in",
        p_out.num_sheets()==p_in.num_sheets() &&
        p_out[0]==p_in[0] &&
        p_out[1]==p_in[1] , true);

  vsl_print_summary(vcl_cout, p_out);
  vcl_cout << vcl_endl;
  vsl_indent_clear_all_data();
}

TESTMAIN(test_polygon_io);
