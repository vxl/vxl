#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_utility.h>

#include <vnl/vnl_test.h>
#include <vnl/vnl_math.h>
#include <vnl/io/vnl_io_vector.h>
#include <vnl/vnl_matrix.h>

#include <vsl/vsl_binary_io.h>


void test_vector_double_io()
{
  vcl_cout << "*****************************" << vcl_endl;
  vcl_cout << "Testing vnl_vector<double> io" << vcl_endl;
  vcl_cout << "*****************************" << vcl_endl;
  //// test constructors, accessors
  const int n = 50;
  vnl_vector<double> v_out(n), v_in;

  for (int i=0; i<n; i++)
  {
    v_out(i) = (double)(i*i);
  }

  vcl_cout << "before saving:\t"; vsl_print_summary(vcl_cout, v_out);

  vsl_b_ofstream bfs_out("vnl_vector_test_double_io.bvl.tmp");
  TEST ("Created vnl_vector_test_double_io.bvl.tmp for writing",
        (!bfs_out), false);
  vsl_b_write(bfs_out, v_out);
  bfs_out.close();

  vcl_cout << "after saving:\t"; vsl_print_summary(vcl_cout, v_out);

  vsl_b_ifstream bfs_in("vnl_vector_test_double_io.bvl.tmp");
  TEST ("Opened vnl_vector_test_double_io.bvl.tmp for reading",
        (!bfs_in), false);
  vsl_b_read(bfs_in, v_in);
  bfs_in.close();

  vcl_cout << "after reading in:\t"; vsl_print_summary(vcl_cout, v_in);

  TEST ("v_out == v_in", v_out == v_in, true);

  vsl_print_summary(vcl_cout, v_out);
  vcl_cout << vcl_endl;
}


void test_vector_prime()
{
  test_vector_double_io();
}


TESTMAIN(test_vector_prime);
