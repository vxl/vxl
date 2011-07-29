#include <vnl/xio/vnl_xio_matrix_fixed.h>
#include <vnl/xio/vnl_xio_quaternion.h>
#include <vnl/xio/vnl_xio_vector.h>
#include <vnl/xio/vnl_xio_vector_fixed.h>

#include <testlib/testlib_test.h>
#include <vcl_sstream.h>

static void test_xio_matrix_fixed()
{
  vcl_stringstream s;

  double data_m[9] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  vnl_matrix_fixed<double,3,3> m(data_m);
  x_write(s, m);
  TEST("XML I/O for vnl_matrix_fixed<double,3,3>", s.str(),
       "<vnl_matrix_fixed rows=\"3\" cols=\"3\">\n1.000000 2.000000 3.000000 4.000000 5.000000 6.000000 7.000000 8.000000 9.000000\n</vnl_matrix_fixed>\n");
}

static void test_xio_vector_fixed()
{
  vcl_stringstream s;

  vnl_vector_fixed<double,3> vf(10.0,20.0,5.0);
  x_write(s, vf);
  TEST("XML I/O for vnl_vector_fixed<double,3>", s.str(),
       "<vnl_vector_fixed size=\"3\">\n10.000000 20.000000 5.000000\n</vnl_vector_fixed>\n");
}

static void test_xio_vector()
{
  vcl_stringstream s;

  double data_v[9] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  vnl_vector<double> v(data_v,9);
  x_write(s, v);
  TEST("XML I/O for vnl_vector<double>", s.str(),
       "<vnl_vector size=\"9\">\n1.000000 2.000000 3.000000 4.000000 5.000000 6.000000 7.000000 8.000000 9.000000\n</vnl_vector>\n");
}

static void test_xio_quaternion()
{
  vcl_stringstream s;

  vnl_quaternion<double> q(1,2,3,4);
  x_write(s, q);
  TEST("XML I/O for vnl_quaternion<double>", s.str(),
       "<vnl_quaternion x=\"1.000000\" y=\"2.000000\" z=\"3.000000\" r=\"4.000000\">\n</vnl_quaternion>\n");
}

static void test_vnl_xio()
{
  test_xio_matrix_fixed();
  test_xio_vector_fixed();
  test_xio_vector();
  test_xio_quaternion();
}

TESTMAIN(test_vnl_xio);
