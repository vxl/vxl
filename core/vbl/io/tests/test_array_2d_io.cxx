// This is vxl/vbl/io/tests/test_array_2d_io.cxx
#include <vcl_iostream.h>
#include <vbl/io/vbl_io_array_2d.h>
#include <vsl/vsl_binary_io.h>
#include <testlib/testlib_test.h>
#include <vpl/vpl.h>

void test_array_2d_io()
{
  vcl_cout << "******************************\n"
           << "Testing vbl_array_2d<float> io\n"
           << "******************************\n";

  //// test constructors, accessors
  const int array_rows = 8;
  const int array_cols = 6;
  vbl_array_2d<int> v_out(array_rows, array_cols), v_in;

  for (int i=0; i<array_rows; i++)
  {
    for (int j=0; j< array_cols; j++)
      v_out(i,j) = i*j*j;
  }

  vsl_b_ofstream bfs_out("vbl_array_2d_test_io.bvl.tmp");
  TEST("Created vbl_array_2d_test_io.bvl.tmp for writing", (!bfs_out), false);
  vsl_b_write(bfs_out, v_out);
  bfs_out.close();

  vsl_b_ifstream bfs_in("vbl_array_2d_test_io.bvl.tmp");
  TEST("Opened vbl_array_2d_test_io.bvl.tmp for reading", (!bfs_in), false);
  vsl_b_read(bfs_in, v_in);
  TEST("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  vpl_unlink ("vbl_array_2d_test_io.bvl.tmp");

  //kym - double = not defined for vbl_array_2d
  //TEST("v_out == v_in", v_out, v_in);

  bool test_result = true;
  if (v_out.rows() != v_in.rows())
    test_result = false;
  else if (v_out.cols() != v_in.cols())
    test_result = false;
  else
  {
    unsigned array_rows = v_out.rows();
    unsigned array_cols = v_out.cols();
    for (unsigned i=0; i<array_rows; i++)
    {
      for (unsigned j=0; j<array_cols; j++)
        if (v_out(i,j) != v_in(i,j))
          test_result = false;
    }
  }
  TEST("v_out == v_in", test_result, true);

  vsl_print_summary(vcl_cout, v_in);
  vcl_cout << vcl_endl;
}

TESTMAIN(test_array_2d_io);
