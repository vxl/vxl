// This is core/vgl/io/tests/test_box_3d_io.cxx
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_box_3d.h>
#include <vsl/vsl_indent.h>
#include <vgl/io/vgl_io_box_3d.h>
#include <testlib/testlib_test.h>
#include <vpl/vpl.h>

static void test_box_3d_double_io()
{
  std::cout << "*******************************\n"
           << " Testing vgl_box_3d<double> io\n"
           << "*******************************\n";

  //// test constructors, accessors
  vgl_box_3d<double> p_out(1.2,3.4,5.6,2.3,4.5,6.7), p_in;

  vsl_b_ofstream bfs_out("vgl_box_3d_test_double_io.bvl.tmp");
  TEST ("Created vgl_box_3d_test_double_io.bvl.tmp for writing",
        (!bfs_out), false);
  vsl_b_write(bfs_out, p_out);
  bfs_out.close();

  vsl_b_ifstream bfs_in("vgl_box_3d_test_double_io.bvl.tmp");
  TEST ("Opened vgl_box_3d_test_double_io.bvl.tmp for reading",
        (!bfs_in), false);
  vsl_b_read(bfs_in, p_in);
  TEST ("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  vpl_unlink ("vgl_box_3d_test_double_io.bvl.tmp");

  TEST ("p_out==p_in",p_out, p_in);

  vsl_print_summary(std::cout, p_out);
  std::cout << std::endl;
  vsl_indent_clear_all_data();
}

static void test_box_3d_io()
{
  test_box_3d_double_io();
}

TESTMAIN(test_box_3d_io);
