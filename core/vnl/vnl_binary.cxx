/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
#endif
#include "vnl_binary.h"

#include <vcl/vcl_iostream.h>

#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_diag_matrix.h>
#include <vnl/vnl_resize.h>

#include <vcl/vcl_complex_fwd.h>
template <class T> struct vnl_binary_scalar_traits;
VCL_DEFINE_SPECIALIZATION struct vnl_binary_scalar_traits<int> { enum { id = 0x37AF98EB }; };
VCL_DEFINE_SPECIALIZATION struct vnl_binary_scalar_traits<float> { enum { id = 0x12345678 }; };
VCL_DEFINE_SPECIALIZATION struct vnl_binary_scalar_traits<double> { enum { id = 0x56781234 }; };
VCL_DEFINE_SPECIALIZATION struct vnl_binary_scalar_traits<vcl_complex<float> > { enum { id = 0xABCDEF01 }; };
VCL_DEFINE_SPECIALIZATION struct vnl_binary_scalar_traits<vcl_complex<double> > { enum { id = 0xEF01ABCD }; };

// KAI C++ wants char*, so it must be correct....
#define stream_cast (char*)

// -------------------- vnl_vector

template <class T>
bool vnl_binary_save(ostream &f, vnl_vector<T> const &v)
{
  unsigned tmp;
  tmp = v.size(); f.write(stream_cast &tmp, sizeof(tmp));
  f.write(stream_cast v.data_block(), v.size() * sizeof(T));
  return true;
}

template <class T>
bool vnl_binary_load(istream &f, vnl_vector<T> &v)
{
  unsigned n;
  f.read(stream_cast &n, sizeof(n));
  vnl_resize(v, n);
  f.read(stream_cast v.data_block(), v.size() * sizeof(T));
  return true;
}

// -------------------- vnl_matrix

template <class T>
bool vnl_binary_save(ostream &f, vnl_matrix<T> const &A)
{
  unsigned tmp;
  tmp = A.rows(); f.write(stream_cast &tmp, sizeof(tmp));
  tmp = A.cols(); f.write(stream_cast &tmp, sizeof(tmp));
  f.write(stream_cast A.data_block(), A.size() * sizeof(T));
  return true;
}

template <class T>
bool vnl_binary_load(istream &f, vnl_matrix<T> &A)
{
  unsigned r, c;
  f.read(stream_cast &r, sizeof(r));
  f.read(stream_cast &c, sizeof(c));
  vnl_resize(A, r, c);
  f.read(stream_cast A.data_block(), A.size() * sizeof(T));
  return true;
}

// -------------------- vnl_diag_matrix

template <class T>
bool vnl_binary_save(ostream &f, vnl_diag_matrix<T> const &D)
{
  unsigned tmp;
  tmp = D.size(); f.write(stream_cast &tmp, sizeof(tmp));
  f.write(stream_cast D.data_block(), D.size() * sizeof(T));
  return true;
}

template <class T>
bool vnl_binary_load(istream &f, vnl_diag_matrix<T> &D)
{
  unsigned n;
  f.read(stream_cast &n, sizeof(n));
  vnl_resize(D, n);
  f.read(stream_cast D.data_block(), D.size() * sizeof(T));
  return true;
}

//------------------------------------------------------------

#define inst(T) \
template bool vnl_binary_save(ostream &, vnl_vector<T > const &); \
template bool vnl_binary_save(ostream &, vnl_matrix<T > const &); \
template bool vnl_binary_save(ostream &, vnl_diag_matrix<T > const &); \
template bool vnl_binary_load(istream &, vnl_vector<T > &); \
template bool vnl_binary_load(istream &, vnl_matrix<T > &); \
template bool vnl_binary_load(istream &, vnl_diag_matrix<T > &); \
;

inst(int);
inst(float);
inst(double);
