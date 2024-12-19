// This is core/vpgl/algo/vpgl_fit_rational_cubic.h
#ifndef vpgl_fit_rational_cubic_h_
#define vpgl_fit_rational_cubic_h_
//:
// \file
// \brief fit rational cubic polynomial to the projection from ground to image
// \author J. L. Mundy
// \date January 26, 2019
//
// \verbatim
// modifications - none
// \endverbatim
// The map from (x,y,z) to image coordinates, (u, v) is defined by:
//       P(x,y,z)        S(x,y,z)
//   u = -------  ,  v = -------- , (u,v) = f(x,y,z)
//       Q(x,y,z)        T(x,y,z)
// where P, Q, S, and T are cubic polynomials. A cubic polynomial in (x,y,z) has 20 coefficients.
// Thus the map is defined by 80 cubic coefficients.
// The algorithm finds a set of coefficients that models the projection of a set of 3-d pts into
// a corresponding set of image points. It is assumed that the 3-d points have been normalized
// so that x, y, z are within the range [-1, 1]. This class is designed to be used in conjuction
// with vpgl_camera_compute that provides the normalization step. The coefficient solution is
// normalized so that the largest coefficient has magnitude 1.
//
// The algorithm consists of two stages: 1) Linear least squares (SVD) to find an initial guess
// for the 80 coefficients; 2) Levenberg-Marquardt non-linear optimization to refine the coefficients.
// An error state occurs if the rank of the coefficient data matrix is less than 80 or the final
// projection rms error is greater than max_err_. RMS projection error is
//                      1
//   RMS error = sqrt( --- sum(||uv-f(x,y,z)||^2)  ).
//                      N
// A typical rms error is 10^-9 for points randomly generated from an original RPC model.
// (Note that there is no guarantee that the fitted cubic coefficients will be the same
// as the original RPC coefficients.)
//
#include <utility>

#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vnl/vnl_least_squares_function.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>

//
// the least squares function, f, takes the current estimate for the coefficients
// and computes the projection errors
//
class vpgl_cubic_lsqr : public vnl_least_squares_function
{
public:
  //: Constructor
  vpgl_cubic_lsqr(const std::vector<vgl_point_2d<double>> & image_pts, std::vector<vgl_point_3d<double>> ground_pts);

  //: The main function.
  //  Given the parameter vector x, compute the vector of residuals fx.
  //  fx has been sized appropriately before the call.
  void
  f(const vnl_vector<double> & coefs, vnl_vector<double> & residuals) override;
  static vnl_vector_fixed<double, 20>
  power_vector(double x, double y, double z);

private:
  vpgl_cubic_lsqr() = delete; // not valid
  void
  project(const double x, const double y, const double z, double & u, double & v);
  std::vector<vgl_point_2d<double>> image_pts_;
  std::vector<vgl_point_3d<double>> ground_pts_;
  vnl_matrix_fixed<double, 4, 20> rational_coeffs_;
};


class vpgl_fit_rational_cubic
{
public:
  vpgl_fit_rational_cubic(std::vector<vgl_point_2d<double>> image_pts, std::vector<vgl_point_3d<double>> ground_pts)
    : image_pts_(std::move(image_pts))
    , ground_pts_(std::move(ground_pts))
    , max_err_(1.0e-5)
    , verbose_(false)
  {
    initial_guess_.set_size(80);
    initial_guess_.fill(0.0);
    // a crude initial guess, avoids division by zero
    initial_guess_[19] = 1.0;
    initial_guess_[39] = 1.0;
    initial_guess_[59] = 1.0;
    initial_guess_[79] = 1.0;
  }

  void
  set_verbose(bool verbose)
  {
    verbose_ = verbose;
  }
  void
  set_max_error(double max_error)
  {
    max_err_ = max_error;
  }

  //: the user defines the initial coefficient values
  void
  set_initial_guess(vnl_vector<double> initial_guess)
  {
    initial_guess_ = initial_guess;
  }

  //: the initial coefficients are computed using linear least squares
  bool
  compute_initial_guess();

  //: result of linear least squares
  double
  initial_rms_error();

  //: error after non-linear optimization
  double
  final_rms_error() const
  {
    return levmq_err_;
  }

  //: the main process method
  bool
  fit();

  //: return the fitted coefficients
  std::vector<std::vector<double>>
  rational_coeffs()
  {
    return rational_coefs_;
  }

private:
  vpgl_fit_rational_cubic() = delete;
  bool verbose_;
  double max_err_;
  double levmq_err_;
  std::vector<vgl_point_2d<double>> image_pts_;
  std::vector<vgl_point_3d<double>> ground_pts_;
  std::vector<std::vector<double>> rational_coefs_;
  vnl_vector<double> initial_guess_;
};


#endif // vpgl_fit_rational_cubic_h_
