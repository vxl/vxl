/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
#endif

// Adapted from awf's MatOps class.

#include "vnl_matlab_print.h"

#include <vcl/vcl_cstdio.h>  // sprintf()
#include <vcl/vcl_cstdlib.h> // abort()
#include <vcl/vcl_cstring.h> // strlen()
#include <vcl/vcl_cctype.h>
#include <vcl/vcl_cassert.h>
#include <vcl/vcl_iostream.h>
#include <vcl/vcl_vector.h>

#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_diag_matrix.h>
#include <vnl/vnl_complex.h>


// -- Choose precision in printouts.
// 
// vnl_matlab_format(vnl_matops::fmt_long) selects 16-digit precision
// 
// vnl_matlab_format(vnl_matops::fmt_short) selects 4-digit precision
// 

//: this variable is the current top of the stack.
static vnl_matlab_Format the_format = vnl_matlab_fmt_short;
//: the rest of the stack is stored in this vector.
static vcl_vector<int> *format_stack = 0;
//: call this to initialize the format stack.
static void vnl_matlab_print_format_init() { if (!format_stack) format_stack = new vcl_vector<int>; }

void vnl_matlab_print_format_push(vnl_matlab_Format f)
{
  vnl_matlab_print_format_init();
  format_stack->push_back(the_format);
  the_format = f;
}

void vnl_matlab_print_format_pop()
{
  vnl_matlab_print_format_init();
  if (format_stack->empty())
    cerr << __FILE__ ": format stack empty" << endl;
  else {
    the_format = vnl_matlab_Format(format_stack->back());
    format_stack->pop_back();
  }
}

vnl_matlab_Format vnl_matlab_print_format(vnl_matlab_Format f)
{
  vnl_matlab_print_format_init();
  vnl_matlab_Format old = the_format;
  the_format = f;
  return old;
}

//--------------------------------------------------------------------------------

VCL_DEFINE_SPECIALIZATION
void vnl_matlab_print_scalar(int const &v, 
			     char *buf, 
			     vnl_matlab_Format VCL_DEFAULT_VALUE(vnl_matlab_fmt_default))
{
  sprintf(buf, "%4d ", v);
}

VCL_DEFINE_SPECIALIZATION
void vnl_matlab_print_scalar(float const &v, 
			     char *buf, 
			     vnl_matlab_Format format VCL_DEFAULT_VALUE(vnl_matlab_fmt_default))
{
  if (format == vnl_matlab_fmt_default)
    format = the_format;
  switch (format) {
  case vnl_matlab_fmt_long:
    if (v == 0.0) 
      sprintf(buf, "%8d ", 0);
    else
      sprintf(buf, "%8.5f ", v);
    break;
  case vnl_matlab_fmt_short:
    if (v == 0.0)
      sprintf(buf, "%6d ", 0);
    else
      sprintf(buf, "%6.3f ", v);
    break;
  case vnl_matlab_fmt_long_e:
    sprintf(buf, "%11.7e ", v);
    break;
  case vnl_matlab_fmt_short_e:
    sprintf(buf, "%8.4e ", v);
    break;
  default:/*vnl_matlab_fmt_default:*/ abort(); break;
  }
}

VCL_DEFINE_SPECIALIZATION
void vnl_matlab_print_scalar(double const &v, 
			     char *buf,
			     vnl_matlab_Format format VCL_DEFAULT_VALUE(vnl_matlab_fmt_default))
{
  if (format == vnl_matlab_fmt_default)
    format = the_format;
  switch (format) {
  case vnl_matlab_fmt_long:
    if (v == 0.0) 
      sprintf(buf, "%16d ", 0);
    else
      sprintf(buf, "%16.13f ", v);
    break;
  case vnl_matlab_fmt_short:
    if (v == 0.0)
      sprintf(buf, "%8d ", 0);
    else
      sprintf(buf, "%8.4f ", v);
    break;
  case vnl_matlab_fmt_long_e:
    sprintf(buf, "%20.14e ", v);
    break;
  case vnl_matlab_fmt_short_e:
    sprintf(buf, "%10.4e ", v);
    break;
  default:/*vnl_matlab_fmt_default:*/ abort(); break;
  }
}

