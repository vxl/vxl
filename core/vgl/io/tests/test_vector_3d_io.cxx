#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_utility.h>

#include <testlib/testlib_test.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/io/vgl_io_vector_3d.h>

void test_vector_3d_double_io()
{
  vcl_cout << "********************************\n"
           << "Testing vgl_vector_3d<double> io\n"
           << "********************************\n";

  //// test constructors, accessors
  vgl_vector_3d<double> v_out(1.2,-3.4,5.6), v_in;

  vsl_b_ofstream bfs_out("vgl_vector_3d_test_double_io.bvl.tmp");
  TEST ("Created vgl_vector_3d_test_double_io.bvl.tmp for writing",
        (!bfs_out), false);
  vsl_b_write(bfs_out, v_out);
  bfs_out.close();

  vsl_b_ifstream bfs_in("vgl_vector_3d_test_double_io.bvl.tmp");
  TEST ("Opened vgl_vector_3d_test_double_io.bvl.tmp for reading",
        (!bfs_in), false);
  vsl_b_read(bfs_in, v_in);
  TEST ("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  TEST ("v_out == v_in", v_out, v_in);

  vsl_print_summary(vcl_cout, v_out);
  vcl_cout << vcl_endl;
}

void test_vector_3d_float_io()
{
  vcl_cout << "*******************************\n"
           << "Testing vgl_vector_3d<float> io\n"
           << "*******************************\n";

  //// test constructors, accessors
  vgl_vector_3d<float> v_out(1.2f,-3.4f,5.6f), v_in;

  vsl_b_ofstream bfs_out("vgl_vector_3d_test_float_io.bvl.tmp");
  TEST ("Created vgl_vector_3d_test_float_io.bvl.tmp for writing",
        (!bfs_out), false);
  vsl_b_write(bfs_out, v_out);
  bfs_out.close();

  vsl_b_ifstream bfs_in("vgl_vector_3d_test_float_io.bvl.tmp");
  TEST ("Opened vgl_vector_3d_test_float_io.bvl.tmp for reading",
        (!bfs_in), false);
  vsl_b_read(bfs_in, v_in);
  TEST ("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  TEST ("v_out == v_in", v_out, v_in);

  vsl_print_summary(vcl_cout, v_out);
  vcl_cout << vcl_endl;
}

void test_vector_3d_int_io()
{
  vcl_cout << "*****************************\n"
           << "Testing vgl_vector_3d<int> io\n"
           << "*****************************\n";

  //// test constructors, accessors
  vgl_vector_3d<int> v_out(1,-3,5), v_in;

  vsl_b_ofstream bfs_out("vgl_vector_3d_test_int_io.bvl.tmp");
  TEST ("Created vgl_vector_3d_test_int_io.bvl.tmp for writing",
        (!bfs_out), false);
  vsl_b_write(bfs_out, v_out);
  bfs_out.close();

  vsl_b_ifstream bfs_in("vgl_vector_3d_test_int_io.bvl.tmp");
  TEST ("Opened vgl_vector_3d_test_int_io.bvl.tmp for reading",
        (!bfs_in), false);
  vsl_b_read(bfs_in, v_in);
  TEST ("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  TEST ("v_out == v_in", v_out, v_in);

  vsl_print_summary(vcl_cout, v_out);
  vcl_cout << vcl_endl;
}

void test_vector_3d_io()
{
  test_vector_3d_double_io();
  test_vector_3d_float_io();
  test_vector_3d_int_io();
}

TESTMAIN(test_vector_3d_io);
