// This is ./vxl/vnl/vnl_binary.h
#ifndef vnl_binary_h_
#define vnl_binary_h_
#ifdef __GNUC__
#pragma interface
#endif

//: \file
//  \brief Functions for saving and loading matrices and vectors in binary format.
//  \author fsm@robots.ox.ac.uk
//  \deprecated Use vsl instead
//
// The point of doing that is to 1) make it
// go faster and 2) ensure that no accuracy is lost in the
// process of saving to disk. It doesn't matter that the
// format is architecture dependent.
//
// \verbatim
// Modifications
//  LSB (Manchester) 23/3/01
//  Peter Vanroose - 27 June 2001 - deprecated in favour of vsl
// \endverbatim

#include <vcl_deprecated_header.h>
#include <vcl_iosfwd.h>
template <class T> class vnl_vector;
template <class T> class vnl_matrix;
template <class T> class vnl_diag_matrix;

template <class T> bool vnl_binary_save(vcl_ostream &, vnl_vector<T> const &);
template <class T> bool vnl_binary_save(vcl_ostream &, vnl_matrix<T> const &);
template <class T> bool vnl_binary_save(vcl_ostream &, vnl_diag_matrix<T> const &);

template <class T> bool vnl_binary_load(vcl_istream &, vnl_vector<T> &);
template <class T> bool vnl_binary_load(vcl_istream &, vnl_matrix<T> &);
template <class T> bool vnl_binary_load(vcl_istream &, vnl_diag_matrix<T> &);

#endif // vnl_binary_h_