VCL_DEFINE_SPECIALIZATION
void vnl_matlab_print_scalar(vnl_double_complex const &v, 
			     char *buf,
			     vnl_matlab_Format format VCL_DEFAULT_VALUE(vnl_matlab_fmt_default))
{
  if (format == vnl_matlab_fmt_default)
    format = the_format;
  int width = 16;
  int precision = 12;
  char conv = 'f';

  switch (format) {
  case vnl_matlab_fmt_long:
  case vnl_matlab_fmt_long_e:
    width = 16;
    precision = 12;
    break;
  case vnl_matlab_fmt_short:
  case vnl_matlab_fmt_short_e:
    width = 8;
    precision = 4;
    break;
  default:/*vnl_matlab_fmt_default:*/ abort(); break;
  }

  switch (format) {
  case vnl_matlab_fmt_long:
  case vnl_matlab_fmt_short:
    conv = 'f';
    break;
  case vnl_matlab_fmt_long_e:
  case vnl_matlab_fmt_short_e:
    conv = 'e';
    break;
  default:/*vnl_matlab_fmt_default:*/ abort(); break;
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
  
  // Imaginary part.  Width is reduced as sign is taken care of separately
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

VCL_DEFINE_SPECIALIZATION
void vnl_matlab_print_scalar(vnl_float_complex const &v, 
			     char *buf,
			     vnl_matlab_Format format VCL_DEFAULT_VALUE(vnl_matlab_fmt_default))
{
  if (format == vnl_matlab_fmt_default)
    format = the_format;
  int width = 10;
  int precision = 6;
  char conv = 'f';

  switch (format) {
  case vnl_matlab_fmt_long:
  case vnl_matlab_fmt_long_e:
    width = 10;
    precision = 6;
    break;
  case vnl_matlab_fmt_short:
  case vnl_matlab_fmt_short_e:
    width = 8;
    precision = 4;
    break;
  default:/*vnl_matlab_fmt_default:*/ abort(); break;
  }

  switch (format) {
  case vnl_matlab_fmt_long:
  case vnl_matlab_fmt_short:
    conv = 'f';
    break;
  case vnl_matlab_fmt_long_e:
  case vnl_matlab_fmt_short_e:
    conv = 'e';
    break;
  default:/*vnl_matlab_fmt_default:*/ abort(); break;
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
  
  // Imaginary part.  Width is reduced as sign is taken care of separately
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

//--------------------------------------------------------------------------------

template <class T>
ostream &vnl_matlab_print(ostream& s, 
			  T const* row, 
			  unsigned length,
			  vnl_matlab_Format format VCL_DEFAULT_VALUE(vnl_matlab_fmt_default))
{
  char buf[1024];
  for (int j=0; j<length; j++ ) {
    // Format according to selected style
    // In both cases an exact 0 goes out as such
    vnl_matlab_print_scalar(row[j], buf, format);
    s << buf;
  }
  
  return s;
}

template <class T>
ostream& vnl_matlab_print(ostream& s, 
			  vnl_diag_matrix<T> const& D, 
			  char const* variable_name,
			  vnl_matlab_Format format VCL_DEFAULT_VALUE(vnl_matlab_fmt_default))
{
  if (variable_name) 
    s << variable_name << " = diag([ ";
  
  vnl_matlab_print(s, D.begin(), D.size(), format);
  
  if (variable_name) {
    s << " ])";
    s << endl;
  }
  
  return s;
}

template <class T>
ostream& vnl_matlab_print(ostream& s,
			  vnl_matrix<T> const& M, 
			  char const* variable_name,
			  vnl_matlab_Format format VCL_DEFAULT_VALUE(vnl_matlab_fmt_default))
{
  if (variable_name) 
    s << variable_name << " = [ ... \n";
  
  for (int i=0; i<M.rows(); i++ ) {
    vnl_matlab_print(s, M[i], M.cols(), format);
    
    if (variable_name && (i == M.rows()-1))
      s << " ]";
    
    s << endl;
  }
  
  return s;
}

template <class T>
ostream& vnl_matlab_print(ostream& s,
			  vnl_vector<T> const & v, 
			  char const* variable_name,
			  vnl_matlab_Format format VCL_DEFAULT_VALUE(vnl_matlab_fmt_default))
{
  if (variable_name) 
    s << variable_name << " = [ ";
  
  vnl_matlab_print(s, v.begin(), v.size(), format);
  
  if (variable_name) {
    s << " ]";
    s << endl;
  }
  
  return s;
}

//--------------------------------------------------------------------------------

// -- Can be used within debugger to print matrix
extern "C"
void vnl_dbprintmx(vnl_matrix<double> const& p)
{
#if defined(VCL_GCC_27)
  // a mysterious error :
  //vnl_matlab.cxx: In function `void dbprintmx(const class vnl_matrix<double> &)':
  //336: call of overloaded `vnl_matlab_print' is ambiguous
  //216: candidates are: vnl_matlab_print(ostream &, const vnl_matrix<double> &, const char *)
  //                     vnl_matlab_print(ostream &, const vnl_matrix<double> &, const char *)
  cerr << "[" << endl << p << "]" << endl;
  abort();
#else
  // why the cast? is it a const_cast?
  vnl_matlab_print(cerr, p, (char const*)"M", vnl_matlab_fmt_default);
#endif
}

//--------------------------------------------------------------------------------

#define inst(T) \
template ostream &vnl_matlab_print(ostream &, T const *, unsigned, vnl_matlab_Format); \
template ostream &vnl_matlab_print(ostream &, vnl_diag_matrix<T> const &, char const *, vnl_matlab_Format); \
template ostream &vnl_matlab_print(ostream &, vnl_matrix<T> const &, char const *, vnl_matlab_Format); \
template ostream &vnl_matlab_print(ostream &, vnl_vector<T> const &, char const *, vnl_matlab_Format);
inst(int);
inst(float);
inst(double);
inst(vnl_float_complex);
inst(vnl_double_complex);
#undef inst
