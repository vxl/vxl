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
// \verbatim
// April 08, 2003 - J.L. Mundy, (a port of fsm's osl/osl_ortho_regress class)
//
// Modifications - none
// 
// \endverbatim
#include <vcl_vector.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_vector_fixed.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_line_2d.h>

// a class to hold the line 2d_regression data and actual fitting code.
template <class T>
class vgl_line_2d_regression
{
 private:
  int npts_;
  vgl_line_2d<T> line_;//the fitted line
  T Sx_, Sy_, Sxx_, Sxy_, Syy_;//partial sums
  double squared_error_;//an estimate of the squared error
 public:
  vgl_line_2d_regression();
  ~vgl_line_2d_regression(){};

  inline int get_n_pts(){return npts_;}

  //: Add a point to the 2d_regression
  void increment_partial_sums(const T x, const T y);

  //: Remove a point from the 2d_regression
  void decrement_partial_sums(const T x, const T y);

  //: Clear 2d_regression sums
  void clear();

  //: get fitting error for a given line
  double get_rms_error(const T a, const T b, const T c);

  //: get fitting error for current fitted line
  double get_rms_error();

  //: initialize estimated fitting error
  void init_rms_error_est();

  //: get estimated fitting error if the point (x, y) were added to the fit
  double get_rms_error_est(vgl_point_2d<T> const& p, bool increment=true);

  //: get the fitted line
  vgl_line_2d<T> get_line(){return line_;}

  //: fit a line to the current point set
  bool fit();

  //: fit a line to the current point set
  bool fit_constrained(T x, T y);
};  

#define VGL_LINE_2D_REGRESSION_INSTANTIATE(T) extern "please include vgl/algo/vgl_line_2d_regression.txx first"

#endif // vgl_line_2d_regression_h_
