// This is core/vgl/algo/vgl_line_2d_regression.h
#ifndef vgl_line_2d_regression_h_
#define vgl_line_2d_regression_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief A helper class for line fitting. Holds the 2d_regression data
//
// \author
//   April 08, 2003 - J.L. Mundy, (a port of fsm's osl/osl_ortho_regress class)
//
// \verbatim
//  Modifications
//   12 Aug 2003 - Amitha Perera. Add some more documentation
// \endverbatim

#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_line_2d.h>

//: A class to hold the line 2d_regression data and actual fitting code.
//
// In addition to fitting a line to a set of points (orthogonal
// regression), it is designed to help with incremental fitting. You
// can inexpensively add and remove points. This class does not store
// the points; it merely stores enough aggregate information to
// estimate the line parameters.
//
template <class T>
class vgl_line_2d_regression
{
  // Data members
  unsigned int npts_;//!< number of points in the regression data
  vgl_line_2d<T> line_;//!< the fitted line
  T Sx_, Sy_, Sxx_, Sxy_, Syy_;//!< partial sums
  double squared_error_;//!< an estimate of the squared error
 public:
  vgl_line_2d_regression();
  ~vgl_line_2d_regression(){};

  //: The number of points added.
  inline unsigned int get_n_pts() { return npts_; }

  //: Add a point to the 2d_regression
  void increment_partial_sums(const T x, const T y);

  //: Remove a point from the 2d_regression
  //
  //  This should be a previously added point, although this cannot be
  //  verified.
  void decrement_partial_sums(const T x, const T y);

  //: Clear 2d_regression sums
  //
  //  This will reset the object to the freshly constructed state of having
  //  zero points.
  void clear();

  //: Get fitting error for a given line
  double get_rms_error(const T a, const T b, const T c);

  //: Get fitting error for current fitted line
  double get_rms_error();

  //: Initialize estimated fitting error
  void init_rms_error_est();

  //: Get estimated fitting error if the point (x, y) were added to the fit
  //
  //  You must call init_rms_error_est() to initialize the running
  //  totals before the first use of this function.
  //
  //  If \a increment is true, the running totals are updated as if
  //  the point \a p was added to the point set. It does not update
  //  the point set, however, so the point will not affect subsequent
  //  line estimation.
  //
  double get_rms_error_est(vgl_point_2d<T> const& p, bool increment=true);

  //: Get the fitted line
  vgl_line_2d<T> get_line() { return line_; }

  //: Fit a line to the current point set
  bool fit();

  //: Fit a line to the current point set constrained to pass through (x,y).
  bool fit_constrained(T x, T y);
};

#define VGL_LINE_2D_REGRESSION_INSTANTIATE(T) extern "please include vgl/algo/vgl_line_2d_regression.txx first"

#endif // vgl_line_2d_regression_h_
