#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_utility.h>

#include <vsl/vsl_test.h>
#include <vsl/vsl_complex_io.h>

void test_complex_io()
{
  vcl_cout << "****************************" << vcl_endl;
  vcl_cout << "Testing vcl_complex binary io" << vcl_endl;
  vcl_cout << "****************************" << vcl_endl;

  vcl_complex<float> c_float_out(1.23f, 4.56f);

  vsl_b_ofstream bfs_out("vsl_complex_io_test.bvl.tmp",
    vcl_ios_out | vcl_ios_binary);
  TEST ("Created vsl_complex_io_test.bvl.tmp for writing",
    (!bfs_out), false);
  vsl_b_write(bfs_out, c_float_out);
  bfs_out.close();
  
  vcl_complex<float> c_float_in;
  
  vsl_b_ifstream bfs_in("vsl_complex_io_test.bvl.tmp",
    vcl_ios_in | vcl_ios_binary);
  TEST ("Opened vsl_complex_io_test.bvl.tmp for reading",
    (!bfs_in), false);
  vsl_b_read(bfs_in, c_float_in);
  bfs_in.close();

  TEST ("vcl_complex<float> out == vcl_complex<float> in",
    c_float_out == c_float_in, true);

  vsl_print_summary(vcl_cout, c_float_in);
  vcl_cout << vcl_endl;
}

TESTMAIN(test_complex_io);
