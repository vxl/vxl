// This is vxl/vil/io/tests/test_rgba_io.cxx
#include <vcl_iostream.h>
#include <vil/io/vil_io_rgba.h>
#include <testlib/testlib_test.h>
#include <vpl/vpl.h>

void test_rgba_io()
{
  vcl_cout << "***************************\n"
           << "Testing vil_rgba<double> io\n"
           << "***************************\n";

  //// test constructors, accessors
  vil_rgba<double> p_out(1.2,3.4,5.6,7.8), p_in;

  vsl_b_ofstream bfs_out("vil_rgba_test_double_io.bvl.tmp");
  TEST ("Created vil_rgba_test_double_io.bvl.tmp for writing", 
        (!bfs_out), false);
  vsl_b_write(bfs_out, p_out);
  bfs_out.close();

  vsl_b_ifstream bfs_in("vil_rgba_test_double_io.bvl.tmp");
  TEST ("Opened vil_rgba_test_double_io.bvl.tmp for reading", 
        (!bfs_in), false);
  vsl_b_read(bfs_in, p_in);
  TEST ("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  vpl_unlink ("vil_rgba_test_double_io.bvl.tmp");

  TEST ("p_out == p_in", 
        p_out.R()==p_in.R() && 
        p_out.G()==p_in.G() && 
        p_out.B()==p_in.B() && 
        p_out.A()==p_in.A() , true);

  vsl_print_summary(vcl_cout, p_out);
  vcl_cout << vcl_endl;
}


TESTMAIN( test_rgba_io );
