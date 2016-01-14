#include <cstdlib>
#include <testlib/testlib_test.h>

#include <vcl_iostream.h>
#include <vcl_complex.h>
#include <vnl/vnl_complexify.h>

#include <vnl/vnl_vector.h>
#include <vnl/vnl_vector_fixed.h>

#include <vnl/vnl_matrix.h>
#include <vnl/vnl_matrix_fixed.h>

#include <vnl/vnl_diag_matrix.h>
#include <vnl/vnl_diag_matrix_fixed.h>

#include <vnl/vnl_sym_matrix.h>

void test_complexify()
{

  typedef float TReal;
  const unsigned int length = 2;
  const TReal value = 7.5;

  vcl_cout << "Testing vnl_vector" << vcl_endl;
  vnl_vector<TReal> r_vector(length,value);
  vnl_vector<vcl_complex<TReal> > c_vector
    = vnl_complexify(r_vector);
  for (unsigned int i = 0; i < length; ++i)
    {
    TEST("vnl_vector real",7.5,c_vector.get(i).real());
    TEST("vnl_vector imag",0.0,c_vector.get(i).imag());
    }

  vcl_cout << "Testing vnl_vector_fixed" << vcl_endl;
  vnl_vector_fixed<TReal,length> r_vector_fixed(value);
  vnl_vector_fixed<vcl_complex<TReal>,length > c_vector_fixed
    = vnl_complexify(r_vector_fixed);
  for (unsigned int i = 0; i < length; ++i)
    {
    TEST("vnl_vector_fixed real",7.5,c_vector_fixed.get(i).real());
    TEST("vnl_vector_fixed imag",0.0,c_vector_fixed.get(i).imag());
    }

  vcl_cout << "Testing vnl_matrix" << vcl_endl;
  vnl_matrix<TReal> r_matrix(length,length,value);
  vnl_matrix<vcl_complex<TReal> > c_matrix
    = vnl_complexify(r_matrix);
  for (unsigned int c = 0; c < length; ++c)
    {
    for (unsigned int r = 0; r < length; ++r)
      {
      TEST("vnl_matrix real",7.5,c_matrix.get(r,c).real());
      TEST("vnl_matrix imag",0.0,c_matrix.get(r,c).imag());
      }
    }

  vcl_cout << "Testing vnl_matrix_fixed" << vcl_endl;
  vnl_matrix_fixed<TReal,length,length> r_matrix_fixed(value);
  vnl_matrix_fixed<vcl_complex<TReal>,length,length > c_matrix_fixed
    = vnl_complexify(r_matrix_fixed);
  for (unsigned int c = 0; c < length; ++c)
    {
    for (unsigned int r = 0; r < length; ++r)
      {
      TEST("vnl_matrix_fixed real",7.5,c_matrix_fixed.get(r,c).real());
      TEST("vnl_matrix_fixed imag",0.0,c_matrix_fixed.get(r,c).imag());
      }
    }

  vcl_cout << "Testing vnl_diag_matrix" << vcl_endl;
  vnl_diag_matrix<TReal> r_diag_matrix(length,value);
  vnl_diag_matrix<vcl_complex<TReal> > c_diag_matrix
    = vnl_complexify(r_diag_matrix);
  for (unsigned int i = 0; i < length; ++i)
    {
    TEST("vnl_diag_matrix real",7.5,c_diag_matrix.get(i,i).real());
    TEST("vnl_diag_matrix imag",0.0,c_diag_matrix.get(i,i).imag());
    }

  vcl_cout << "Testing vnl_diag_matrix_fixed" << vcl_endl;
  vnl_diag_matrix_fixed<TReal,length> r_diag_matrix_fixed(value);
  vnl_diag_matrix_fixed<vcl_complex<TReal>,length > c_diag_matrix_fixed
    = vnl_complexify(r_diag_matrix_fixed);
  for (unsigned int i = 0; i < length; ++i)
    {
    TEST("vnl_diag_matrix_fixed real",7.5,c_diag_matrix_fixed.get(i,i).real());
    TEST("vnl_diag_matrix_fixed imag",0.0,c_diag_matrix_fixed.get(i,i).imag());
    }

  vcl_cout << "Testing vnl_sym_matrix" << vcl_endl;
  vnl_sym_matrix<TReal> r_sym_matrix(length,value);
  vnl_sym_matrix<vcl_complex<TReal> > c_sym_matrix
    = vnl_complexify(r_sym_matrix);
  for (unsigned int c = 0; c < length; ++c)
    {
    for (unsigned int r = 0; r < length; ++r)
      {
      TEST("vnl_sym_matrix real",7.5,c_sym_matrix(r,c).real()); // no get()
      TEST("vnl_sym_matrix imag",0.0,c_sym_matrix(r,c).imag());
      }
    }

}

TESTMAIN( test_complexify );
