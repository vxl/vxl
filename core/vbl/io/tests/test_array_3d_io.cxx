// This is vxl/vbl/io/tests/test_array_3d_io.cxx
#include <vcl_iostream.h>
#include <vbl/io/vbl_io_array_3d.h>
#include <testlib/testlib_test.h>

void test_array_3d_io()
{
  vcl_cout << "******************************\n"
           << "Testing vbl_array_3d<float> io\n"
           << "******************************\n";

  //// test constructors, accessors
  const int array_row1 = 8;
  const int array_row2 = 7;
  const int array_row3 = 9;
  vbl_array_3d<int> v_out(array_row1, array_row2, array_row3), v_in;

  for (int i=0; i<array_row1; i++)
    for (int j=0; j< array_row2; j++)
      for (int k=0; k<array_row3; k++)
        v_out(i,j,k) = i*j*j*k;

  vsl_b_ofstream bfs_out("vbl_array_3d_test_io.bvl.tmp");
  TEST("Created vbl_array_3d_test_io.bvl.tmp for writing", (!bfs_out), false);
  vsl_b_write(bfs_out, v_out);
  bfs_out.close();

  vsl_b_ifstream bfs_in("vbl_array_3d_test_io.bvl.tmp");
  TEST("Opened vbl_array_3d_test_io.bvl.tmp for reading", (!bfs_in), false);
  vsl_b_read(bfs_in, v_in);
  TEST("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  //kym - double = not defined for vbl_array_3d
  //TEST("v_out == v_in", v_out, v_in);

  bool test_result = true;
  if (v_out.get_row1_count() != v_in.get_row1_count())
    test_result = false;
  else if (v_out.get_row2_count() != v_in.get_row2_count())
    test_result = false;
  else if (v_out.get_row3_count() != v_in.get_row3_count())
    test_result = false;
  else
  {
    unsigned array_row1 = v_out.get_row1_count();
    unsigned array_row2 = v_out.get_row2_count();
    unsigned array_row3 = v_out.get_row3_count();
    for (unsigned i=0; i<array_row1; i++)
      for (unsigned j=0; j<array_row2; j++)
        for (unsigned k=0; k<array_row3; k++)
          if (v_out(i,j,k) != v_in(i,j,k))
            test_result = false;
  }
  TEST("v_out == v_in", test_result, true);

  vsl_print_summary(vcl_cout, v_in);
  vcl_cout << vcl_endl;
}

TESTMAIN(test_array_3d_io);
