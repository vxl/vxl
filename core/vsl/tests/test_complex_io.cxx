// This is vxl/vsl/tests/test_complex_io.cxx
#include <vcl_iostream.h>
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_complex_io.h>
#include <testlib/testlib_test.h>

void test_complex_io()
{
  vcl_cout << "*****************************\n"
           << "Testing vcl_complex binary io\n"
           << "*****************************\n";

  vcl_complex<float> c_float_out(1.23f, 4.56f);

  vsl_b_ofstream bfs_out("vsl_complex_io_test.bvl.tmp");
  TEST ("Created vsl_complex_io_test.bvl.tmp for writing",
    (!bfs_out), false);
  vsl_b_write(bfs_out, c_float_out);
  bfs_out.close();

  vcl_complex<float> c_float_in;

  vsl_b_ifstream bfs_in("vsl_complex_io_test.bvl.tmp");
  TEST ("Opened vsl_complex_io_test.bvl.tmp for reading",
    (!bfs_in), false);
  vsl_b_read(bfs_in, c_float_in);
  TEST ("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  TEST ("vcl_complex<float> out == vcl_complex<float> in",
    c_float_out == c_float_in, true);

  vsl_print_summary(vcl_cout, c_float_in);
  vcl_cout << vcl_endl;
}

TESTLIB_DEFINE_MAIN(test_complex_io);
