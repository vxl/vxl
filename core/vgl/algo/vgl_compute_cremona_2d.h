// This is core/vgl/algo/vgl_fit_cremona_2d.h
#ifndef vgl_compute_cremona_2d_h_
#define vgl_compute_cremona_2d_h_
//:
// \file
// \brief Fit a cremona transform in the plane given a set of corresponding points
//
// \verbatim
//  Modifications
//   Created November 22, 2019 - J.L. Mundy
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
//
// Various constraints on the generality of the transformation are available:
//
// BI_RATIONAL - full generality
// COMMON_DENOMINATOR - the maps to x and to y have the same bi-rational denominator
// UNITY_DENOMINATOR  - both denominators are 1 - so not a rational form (analogous to affine)
//
#include <iosfwd>
#include <vgl/algo/vgl_norm_trans_2d.h>
#include <vgl/vgl_homg_point_2d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>
#include "vgl_cremona_trans_2d.h"
template <class T, size_t deg>
class vgl_compute_cremona_2d
{
public:
  enum constraint_t
  {
    BI_RATIONAL,
    COMMON_DENOMINATOR,
    UNITY_DENOMINATOR,
    UNKNOWN
  };
  // Constructors/Initializers/Destructors-------------------------------------

  vgl_compute_cremona_2d()
    : constr_type_(BI_RATIONAL)
  {}
  ~vgl_compute_cremona_2d() = default;


  // Operations----------------------------------------------------------------

  bool
  compute_linear(const std::vector<vgl_homg_point_2d<T>> & from_pts,
                 const std::vector<vgl_homg_point_2d<T>> & to_pts,
                 constraint_t ctype = BI_RATIONAL);
  bool
  project_linear(T X, T Y, T & x, T & y) const;
  T
  linear_error()
  {
    return linear_error_;
  }

  //: the cremona transformation produced by the linear solution
  vgl_cremona_trans_2d<T, deg>
  linear_trans()
  {
    return vgl_cremona_trans_2d<T, deg>(tr_from_, tr_to_, linear_coeff_);
  }
  // internals
  vnl_vector<T>
  linear_coeff() const
  {
    return linear_coeff_;
  }
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

protected:
  bool
  normalize();
  bool
  compute_linear_solution_error();
  constraint_t constr_type_;
  std::vector<vgl_homg_point_2d<T>> from_pts_;
  std::vector<vgl_homg_point_2d<T>> to_pts_;
  vgl_norm_trans_2d<T> tr_from_;
  vgl_norm_trans_2d<T> tr_to_;
  std::vector<vgl_homg_point_2d<T>> norm_from_pts_;
  std::vector<vgl_homg_point_2d<T>> norm_to_pts_;
  T linear_error_;
  vnl_vector<T> linear_coeff_;
  bool linear_solved_{ false };
};

#define VGL_COMPUTE_CREMONA_2D_INSTANTIATE(T, deg) extern "please include vgl/algo/vgl_compute_cremona_2d.hxx first"

#endif // vgl_compute_cremona_2d_h_
