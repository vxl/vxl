// This is core/vgl/io/tests/test_vector_2d_io.cxx
#include <vcl_iostream.h>
#include <vgl/vgl_vector_2d.h>
#include <vgl/io/vgl_io_vector_2d.h>
#include <vsl/vsl_indent.h>
#include <vsl/vsl_binary_io.h>
#include <testlib/testlib_test.h>
#include <vpl/vpl.h>

void test_vector_2d_double_io()
{
  vcl_cout << "********************************\n"
           << "Testing vgl_vector_2d<double> io\n"
           << "********************************\n";

  //// test constructors, accessors
  vgl_vector_2d<double> v_out(1.2,-3.4), v_in;

  vsl_b_ofstream bfs_out("vgl_vector_2d_test_double_io.bvl.tmp");
  TEST("Created vgl_vector_2d_test_double_io.bvl.tmp for writing",
       (!bfs_out), false);
  vsl_b_write(bfs_out, v_out);
  bfs_out.close();

  vsl_b_ifstream bfs_in("vgl_vector_2d_test_double_io.bvl.tmp");
  TEST ("Opened vgl_vector_2d_test_double_io.bvl.tmp for reading",
        (!bfs_in), false);
  vsl_b_read(bfs_in, v_in);
  TEST ("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  vpl_unlink ("vgl_vector_2d_test_double_io.bvl.tmp");

  TEST ("v_out == v_in", v_out, v_in);

  vsl_print_summary(vcl_cout, v_out);
  vcl_cout << vcl_endl;
}

void test_vector_2d_float_io()
{
  vcl_cout << "*******************************\n"
           << "Testing vgl_vector_2d<float> io\n"
           << "*******************************\n";

  //// test constructors, accessors
  vgl_vector_2d<float> v_out(1.2f,-3.4f), v_in;

  vsl_b_ofstream bfs_out("vgl_vector_2d_test_float_io.bvl.tmp");
  TEST("Created vgl_vector_2d_test_float_io.bvl.tmp for writing",
       (!bfs_out), false);
  vsl_b_write(bfs_out, v_out);
  bfs_out.close();

  vsl_b_ifstream bfs_in("vgl_vector_2d_test_float_io.bvl.tmp");
  TEST ("Opened vgl_vector_2d_test_float_io.bvl.tmp for reading",
        (!bfs_in), false);
  vsl_b_read(bfs_in, v_in);
  TEST ("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  vpl_unlink ("vgl_vector_2d_test_float_io.bvl.tmp");

  TEST ("v_out == v_in", v_out, v_in);

  vsl_print_summary(vcl_cout, v_out);
  vcl_cout << vcl_endl;
}

void test_vector_2d_int_io()
{
  vcl_cout << "*****************************\n"
           << "Testing vgl_vector_2d<int> io\n"
           << "*****************************\n";

  //// test constructors, accessors
  vgl_vector_2d<int> v_out(1,-3), v_in;

  vsl_b_ofstream bfs_out("vgl_vector_2d_test_int_io.bvl.tmp");
  TEST("Created vgl_vector_2d_test_int_io.bvl.tmp for writing",
       (!bfs_out), false);
  vsl_b_write(bfs_out, v_out);
  bfs_out.close();

  vsl_b_ifstream bfs_in("vgl_vector_2d_test_int_io.bvl.tmp");
  TEST ("Opened vgl_vector_2d_test_int_io.bvl.tmp for reading",
        (!bfs_in), false);
  vsl_b_read(bfs_in, v_in);
  TEST ("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  vpl_unlink ("vgl_vector_2d_test_int_io.bvl.tmp");

  TEST ("v_out == v_in", v_out, v_in);

  vsl_print_summary(vcl_cout, v_out);
  vcl_cout << vcl_endl;
}

void test_vector_2d_io()
{
  test_vector_2d_double_io();
  test_vector_2d_float_io();
  test_vector_2d_int_io();
  vsl_indent_clear_all_data();
}

TESTMAIN(test_vector_2d_io);
