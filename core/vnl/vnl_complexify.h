// This is core/vnl/vnl_complexify.h
#ifndef vnl_complexify_h_
#define vnl_complexify_h_
//:
//  \file
//  \brief Functions to create complex vectors and matrices from real ones
//  \author fsm
//
// \verbatim
// Modifications
// Peter Vanroose - 2 July 2002 - part of vnl_complex_ops.h moved here
// \endverbatim

#include <vcl_complex.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_diag_matrix.h>
#include <vnl/vnl_diag_matrix_fixed.h>
#include <vnl/vnl_sym_matrix.h>

//: Overwrite complex array C (of length n) with pairs from real arrays R and I.
template <class T>
void
  vnl_complexify(T const* R, T const* I, vcl_complex<T>* C, unsigned n);
//: Overwrite complex array C (sz n) with complexified version of real array R.
template <class T>
void
  vnl_complexify(T const* R,             vcl_complex<T>* C, unsigned n);

//: Return complexified version of real vector R.
// \relatesalso vnl_vector
template <class T>
vnl_vector<vcl_complex<T> >
  vnl_complexify(vnl_vector<T> const& R);

//: Return complexified version of real fixed vector R.
// \relatesalso vnl_vector_fixed
template <class T, unsigned int n>
vnl_vector_fixed<vcl_complex<T>,n>
  vnl_complexify(vnl_vector_fixed<T,n> const& v)
{
  vnl_vector_fixed<vcl_complex<T>,n> vc;
  vnl_complexify(v.begin(), vc.begin(), v.size());
  return vc;
}

//: Return complex vector R+j*I from two real vectors R and I.
// \relatesalso vnl_vector
template <class T>
vnl_vector<vcl_complex<T> >
  vnl_complexify(vnl_vector<T> const& R, vnl_vector<T> const& I);

//: Return complexified version of real matrix R.
// \relatesalso vnl_matrix
template <class T>
vnl_matrix<vcl_complex<T> >
  vnl_complexify(vnl_matrix<T> const& R);

//: Return complexified version of real fixed matrix R.
// \relatesalso vnl_matrix_fixed
template <class T, unsigned int r, unsigned int c>
vnl_matrix_fixed<vcl_complex<T>,r,c >
  vnl_complexify(vnl_matrix_fixed<T,r,c> const& M)
{
  vnl_matrix_fixed<vcl_complex<T>,r,c> Mc;
  vnl_complexify(M.begin(), Mc.begin(), M.size());
  return Mc;
}

//: Return complexified version of real diagonal matrix R.
// \relatesalso vnl_diag_matrix
template <class T>
vnl_diag_matrix<vcl_complex<T> >
  vnl_complexify(vnl_diag_matrix<T> const& M)
{
  vnl_diag_matrix<vcl_complex<T> > Mc(M.rows(), M.cols());
  vnl_complexify(M.begin(), Mc.begin(), M.size());
  return Mc;
}

//: Return complexified version of real fixed diagonal matrix R.
// \relatesalso vnl_diag_matrix_fixed
template <class T, unsigned int n>
vnl_diag_matrix_fixed<vcl_complex<T>,n >
  vnl_complexify(vnl_diag_matrix_fixed<T,n> const& M)
{
  vnl_diag_matrix_fixed<vcl_complex<T>,n> Mc;
  vnl_complexify(M.begin(), Mc.begin(), M.size());
  return Mc;
}

//: Return complexified version of real symmetric matrix R.
// \relatesalso vnl_sym_matrix
template <class T>
vnl_sym_matrix<vcl_complex<T> >
  vnl_complexify(vnl_sym_matrix<T> const& M)
{
  vnl_sym_matrix<vcl_complex<T> > Mc(M.size());
  vnl_complexify(M.begin(), Mc.begin(), M.size());
  return Mc;
}

//: Return complex matrix R+j*I from two real matrices R and I.
// \relatesalso vnl_matrix
template <class T>
vnl_matrix<vcl_complex<T> >
  vnl_complexify(vnl_matrix<T> const& R, vnl_matrix<T> const& I);

#endif // vnl_complexify_h_
