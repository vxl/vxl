// This is ./vxl/vnl/vnl_complex_ops.txx
#ifndef vnl_complex_ops_txx_
#define vnl_complex_ops_txx_

//:
// \file

/*
  fsm@robots.ox.ac.uk
*/
#include "vnl_complex_ops.h"

#include <vcl_cassert.h>
#include <vcl_cmath.h> // for vcl_atan2()

//-----------------------------------------------------------------------

template <class T>
void vnl_complexify(T const *src, vcl_complex<T> *dst, unsigned n) {
  for (unsigned i=0; i<n; ++i)
    dst[i] = src[i];
}

template <class T>
void vnl_complexify(T const *re, T const *im, vcl_complex<T> *dst, unsigned n) {
  for (unsigned i=0; i<n; ++i)
    dst[i] = vcl_complex<T>(re[i], im[i]);
}

template <class T>
vnl_vector<vcl_complex<T> > vnl_complexify(vnl_vector<T> const &v) {
  vnl_vector<vcl_complex<T> > vc(v.size());
  vnl_complexify(v.begin(), vc.begin(), v.size());
  return vc;
}

template <class T>
vnl_vector<vcl_complex<T> > vnl_complexify(vnl_vector<T> const &re, vnl_vector<T> const &im) {
  assert(re.size() == im.size());
  vnl_vector<vcl_complex<T> > vc(re.size());
  vnl_complexify(re.begin(), im.begin(), vc.begin(), re.size());
  return vc;
}

template <class T>
vnl_matrix<vcl_complex<T> > vnl_complexify(vnl_matrix<T> const &M) {
  vnl_matrix<vcl_complex<T> > Mc(M.rows(), M.cols());
  vnl_complexify(M.begin(), Mc.begin(), M.size());
  return Mc;
}

template <class T>
vnl_matrix<vcl_complex<T> > vnl_complexify(vnl_matrix<T> const &re, vnl_matrix<T> const &im) {
  assert(re.rows() == im.rows());
  assert(re.cols() == im.cols());
  vnl_matrix<vcl_complex<T> > Mc(re.rows(), re.cols());
  vnl_complexify(re.begin(), im.begin(), Mc.begin(), re.size());
  return Mc;
}

#if 0 // fsm
//: Vector of absolute values of vnl_complex_vector_t<REAL>.
template <class T>
vnl_vector<T> vnl_abs(vnl_vector<vcl_complex<T> > const & C)
{
  vnl_vector<T> ret(C.size());
  for (unsigned i = 0; i < C.size(); ++i)
    ret[i] = vcl_abs(C[i]);
  return ret;
}


//: Vector of angles of vnl_vector<vcl_complex<T> >.
// vcl_atan2(vcl_imag(C), vcl_real(C))
template <class T>
vnl_vector<T> vnl_angle(vnl_vector<vcl_complex<T> > const & C)
{
  vnl_vector<T> ret(C.size());
  for (unsigned i = 0; i < C.size(); ++i)
    ret[i] = vcl_atan2(vcl_imag(C[i]), vcl_real(C[i]));
  return ret;
}
#endif

//: Vector of real parts of vnl_vector<vcl_complex<T> >.
template <class T>
vnl_vector<T> vnl_real(vnl_vector<vcl_complex<T> > const & C)
{
  vnl_vector<T> ret(C.size());
  for (unsigned i = 0; i < C.size(); ++i)
    ret[i] = vcl_real(C[i]);
  return ret;
}

//: Vector of imaginary parts of vnl_vector<vcl_complex<T> >.
template <class T>
vnl_vector<T> vnl_imag(vnl_vector<vcl_complex<T> > const & C)
{
  vnl_vector<T> ret(C.size());
  for (unsigned i = 0; i < C.size(); ++i)
    ret[i] = vcl_imag(C[i]);
  return ret;
}

//----------------------------------------------------------------------

#if 0 // fsm
//: Matrix of absolute values of vnl_complex_matrix_t<REAL>.
template <class T>
vnl_matrix<T> vnl_abs(vnl_matrix<vcl_complex<T> > const& C)
{
  vnl_matrix<T> ret(C.rows(), C.columns());
  for(unsigned i = 0; i < C.rows(); ++i)
    for(unsigned j = 0; j < C.columns(); ++j)
      ret(i,j) = vcl_abs(C(i,j));
  return ret;
}


//: Matrix of angles of vnl_complex_matrix_t<T>.
// vcl_atan2(vcl_imag(C), vcl_real(C))
template <class T>
vnl_matrix<T> vnl_angle(vnl_matrix<vcl_complex<T> > const& C)
{
  vnl_matrix<T> ret(C.rows(), C.columns());
  for(unsigned i = 0; i < C.rows(); ++i)
    for(unsigned j = 0; j < C.columns(); ++j)
      ret(i,j) = vcl_atan2(vcl_imag(C(i,j)), vcl_real(C(i,j)));
  return ret;
}
#endif

//: Matrix of real parts of vnl_complex_matrix_t<T>.
template <class T>
vnl_matrix<T> vnl_real(vnl_matrix<vcl_complex<T> > const& C)
{
  vnl_matrix<T> ret(C.rows(), C.columns());
  for(unsigned i = 0; i < C.rows(); ++i)
    for(unsigned j = 0; j < C.columns(); ++j)
      ret(i,j) = vcl_real(C(i,j));
  return ret;
}

//: Matrix of imaginary parts of vnl_complex_matrix_t<T>.
template <class T>
vnl_matrix<T> vnl_imag(vnl_matrix<vcl_complex<T> > const& C)
{
  vnl_matrix<T> ret(C.rows(), C.columns());
  for(unsigned i = 0; i < C.rows(); ++i)
    for(unsigned j = 0; j < C.columns(); ++j)
      ret(i,j) = vcl_imag(C(i,j));
  return ret;
}

//-------------------------------------------------------------------------

#define VNL_COMPLEX_OPS_INSTANTIATE(T) \
template void vnl_complexify(T const *, vcl_complex<T > *, unsigned); \
template void vnl_complexify(T const *, T const *, vcl_complex<T > *, unsigned); \
\
template vnl_vector<vcl_complex<T > > vnl_complexify(vnl_vector<T > const &); \
template vnl_vector<vcl_complex<T > > vnl_complexify(vnl_vector<T > const &, vnl_vector<T > const &); \
template vnl_matrix<vcl_complex<T > > vnl_complexify(vnl_matrix<T > const &); \
template vnl_matrix<vcl_complex<T > > vnl_complexify(vnl_matrix<T > const &, vnl_matrix<T > const &); \
\
/* template vnl_vector<T > vnl_angle(vnl_vector<vcl_complex<T > > const &); */ \
/* template vnl_vector<T > vnl_abs  (vnl_vector<vcl_complex<T > > const &); */ \
template vnl_vector<T > vnl_real (vnl_vector<vcl_complex<T > > const &); \
template vnl_vector<T > vnl_imag (vnl_vector<vcl_complex<T > > const &); \
\
/* template vnl_matrix<T > vnl_angle(vnl_matrix<vcl_complex<T > > const &); */ \
/* template vnl_matrix<T > vnl_abs  (vnl_matrix<vcl_complex<T > > const &); */ \
template vnl_matrix<T > vnl_real (vnl_matrix<vcl_complex<T > > const &); \
template vnl_matrix<T > vnl_imag (vnl_matrix<vcl_complex<T > > const &)

#endif // vnl_complex_ops_txx_
