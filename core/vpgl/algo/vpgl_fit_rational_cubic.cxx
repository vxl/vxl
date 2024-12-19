#include <iostream>
#include <limits>
#include <math.h>
#include <sstream>
#include <utility>

#include <vnl/algo/vnl_svd.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include "vpgl_fit_rational_cubic.h"

vpgl_cubic_lsqr::vpgl_cubic_lsqr(const std::vector<vgl_point_2d<double>> & image_pts,
                                 std::vector<vgl_point_3d<double>> ground_pts)
  : vnl_least_squares_function(80, 2.0 * image_pts.size(), vnl_least_squares_function::no_gradient)
  , image_pts_(image_pts)
  , ground_pts_(std::move(ground_pts))
{
  rational_coeffs_.fill(0.0);
}

vnl_vector_fixed<double, 20>
vpgl_cubic_lsqr::power_vector(double x, double y, double z)
{
  // Form the monomials in homogeneous form
  double w = 1;
  double xx = x * x;
  double xy = x * y;
  double xz = x * z;
  double yy = y * y;
  double yz = y * z;
  double zz = z * z;
  double xxx = x * xx;
  double xxy = x * xy;
  double xxz = x * xz;
  double xyy = x * yy;
  double xyz = x * yz;
  double xzz = x * zz;
  double yyy = y * yy;
  double yyz = y * yz;
  double yzz = y * zz;
  double zzz = z * zz;
  double xww = x * w * w;
  double yww = y * w * w;
  double zww = z * w * w;
  double www = w * w * w;
  double xxw = xx * w;
  double xyw = xy * w;
  double xzw = xz * w;
  double yyw = yy * w;
  double yzw = yz * w;
  double zzw = zz * w;

  // fill the vector
  vnl_vector_fixed<double, 20> pv;
  pv.put(0, double(xxx));
  pv.put(1, double(xxy));
  pv.put(2, double(xxz));
  pv.put(3, double(xxw));
  pv.put(4, double(xyy));
  pv.put(5, double(xyz));
  pv.put(6, double(xyw));
  pv.put(7, double(xzz));
  pv.put(8, double(xzw));
  pv.put(9, double(xww));
  pv.put(10, double(yyy));
  pv.put(11, double(yyz));
  pv.put(12, double(yyw));
  pv.put(13, double(yzz));
  pv.put(14, double(yzw));
  pv.put(15, double(yww));
  pv.put(16, double(zzz));
  pv.put(17, double(zzw));
  pv.put(18, double(zww));
  pv.put(19, double(www));
  return pv;
}

void
vpgl_cubic_lsqr::project(const double x, const double y, const double z, double & u, double & v)
{
  vnl_vector_fixed<double, 4> polys = rational_coeffs_ * power_vector(x, y, z);
  u = polys[0] / polys[1];
  v = polys[2] / polys[3];
}

void
vpgl_cubic_lsqr::f(const vnl_vector<double> & coeffs, vnl_vector<double> & residuals)
{
  // unpack the cubic coefficients
  for (size_t j = 0; j < 4; ++j)
    for (size_t i = 0; i < 20; ++i)
    {
      size_t index = j * 20 + i;
      rational_coeffs_[j][i] = coeffs[index];
    }
  size_t n = image_pts_.size();
  for (size_t k = 0; k < n; ++k)
  {
    double u = 0.0, v = 0.0;
    const vgl_point_3d<double> & p = ground_pts_[k];
    const vgl_point_2d<double> & uv = image_pts_[k];
    project(p.x(), p.y(), p.z(), u, v);
    residuals[2 * k] = u - uv.x();
    residuals[2 * k + 1] = v - uv.y();
  }
}

bool
vpgl_fit_rational_cubic::compute_initial_guess()
{
  size_t n = image_pts_.size();
  vnl_matrix<double> A(2 * n, 80);
  A.fill(0.0);
  for (size_t k = 0; k < n; ++k)
  {
    const vgl_point_3d<double> & p = ground_pts_[k];
    const vgl_point_2d<double> & uv = image_pts_[k];
    vnl_vector_fixed<double, 20> pv = vpgl_cubic_lsqr::power_vector(p.x(), p.y(), p.z());
    double u = uv.x(), v = uv.y();
    // the n x 80 data matrix is formed by:
    //     20                20            20            20
    // [pv(x0,y0,z0)  -u*pv(x0,y0,z0)       0             0      ]
    // [      0               0      pv(x0,y0,z0) -v*pv(x0,y0,z0)]
    // [pv(x1,y1,z1)  -u*pv(x1,y1,z1)       0             0      ]
    // [      0               0      pv(x1,y1,z1) -v*pv(x1,y1,z1)]
    // [                       ...                               ]
    //
    for (size_t i = 0; i < 20; ++i)
    {
      A[2 * k][i] = pv[i];
      A[2 * k][i + 20] = -u * pv[i];
      A[2 * k + 1][i + 40] = pv[i];
      A[2 * k + 1][i + 60] = -v * pv[i];
    }
  }
  vnl_svd<double> svd(A);
  size_t r = svd.rank();
  if (r < 80)
  {
    std::cout << "insufficent rank " << r << " for linear solution of cubic coefficients" << std::endl;
    return false;
  }
  initial_guess_ = svd.nullvector();
  return true;
}

