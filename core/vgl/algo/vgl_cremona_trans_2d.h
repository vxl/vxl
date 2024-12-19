// This is core/vgl/algo/vgl_cremona_trans_2d.h
#ifndef vgl_cremona_trans_2d_h_
#define vgl_cremona_trans_2d_h_
//:
// \file
// \brief A rational polynomial transformation of the plane
//
// \verbatim
//   23 Nov 2019 - J.L. Mundy
//  Modifications - None
// \endverbatim
//
// A cremona tranform is a generalization of the 2-d projective transformation
// where the map (X,Y) -> (x, y) in Euclidian coordinates is defined by ratios of polynomials.
//
//       P(X, Y)       R(X, Y)
//   x = -------   y = ------
//       Q(X, Y)       S(X, Y)
//
//   P, Q, R, S are polynomials of degree deg.
//
// In the case where the highest degree is one, the Cremona transformation with
// a common denominator is equivalent to the projective transformation.

#include <utility>

#include <vector>
#include <iosfwd>
#include <vnl/vnl_fwd.h> // for vnl_vector_fixed<T,2>
#include <vgl/vgl_homg_point_2d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include "vgl_norm_trans_2d.h"
//:
// A transformation of the plane with rational polynomials
// in X Y with degree deg
template <class T, size_t deg>
class vgl_cremona_trans_2d
{
public:
  //: default constructor
  vgl_cremona_trans_2d() = default;

  vgl_cremona_trans_2d(const vgl_norm_trans_2d<T> & tr_from,
                       const vgl_norm_trans_2d<T> & tr_to,
                       vnl_vector<T> rational_coeffs)
    : tr_from_(tr_from)
    , tr_to_(tr_to)
    , coeff_(std::move(rational_coeffs))
  {}

  //: set members of a default instance
  void
  set(const vgl_norm_trans_2d<T> & tr_from, const vgl_norm_trans_2d<T> & tr_to, const vnl_vector<T> & rational_coeffs)
  {
    tr_from_ = tr_from;
    tr_to_ = tr_to;
    coeff_ = rational_coeffs;
  }

  //: maps from -> to
  void
  project(T X, T Y, T & x, T & y) const;
  vgl_homg_point_2d<T>
  operator()(const vgl_homg_point_2d<T> & p) const;
  vgl_point_2d<T>
  operator()(const vgl_point_2d<T> & p) const;


  // utility functions (static to allow use in other classes)

  //: the number of coefficients in a polynomial in X,Y with degree deg
  static size_t
  n_coeff();

  //: the monomials in a polynomial in X, Y of degree deg
  static vnl_vector<T>
  power_vector(T x, T y);

  // Data Members--------------------------------------------------------------
  vgl_norm_trans_2d<T>
  tr_from() const
  {
    return tr_from_;
  }
  vgl_norm_trans_2d<T>
  tr_to() const
  {
    return tr_to_;
  }
  vnl_vector<T>
  coeff() const
  {
    return coeff_;
  }

protected:
  //: normalizing transformations to
  // enable accurate polynomial evaluation
  vgl_norm_trans_2d<T> tr_from_;
  vgl_norm_trans_2d<T> tr_to_;
  // rational coefficients
  // <--------- 4 x n_coeff -------->
  //[ x_neu | x_den | y_neu | y_den ]
  vnl_vector<T> coeff_;
};
//: stream operators
template <class T, size_t deg>
std::ostream &
operator<<(std::ostream & s, const vgl_cremona_trans_2d<T, deg> & t);
template <class T, size_t deg>
std::istream &
operator>>(std::istream & s, vgl_cremona_trans_2d<T, deg> & t);

#define VGL_CREMONA_TRANS_2D_INSTANTIATE(T) extern "please include vgl/algo/vgl_cremona_trans_2d.hxx first"

#endif // vgl_cremona_trans_2d_h_
