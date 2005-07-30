// This is core/vgl/algo/vgl_conic_2d_regression.h
#ifndef vgl_conic_2d_regression_h_
#define vgl_conic_2d_regression_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Fits a conic to a set of points using linear regression
// \author J.L. Mundy
// \date June 17, 2005
//
//  Since conic fitting is rather ill-conditioned it is necessary to
//  normalize the point coordinates so that they have equal standard
//  deviations with respect to the center (mean) of the pointset.
//  The points are transformed to have zero mean. The resulting conic
//  is transformed back to the original frame for computing
//  the Sampson approximation to fitting error.
//
//  The regression uses the Bookstein algorithm which constrains the
//  conic norm, $ |ax^2 + bxy + cy^2 + dx + ey +f|^2 $, so that
//   $ a^2 + 0.5*b^2 + c^2 = 2 $
//  With this normalization, the resulting fit is invariant up to a similarity
//  transform of the pointset.  The solution is formulated as an eigenvalue
//  problem as follows:
//
//      The scatter matrix S is decomposed as
//              
//      $ S   =  \begin{array}{cc}     S_{11} & S_{12}     \\
//                                     S_{21} & S_{22}     \end{array}$
//      note $S_{21} = S_{12}^t$
//
//      The conic coefficients are $v_1 = \{a,b,c\}^t$ and $v_2 = \{d,e,f\}^t$
//      The Bookstein constraint is expressed by the diagonal matrix
//               D = Diag{1, 0.5, 1, 0, 0, 0}
//      The Lagrangian to be minimized is
//      $ L =  v_1^t S_{11} v_1  + 2 v_2^t S_{21} v_1  + v_2^t S_{22} v_2
//             - \lambda(v_1^t D v_1 -2) $.
//
//      Minimizing with respect to v2 gives
//      $ dL/dv_2 = 2 S_{21} v_1 + 2 S_{22} v_2 = 0 $
//
//      So,  $ v_2 = -S_{22}^-1 S_{21} v_1 $.
//
//      Substituting for v2 in L,
//
// $ L = v_1^t ( S_{11} - S_{12} * S_{22}^-1 * S_{21}) v_1 - \lambda(v_1^t D v_1 -2)
//     = v_1^t ( (S_{11} - S_{12} * S_{22}^-1 * S_{21}) - \lambda D ) v_1 - 2 \lambda $,
//
// $ dL/dv_1 =  ( (S_{11} - S_{12} * S_{22}^-1 * S_{21}) - \lambda D ) v_1 = 0 $.
//
//   So, $ \lambda v_1 = D^-1 (S_{11} - S_{12} * S_{22}^-1 * S_{21}) v_1 $.
//
//   This eigenvalue problem is solved using singular value decomposition.
//
// \verbatim
//  Modifications
//   none
// \endverbatim

#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_double_3x3.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_conic.h>
#include <vgl/algo/vgl_norm_trans_2d.h>

template <class T>
class vgl_conic_2d_regression
{
 public:

  // Constructors/Initializers/Destructors-------------------------------------

  vgl_conic_2d_regression();

  ~vgl_conic_2d_regression(){}

  // Operations----------------------------------------------------------------


  //: Number of regression points
  unsigned get_n_pts() const {return npts_;}

  //: clear the regression data
  void clear_points();

  void add_point(vgl_point_2d<T> const& p);

  void remove_point(vgl_point_2d<T> const& p);

  //: get the estimated Euclidean error with respect to the fitted conic segment in the original frame
  T get_rms_error_est(vgl_point_2d<T> const& p) const;

  //: get the current Euclidean fitting error in the original frame
  T get_rms_sampson_error() const { return sampson_error_; } //temporarily

  //: get the current algebraic fitting error in the normalized frame
  T get_rms_algebraic_error() const;

  //: the fitting method
  bool fit();

  // Data Access---------------------------------------------------------------

  vgl_conic<T> conic() const { return conic_; }

  // Debug support
  void print_pointset(vcl_ostream& str = vcl_cout );
 protected:
  // Internals
  void init();
  void compute_partial_sums();
  void fill_scatter_matrix();
  void set_sampson_error(const double a, const double b,
                         const double c, const double d,
                         const double e, const double f);

  // Data Members--------------------------------------------------------------

  //: The current set of points
  vcl_vector<vgl_point_2d<T> > points_;

  //: Size of point set
  unsigned npts_;

  //: The normalizing transformation
  vgl_norm_trans_2d<double> trans_;

  //: The partial scatter term sums, updated with each ::add_point
  vcl_vector<double> partial_sums_;

  //: The fitting matrices
  vnl_double_3x3 S11_, S12_, S22_;
  vnl_double_3x3 Dinv_;

  //: The fitted conic in the un-normalized frame
  vgl_conic<T> conic_;

  //: The algebraic fitting cost in the normalized frame
  T cost_;

  //: The Sampson approximation to Euclidean distance in the normalized frame
  T sampson_error_;

  //: Normalized points
  vcl_vector<vgl_homg_point_2d<double> > hnorm_points_;
};

#define VGL_CONIC_2D_REGRESSION_INSTANTIATE(T) extern "please include vgl/algo/vgl_conic_2d_regression.txx first"

#endif // vgl_conic_2d_regression_h_
