#ifndef vnl_matlab_print_h_
#define vnl_matlab_print_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  fsm@robots.ox.ac.uk
*/

template <class T> class vnl_vector;
template <class T> class vnl_matrix;
#include <vcl/vcl_iosfwd.h>
#include <vcl/vcl_complex_fwd.h>

//: pretty-printing matlab formats
enum vnl_matlab_Format {
  vnl_matlab_fmt_short,
  vnl_matlab_fmt_long,
  vnl_matlab_fmt_short_e,
  vnl_matlab_fmt_long_e
};
// set new, get old format :
vnl_matlab_Format vnl_matlab_print_format(vnl_matlab_Format);


//: Print in nice MATLAB columnar format
template <class T>
ostream &vnl_matlab_print(ostream &, vnl_matrix<T> const &, char const *variable_name =0);

template <class T>
ostream &vnl_matlab_print(ostream &, vnl_vector<T> const &, char const *variable_name =0);


//: print real or complex scalar into character buffer.
template <class T> void vnl_matlab_print_scalar(T const &v, char *buf);
// void vnl_matlab_print_float(float v, char *buf);
// void vnl_matlab_print_double(double v, char *buf);
// void vnl_matlab_print_float_complex(vcl_float_complex const &v, char *buf);
// void vnl_matlab_print_double_complex(vcl_double_complex const &v, char *buf);

#define MATLABPRINT(X) (vnl_matlab_print(cerr, X, #X))

#endif
