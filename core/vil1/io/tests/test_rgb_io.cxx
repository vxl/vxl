// This is vxl/vil/io/tests/test_rgb_io.cxx
#include <vcl_iostream.h>
#include <vil/io/vil_io_rgb.h>
#include <testlib/testlib_test.h>
#include <vpl/vpl.h>

void test_rgb_io()
{
  vcl_cout << "**************************\n"
           << "Testing vil_rgb<double> io\n"
           << "**************************\n";

  //// test constructors, accessors
  vil_rgb<double> p_out(1.2,3.4,5.6), p_in;


  vsl_b_ofstream bfs_out("vil_rgb_test_double_io.bvl.tmp");
  TEST ("Created vil_rgb_test_double_io.bvl.tmp for writing", 
        (!bfs_out), false);
  vsl_b_write(bfs_out, p_out);
  bfs_out.close();

  vsl_b_ifstream bfs_in("vil_rgb_test_double_io.bvl.tmp");
  TEST ("Opened vil_rgb_test_double_io.bvl.tmp for reading", 
        (!bfs_in), false);
  vsl_b_read(bfs_in, p_in);
  TEST ("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  vpl_unlink ("vil_rgb_test_double_io.bvl.tmp");

  TEST ("p_out == p_in", 
        p_out.R()==p_in.R() && 
        p_out.G()==p_in.G() && 
        p_out.B()==p_in.B() , true);

  vsl_print_summary(vcl_cout, p_out);
  vcl_cout << vcl_endl;
}


TESTMAIN( test_rgb_io );
