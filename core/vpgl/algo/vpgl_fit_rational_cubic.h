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

#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vnl/vnl_least_squares_function.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vpgl/vpgl_rational_camera.h>

class vpgl_cubic_lsqr : public vnl_least_squares_function
{
 public:
  //: Constructor
  vpgl_cubic_lsqr(std::vector<vgl_point_2d<double> > const& image_pts,
                  std::vector<vgl_point_3d<double> > const& ground_pts);

  //: The main function.
  //  Given the parameter vector x, compute the vector of residuals fx.
  //  fx has been sized appropriately before the call.
  virtual void f(vnl_vector<double> const& coefs,
                 vnl_vector<double>& residuals);
  static vnl_vector_fixed<double, 20> power_vector(double x, double y, double z);

 private:
  vpgl_cubic_lsqr();//not valid
  void project(const double x, const double y, const double z, double& u, double& v);
  std::vector<vgl_point_2d<double> > image_pts_; //image points
  std::vector<vgl_point_3d<double> > ground_pts_; //image points
  vnl_matrix_fixed<double, 4, 20> rational_coeffs_;
};


class vpgl_fit_rational_cubic
{
 public:
  vpgl_fit_rational_cubic(std::vector<vgl_point_2d<double> > image_pts,
                          std::vector<vgl_point_3d<double> > ground_pts):
  image_pts_(image_pts), ground_pts_(ground_pts), max_err_(1.0e-5), verbose_(false){
	 initial_guess_.set_size(80);
    initial_guess_.fill(0.0); initial_guess_[19] = 1.0;
    initial_guess_[39] = 1.0;initial_guess_[59] = 1.0; initial_guess_[79] = 1.0;
  }
  void set_max_error(double max_error) { max_err_ = max_error; }
  //: the user defines the initial coefficient values
  void set_initial_guess(vnl_vector<double> initial_guess){initial_guess_ = initial_guess;}
  //: the initial coefficients are computed using linear least squares
  void set_verbose(bool verbose){verbose_ = verbose;}
  bool compute_initial_guess();
  double initial_rms_error();
  double final_rms_error(){return levmq_err_;}
  bool fit();
  std::vector<std::vector<double> > rational_coeffs(){return rational_coefs_;}

 private:
  vpgl_fit_rational_cubic();
  bool verbose_;
  double max_err_;
  double levmq_err_;
  std::vector<vgl_point_2d<double> > image_pts_;
  std::vector<vgl_point_3d<double> > ground_pts_;
  std::vector<std::vector<double> > rational_coefs_;
  vnl_vector<double> initial_guess_;
};


#endif // vpgl_fit_rational_cubic_h_
