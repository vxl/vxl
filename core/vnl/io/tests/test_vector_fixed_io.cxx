#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_utility.h>

#include <testlib/testlib_test.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/io/vnl_io_vector.h>

#include <vsl/vsl_binary_io.h>

void test_vector_fixed_double_3_io()
{
  vcl_cout << "*************************************\n";
  vcl_cout << "Testing vnl_vector_fixed<double,3> io\n";
  vcl_cout << "*************************************\n";
  //// test constructors, accessors

  vnl_vector_fixed<double,3> m_out(1.2,3.4,5.6), m_in;

  vsl_b_ofstream bfs_out("vnl_vector_fixed_io.tmp");
  TEST ("vnl_vector_fixed_io.tmp for writing", (!bfs_out), false);
  vsl_b_write(bfs_out, m_out);
  bfs_out.close();

  vsl_b_ifstream bfs_in("vnl_vector_fixed_io.tmp");
  TEST ("vnl_vector_fixed_io.tmp for reading", (!bfs_in), false);
  vsl_b_read(bfs_in, m_in);
  bfs_in.close();

  TEST ("m_out == m_in", m_out, m_in);

  vsl_print_summary(vcl_cout, m_out);
  vcl_cout << vcl_endl;
}


void test_vector_fixed_io()
{
  test_vector_fixed_double_3_io();
}


TESTMAIN(test_vector_fixed_io);
