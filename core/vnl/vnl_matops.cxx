#ifdef __GNUC__
#pragma implementation
#endif
//
// Class: vnl_matops
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 05 Aug 96
// Modifications:
//
//-----------------------------------------------------------------------------

#include <vnl/vnl_complex.h>
#include "vnl_matops.h"
#include <vnl/vnl_math.h>
#include <vcl/vcl_vector.h>
#include <vcl/vcl_cstdio.h> // for sprintf()
#include <vcl/vcl_cstdlib.h> // for abort()
#include <vcl/vcl_cstring.h> // for strlen()
#include <ctype.h>
#include <assert.h>

vnl_matrix<double> vnl_matops::cat(vnl_matrix<double> const &A, vnl_matrix<double> const &B) {
  int rowsA = A.rows();
  int colsA = A.columns();
  int rowsB = B.rows();
  int colsB = B.columns();

  assert(rowsA == rowsB);

  vnl_matrix<double> M(rowsA,colsA+colsB);
  M.update(A,0,0);
  M.update(B,0,colsA);

  return M;
}

vnl_matrix<double> vnl_matops::cat(vnl_matrix<double> const &A, const vnl_vector<double> &B) {
  int rowsA = A.rows();
  int colsA = A.columns();
  int rowsB = B.size();

  assert(rowsA == rowsB);

  vnl_matrix<double> M(rowsA,colsA+1);
  M.update(A,0,0);
  M.set_column(colsA,B);

  return M;
}

vnl_matrix<double> vnl_matops::cat(const vnl_vector<double> &A, vnl_matrix<double> const &B) {
  int rowsA = A.size();
  int rowsB = B.rows();
  int colsB = B.columns();

  assert(rowsA == rowsB);

  vnl_matrix<double> M(rowsA,colsB+1);
  M.set_column(0,A);
  M.update(B,0,1);

  return M;
}

vnl_matrix<double> vnl_matops::vcat(vnl_matrix<double> const &A, vnl_matrix<double> const &B) {
  int rowsA = A.rows();
  int colsA = A.columns();
  int rowsB = B.rows();
  int colsB = B.columns();

  assert(colsA == colsB);
  
  vnl_matrix<double> M(rowsA+rowsB,colsA);
  M.update(A,0,0);
  M.update(B,rowsA,0);

  return M;
}

void vnl_matops::print_float(float v, char *buf)
{
  switch (matlab_format) {
  case fmt_long:
    if (v == 0.0) 
      sprintf(buf, "%8d ", 0);
    else
      sprintf(buf, "%8.5f ", v);
    break;
  case fmt_short:
    if (v == 0.0)
      sprintf(buf, "%6d ", 0);
    else
      sprintf(buf, "%6.3f ", v);
    break;
  case fmt_long_e:
    sprintf(buf, "%11.7e ", v);
    break;
  case fmt_short_e:
    sprintf(buf, "%8.4e ", v);
    break;
  }
}

void vnl_matops::print_double(double v, char *buf)
{
  switch (matlab_format) {
  case fmt_long:
    if (v == 0.0) 
      sprintf(buf, "%16d ", 0);
    else
      sprintf(buf, "%16.14f ", v);
    break;
  case fmt_short:
    if (v == 0.0)
      sprintf(buf, "%8d ", 0);
    else
      sprintf(buf, "%8.4f ", v);
    break;
  case fmt_long_e:
    sprintf(buf, "%20.14e ", v);
    break;
  case fmt_short_e:
    sprintf(buf, "%10.4e ", v);
    break;
  }
}