double
vpgl_fit_rational_cubic::initial_rms_error()
{
  // unpack the initial cubic coefficients
  vnl_matrix_fixed<double, 4, 20> rational_coeffs;
  for (size_t j = 0; j < 4; ++j)
    for (size_t i = 0; i < 20; ++i)
    {
      size_t index = j * 20 + i;
      rational_coeffs[j][i] = initial_guess_[index];
    }
  size_t n = image_pts_.size();
  double err_sq = 0.0;
  for (size_t k = 0; k < n; ++k)
  {
    const vgl_point_3d<double> & p = ground_pts_[k];
    const vgl_point_2d<double> & uv = image_pts_[k];
    vnl_vector_fixed<double, 20> pv = vpgl_cubic_lsqr::power_vector(p.x(), p.y(), p.z());
    vnl_vector_fixed<double, 4> polys = rational_coeffs * pv;
    double u = polys[0] / polys[1];
    double v = polys[2] / polys[3];
    double er = (u - uv.x()) * (u - uv.x()) + (v - uv.y()) * (v - uv.y());
    err_sq += er;
  }
  err_sq /= n;
  return sqrt(err_sq);
}

bool
vpgl_fit_rational_cubic::fit()
{
  if (verbose_)
    std::cout << "\n=====> Solve rational cubic polynomials <=====" << std::endl;

  vpgl_cubic_lsqr lsq(image_pts_, ground_pts_);
  vnl_levenberg_marquardt levmarq(lsq);
  // same as internal default settings
  double xtol = 1e-10;
  double maxfev = 400 * 80;     // Termination maximum number of iterations.
  double ftol = xtol * 0.01;    // Termination tolerance on F (sum of squared residuals)
  double gtol = 1e-5;           // Termination tolerance on Grad(F)' * F = 0
  double epsfcn = xtol * 0.001; // Step length for FD Jacobian
  levmarq.set_verbose(true);
  // Set the x-tolerance.  Minimization terminates when the length of the steps taken in X (variables) are less than
  // input x-tolerance
  levmarq.set_x_tolerance(xtol);
  // Set the epsilon-function.  This is the step length for FD Jacobian
  levmarq.set_epsilon_function(epsfcn);
  // Set the f-tolerance.  Minimization terminates when the successive RSM errors are less then this
  levmarq.set_f_tolerance(ftol);
  // Set the maximum number of iterations
  levmarq.set_max_function_evals(maxfev);
  // Minimize the error and get the best intersection point
  // for output
  vnl_vector<double> coeffs = initial_guess_;
  levmarq.minimize(coeffs);
  std::stringstream ss;
  levmarq.diagnose_outcome(ss);
  std::string outcome = ss.str();
  size_t pos = outcome.find_last_of("/");
  if (pos == std::string::npos)
  {
    std::cout << "unexpected Levenberg-Marquardt error message " << outcome << std::endl;
    return false;
  }
  std::stringstream ss2;
  size_t nout = outcome.size();
  for (size_t p = pos + 1; p < nout; ++p)
    ss2 << outcome[p];
  ss2 >> levmq_err_;
  if (levmq_err_ > max_err_)
  {
    std::cout << "maximum error exceeded in Levenberg-Marquardt " << levmq_err_ << " > " << max_err_ << std::endl;
    return false;
  }
  if (verbose_)
    std::cout << outcome << std::endl;

  vnl_vector<double> residuals(2 * image_pts_.size());
  lsq.f(coeffs, residuals); // final set of residuals and coeffs
  // output result
  double max_coeff = 0.0;
  for (size_t k = 0; k < 80; ++k)
  {
    double mag = fabs(coeffs[k]);
    if (mag > max_coeff)
      max_coeff = mag;
  }
  for (size_t j = 0; j < 4; ++j)
  {
    std::vector<double> temp(20, 0.0);
    for (size_t i = 0; i < 20; ++i)
      temp[i] = coeffs[20 * j + i] / max_coeff;
    rational_coefs_.push_back(temp);
  }
  return true;
}
