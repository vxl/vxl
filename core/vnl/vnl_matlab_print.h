#ifndef vnl_matlab_print_h_
#define vnl_matlab_print_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  fsm@robots.ox.ac.uk, from awf's MatOps code.
*/
template <class T> class vnl_vector;
template <class T> class vnl_matrix;
template <class T> class vnl_diag_matrix;
#include <vcl/vcl_iosfwd.h>

//: pretty-printing matlab formats
enum vnl_matlab_Format {
  vnl_matlab_fmt_default,
  vnl_matlab_fmt_short,
  vnl_matlab_fmt_long,
  vnl_matlab_fmt_short_e,
  vnl_matlab_fmt_long_e
};

//: print real or complex scalar into character buffer.
template <class T> 
void vnl_matlab_print_scalar(T const &v, 
			     char *buf,
			     vnl_matlab_Format =vnl_matlab_fmt_default);

//: Print in nice MATLAB format.
// If a variable name (e.g. "foo") is given, the raw data will be preceded by
//   "foo = diag([ " for a vnl_diag_matrix
//   "foo = [ ...\n" for a vnl_matrix and
//   "foo = [ "      for a vnl_vector
// and followed by "])\n", "]\n" and "]\n" respectively. If the variable name
// is a null pointer, the data is printed as is.
template <class T>
ostream &vnl_matlab_print(ostream &, 
			  T const *row, 
			  unsigned length,
			  vnl_matlab_Format =vnl_matlab_fmt_default);

template <class T>
ostream &vnl_matlab_print(ostream &, 
			  vnl_diag_matrix<T> const &, 
			  char const *variable_name =0,
			  vnl_matlab_Format =vnl_matlab_fmt_default);

template <class T>
ostream &vnl_matlab_print(ostream &, 
			  vnl_matrix<T> const &, 
			  char const *variable_name =0,
			  vnl_matlab_Format =vnl_matlab_fmt_default);

template <class T>
ostream &vnl_matlab_print(ostream &, 
			  vnl_vector<T> const &, 
			  char const *variable_name =0,
			  vnl_matlab_Format =vnl_matlab_fmt_default);

// -------------------- Setting the default format.

//: set new, get old format at top of stack :
vnl_matlab_Format vnl_matlab_print_format(vnl_matlab_Format);
//: push/pop the top of the stack :
void vnl_matlab_print_format_push(vnl_matlab_Format);
void vnl_matlab_print_format_pop ();



//: naughty naming-convention-defying-but-handy macro.
#define MATLABPRINT(X) (vnl_matlab_print(cerr, X, #X))

#endif