void vnl_matops::print_double_complex(vnl_double_complex v, char *buf)
{
  int width = 16;
  int precision = 12;
  char conv = 'f';

  switch (matlab_format) {
  case fmt_long:
  case fmt_long_e:
    width = 16;
    precision = 12;
    break;
  case fmt_short:
  case fmt_short_e:
    width = 8;
    precision = 4;
    break;
  }

  switch (matlab_format) {
  case fmt_long:
  case fmt_short:
    conv = 'f';
    break;
  case fmt_long_e:
  case fmt_short_e:
    conv = 'e';
    break;
  }
  
  double r = v.real();
  double i = v.imag();

  char fmt[1024];
  // Real part
  if (r == 0) {
    sprintf(fmt, "%%" "%d" "d ", width);
    sprintf(buf, fmt, 0);
    
  } else {
    sprintf(fmt, "%%" "%d" "." "%d" "%c ", width, precision, conv);
    sprintf(buf, fmt, r);
  }

  buf += strlen(buf);
  
  // Imaginary part.  Width is reduced as sign is taken care of seperately
  if (i == 0) {
    sprintf(fmt, " %%" "%d" "s  ", width-1);
    sprintf(buf, fmt, "");
  } else {
    char sign = '+';
    if (i < 0) {
      sign = '-';
      i = -i;
    }
    sprintf(fmt, "%c%%" "%d.%d%ci ", sign, width-1, precision, conv);
    sprintf(buf, fmt, i);
  }
}

void vnl_matops::print_float_complex(vnl_float_complex v, char *buf)
{
  int width = 10;
  int precision = 6;
  char conv = 'f';

  switch (matlab_format) {
  case fmt_long:
  case fmt_long_e:
    width = 10;
    precision = 6;
    break;
  case fmt_short:
  case fmt_short_e:
    width = 8;
    precision = 4;
    break;
  }

  switch (matlab_format) {
  case fmt_long:
  case fmt_short:
    conv = 'f';
    break;
  case fmt_long_e:
  case fmt_short_e:
    conv = 'e';
    break;
  }
  
  float r = v.real();
  float i = v.imag();

  char fmt[1024];
  // Real part
  if (r == 0) {
    sprintf(fmt, "%%" "%d" "d ", width);
    sprintf(buf, fmt, 0);
    
  } else {
    sprintf(fmt, "%%" "%d" "." "%d" "%c ", width, precision, conv);
    sprintf(buf, fmt, r);
  }

  buf += strlen(buf);
  
  // Imaginary part.  Width is reduced as sign is taken care of seperately
  if (i == 0) {
    sprintf(fmt, " %%" "%d" "s  ", width-1);
    sprintf(buf, fmt, "");
  } else {
    char sign = '+';
    if (i < 0) {
      sign = '-';
      i = -i;
    }
    sprintf(fmt, "%c%%" "%d.%d%ci ", sign, width-1, precision, conv);
    sprintf(buf, fmt, i);
  }
}

// -- Print in nice MATLAB columnar format
ostream& vnl_matops::matlab_print(ostream& s, vnl_matrix<double> const& M, const char* variable_name)
{
  if (variable_name) 
    s << variable_name << " = [ ... \n";

  char buf[1024];
  int m = M.rows();
  int n = M.columns();
  for (int i=0; i<m; i++ ) {
    for (int j=0; j<n; j++ ) {
      print_double(M(i,j), buf);
      s << buf;
    }
    
    if (variable_name && (i == m-1))
      s << " ]";
    s << endl;
  }
  
  return s;
}

ostream& vnl_matops::matlab_print(ostream& s, const vnl_matrix<vnl_double_complex>& M, const char* variable_name)
{
  if (variable_name) 
    s << variable_name << " = [ ... \n";

  char buf[1024];
  int m = M.rows();
  int n = M.columns();
  for (int i=0; i<m; i++ ) {
    for (int j=0; j<n; j++ ) {
      print_double_complex(M(i,j), buf);
      s << buf;
    }
    
    if (variable_name && (i == m-1))
      s << " ]";
    s << endl;
  }
  
  return s;
}

