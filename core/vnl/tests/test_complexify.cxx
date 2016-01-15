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
  const TReal r = 7.5;
  const TReal i = 8.5;

  vcl_cout << "Testing vnl_vector" << vcl_endl;
  const vnl_vector<TReal> r_vector(length,r);
  const vnl_vector<TReal> i_vector(length,i);
  vnl_vector<vcl_complex<TReal> > c_vector
    = vnl_complexify(r_vector);
  for (unsigned int i = 0; i < length; ++i)
    {
    TEST("vnl_vector real",7.5,c_vector.get(i).real());
    TEST("vnl_vector imag",0.0,c_vector.get(i).imag());
    }
  c_vector
    = vnl_complexify(r_vector,i_vector);
  for (unsigned int i = 0; i < length; ++i)
    {
    TEST("vnl_vector real",7.5,c_vector.get(i).real());
    TEST("vnl_vector imag",8.5,c_vector.get(i).imag());
    }

  vcl_cout << "Testing vnl_vector_fixed" << vcl_endl;
  const vnl_vector_fixed<TReal,length> r_vector_fixed(r);
  const vnl_vector_fixed<TReal,length> i_vector_fixed(i);
  vnl_vector_fixed<vcl_complex<TReal>,length > c_vector_fixed
    = vnl_complexify(r_vector_fixed);
  for (unsigned int i = 0; i < length; ++i)
    {
    TEST("vnl_vector_fixed real",7.5,c_vector_fixed.get(i).real());
    TEST("vnl_vector_fixed imag",0.0,c_vector_fixed.get(i).imag());
    }
  c_vector_fixed
    = vnl_complexify(r_vector_fixed,i_vector_fixed);
  for (unsigned int i = 0; i < length; ++i)
    {
    TEST("vnl_vector_fixed real",7.5,c_vector_fixed.get(i).real());
    TEST("vnl_vector_fixed imag",8.5,c_vector_fixed.get(i).imag());
    }

  vcl_cout << "Testing vnl_matrix" << vcl_endl;
  const vnl_matrix<TReal> r_matrix(length,length,r);
  const vnl_matrix<TReal> i_matrix(length,length,i);
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
  c_matrix
    = vnl_complexify(r_matrix,i_matrix);
  for (unsigned int c = 0; c < length; ++c)
    {
    for (unsigned int r = 0; r < length; ++r)
      {
      TEST("vnl_matrix real",7.5,c_matrix.get(r,c).real());
      TEST("vnl_matrix imag",8.5,c_matrix.get(r,c).imag());
      }
    }

  vcl_cout << "Testing vnl_matrix_fixed" << vcl_endl;
  const vnl_matrix_fixed<TReal,length,length> r_matrix_fixed(r);
  const vnl_matrix_fixed<TReal,length,length> i_matrix_fixed(i);
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
  c_matrix_fixed
    = vnl_complexify(r_matrix_fixed,i_matrix_fixed);
  for (unsigned int c = 0; c < length; ++c)
    {
    for (unsigned int r = 0; r < length; ++r)
      {
      TEST("vnl_matrix_fixed real",7.5,c_matrix_fixed.get(r,c).real());
      TEST("vnl_matrix_fixed imag",8.5,c_matrix_fixed.get(r,c).imag());
      }
    }

  vcl_cout << "Testing vnl_diag_matrix" << vcl_endl;
  const vnl_diag_matrix<TReal> r_diag_matrix(length,r);
  const vnl_diag_matrix<TReal> i_diag_matrix(length,i);
  vnl_diag_matrix<vcl_complex<TReal> > c_diag_matrix
    = vnl_complexify(r_diag_matrix);
  for (unsigned int i = 0; i < length; ++i)
    {
    TEST("vnl_diag_matrix real",7.5,c_diag_matrix.get(i,i).real());
    TEST("vnl_diag_matrix imag",0.0,c_diag_matrix.get(i,i).imag());
    }
  c_diag_matrix
    = vnl_complexify(r_diag_matrix,i_diag_matrix);
  for (unsigned int i = 0; i < length; ++i)
    {
    TEST("vnl_diag_matrix real",7.5,c_diag_matrix.get(i,i).real());
    TEST("vnl_diag_matrix imag",8.5,c_diag_matrix.get(i,i).imag());
    }

  vcl_cout << "Testing vnl_diag_matrix_fixed" << vcl_endl;
  const vnl_diag_matrix_fixed<TReal,length> r_diag_matrix_fixed(r);
  const vnl_diag_matrix_fixed<TReal,length> i_diag_matrix_fixed(i);
  vnl_diag_matrix_fixed<vcl_complex<TReal>,length > c_diag_matrix_fixed
    = vnl_complexify(r_diag_matrix_fixed);
  for (unsigned int i = 0; i < length; ++i)
    {
    TEST("vnl_diag_matrix_fixed real",7.5,c_diag_matrix_fixed.get(i,i).real());
    TEST("vnl_diag_matrix_fixed imag",0.0,c_diag_matrix_fixed.get(i,i).imag());
    }
  c_diag_matrix_fixed
    = vnl_complexify(r_diag_matrix_fixed,i_diag_matrix_fixed);
  for (unsigned int i = 0; i < length; ++i)
    {
    TEST("vnl_diag_matrix_fixed real",7.5,c_diag_matrix_fixed.get(i,i).real());
    TEST("vnl_diag_matrix_fixed imag",8.5,c_diag_matrix_fixed.get(i,i).imag());
    }

  vcl_cout << "Testing vnl_sym_matrix" << vcl_endl;
  const vnl_sym_matrix<TReal> r_sym_matrix(length,r);
  const vnl_sym_matrix<TReal> i_sym_matrix(length,i);
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
  c_sym_matrix
    = vnl_complexify(r_sym_matrix,i_sym_matrix);
  for (unsigned int c = 0; c < length; ++c)
    {
    for (unsigned int r = 0; r < length; ++r)
      {
      TEST("vnl_sym_matrix real",7.5,c_sym_matrix(r,c).real()); // no get()
      TEST("vnl_sym_matrix imag",8.5,c_sym_matrix(r,c).imag());
      }
    }

}

TESTMAIN( test_complexify );