ostream& vnl_matops::matlab_print(ostream& s, const vnl_matrix<vnl_float_complex>& M, const char* variable_name)
{
  if (variable_name) 
    s << variable_name << " = [ ... \n";

  char buf[1024];
  int m = M.rows();
  int n = M.columns();
  for (int i=0; i<m; i++ ) {
    for (int j=0; j<n; j++ ) {
      print_float_complex(M(i,j), buf);
      s << buf;
    }
    
    if (variable_name && (i == m-1))
      s << " ]";
    s << endl;
  }
  
  return s;
}

ostream& vnl_matops::matlab_print(ostream& s, const vnl_vector<double>& v, const char* variable_name)
{
  if (variable_name) 
    s << variable_name << " = [ ";
  
  char buf[1024];
  int n = v.size();
  for (int j=0; j<n; j++ ) {
    // Format accoriding to selected style
    // In both cases an exact 0 goes out as such
    print_double(v[j], buf);
    s << buf;
  }
  
  if (variable_name) {
    s << " ]";
    s << endl;
  }
  
  return s;
}


ostream& vnl_matops::matlab_print(ostream& s, const vnl_matrix<float>& M, const char* variable_name)
{
  if (variable_name) 
    s << variable_name << " = [ ... \n";

  char buf[1024];
  int m = M.rows();
  int n = M.columns();
  for (int i=0; i<m; i++ ) {
    for (int j=0; j<n; j++ ) {
      print_float(M(i,j), buf);
      s << buf;
    }
    
    if (variable_name && (i == m-1))
      s << " ]";
    s << endl;
  }
  
  return s;
}

// -- Choose precision in printouts.
// 
// vnl_matops::format(vnl_matops::fmt_long) selects 16-digit precision
// 
// vnl_matops::format(vnl_matops::fmt_short) selects 4-digit precision
// 
vnl_matops::Format vnl_matops::format(vnl_matops::Format f)
{
  Format old = matlab_format;
  matlab_format = f;
  return old;
}

vnl_matops::Format vnl_matops::matlab_format = vnl_matops::fmt_short;

extern "C" int dtrans_(double *a, const int& m, const int& n, const int& mn, int* move, const int& iwrk, int* iok);

// -- Return fro_norm( (A ./ B) - mean(A ./ B) )
double vnl_matops::homg_diff(vnl_matrix<double> const& A, vnl_matrix<double> const& B)
{
  vnl_matrix<double> ratio = element_quotient(A, B);
  
  return (ratio - ratio.mean()).fro_norm();
}

#define implement_converters(U,V)                           \
vnl_matrix<U> make_matrix_ ## U(const vnl_matrix<V>& M)    \
{							    \
  unsigned m = M.rows();				    \
  unsigned n = M.columns();				    \
  vnl_matrix<U> ret(m, n);				    \
  for(unsigned i = 0; i < m; ++i)			    \
    for(unsigned j = 0; j < n; ++j)			    \
      ret(i,j) = M(i,j);				    \
  return ret;						    \
}							    \
							    \
vnl_vector<U> make_vector_ ## U(const vnl_vector<V>& v)    \
{							    \
  unsigned n = v.size();  				    \
  vnl_vector<U> ret(n);					    \
  for(unsigned i = 0; i < n; ++i)			    \
    ret[i] = v[i];					    \
  return ret;						    \
}							    \

implement_converters(double,float)

implement_converters(float,double)

vnl_matrix<double>  vnl_matops::f2d(const vnl_matrix<float>& M)
{
  return make_matrix_double(M);
}


vnl_matrix<float>  vnl_matops::d2f(vnl_matrix<double> const& M)
{
  return make_matrix_float(M);
}

vnl_vector<double>  vnl_matops::f2d(const vnl_vector<float>& M)
{
  return make_vector_double(M);
}


vnl_vector<float>  vnl_matops::d2f(const vnl_vector<double>& M)
{
  return make_vector_float(M);
}

// -- Can be used within debugger to print matrix
extern "C"
void dbprintmx(vnl_matrix<double> const& p)
{
  vnl_matops::matlab_print(cerr, p, "M");
}
