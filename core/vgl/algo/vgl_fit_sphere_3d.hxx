// This is core/vgl/algo/vgl_fit_sphere_3d.hxx
#ifndef vgl_fit_sphere_3d_hxx_
#define vgl_fit_sphere_3d_hxx_
//:
// \file

#include <iostream>
#include "vgl_fit_sphere_3d.h"
#include <vgl/algo/vgl_norm_trans_3d.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_matrix.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_distance.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <vnl/vnl_least_squares_function.h>
class sphere_residual_function : public vnl_least_squares_function{
 public:
 sphere_residual_function(std::vector<vgl_homg_point_3d<double> >  const& pts ):
  vnl_least_squares_function(4, static_cast<unsigned>(pts.size()), vnl_least_squares_function::use_gradient), pts_(pts){}
  //
  // x = [x0, y0, z0, r]^t
  // f = [(r0-r), (r1-r), ... , (ri-r), ... , (rn-1-r)]^t
  // where ri = sqrt( (xi-x0)^2 + (yi-y0)^2 + (zi-z0)^2 )
  //
  void f(vnl_vector<double> const& x, vnl_vector<double>& fx) override{
  double x0 = x[0], y0 = x[1], z0 = x[2], r = x[3];
  unsigned n = get_number_of_residuals();
  for(unsigned i = 0; i<n; ++i){
    vgl_point_3d<double> p(pts_[i]);
    double xi = (p.x()-x0), yi = (p.y()-y0), zi = (p.z()-z0);
    double ri = std::sqrt(xi*xi + yi*yi + zi*zi);
    fx[i] = (ri-r);
  }
  //std::cout << "f\n" << fx << '\n';
  }
//
// x = [x0, y0, z0, r]^t
// ri = sqrt( (xi-x0)^2 + (yi-y0)^2 + (zi-z0)^2 )
// J[i] = [-(xi-x0)/ri, -(yi-y0)/ri, -(zi-z0)/ri, -1]
//
  void gradf(vnl_vector<double> const& x, vnl_matrix<double>& J) override{
  double x0 = x[0], y0 = x[1], z0 = x[2];
  unsigned n = get_number_of_residuals();
  for (unsigned i = 0; i<n; ++i)
    {
    vgl_point_3d<double> p(pts_[i]);
    double xi = (p.x()-x0), yi = (p.y()-y0), zi = (p.z()-z0);
    double ri = std::sqrt(xi*xi + yi*yi + zi*zi);
    J[i][0]= -(xi-x0)/ri; J[i][1]= -(yi-y0)/ri; J[i][2]= -(zi-z0)/ri; J[i][3]= -1.0;
    }
  //std::cout << "J\n" << J << '\n';
  }
 private:
  std::vector<vgl_homg_point_3d<double> > pts_;
};

template <class T>
vgl_fit_sphere_3d<T>::vgl_fit_sphere_3d(std::vector<vgl_point_3d<T> > points)

{
  for(typename std::vector<vgl_point_3d<T> >::iterator pit = points.begin();
      pit != points.end(); ++pit)
    points_.push_back(vgl_homg_point_3d<T>(*pit));
}

template <class T>
void vgl_fit_sphere_3d<T>::add_point(vgl_point_3d<T> const &p)
{
  points_.push_back(vgl_homg_point_3d<T>(p));
}

template <class T>
void vgl_fit_sphere_3d<T>::add_point(const T x, const T y, const T z)
{
  points_.push_back(vgl_homg_point_3d<T>(x, y, z));
}

 template <class T>
 void vgl_fit_sphere_3d<T>::clear()
{
  points_.clear();
}

template <class T>
T vgl_fit_sphere_3d<T>::fit_linear(std::ostream* errstream)
{
  const unsigned n = static_cast<unsigned>(points_.size());
  if(!n){
    if (errstream)
      *errstream << "No points to fit sphere\n";
    return T(-1);
  }
    // normalize the points
  vgl_norm_trans_3d<T> norm;
  if (!norm.compute_from_points(points_) && errstream) {
    *errstream << "there is a problem with norm transform\n";
    return T(-1);
  }
  vnl_matrix<T> A(n,4), B(n,1);
  for (unsigned i=0; i<n; i++) {
    vgl_homg_point_3d<T> hp = norm(points_[i]);//normalize
    const T x = hp.x()/hp.w();
    const T y = hp.y()/hp.w();
    const T z = hp.z()/hp.w();
    A[i][0]= -T(2)*x; A[i][1]= -T(2)*y; A[i][2]= -T(2)*z; A[i][3]=T(1);
    B[i][0] = -(x*x + y*y + z*z);
  }
  vnl_svd<T> svd(A);
  vnl_matrix<T> P = svd.solve(B);
  T x0 = P[0][0], y0 = P[1][0], z0 = P[2][0];
  T rho = P[3][0];
  T r2 =  x0*x0 + y0*y0 + z0*z0 - rho;
  if(r2<T(0)){
    if(errstream)
      *errstream << "Negative squared radius - impossible result \n";
    return T(-1);
  }
  T r = std::sqrt(r2);

  vnl_matrix_fixed<T,4,4> H = norm.get_matrix();
  T scale = H[0][0];
  T tx = H[0][3];  T ty= H[1][3]; T tz= H[2][3];

  // scale back to original coordinates
  T x0p = (x0-tx)/scale,  y0p = (y0-ty)/scale,  z0p = (z0-tz)/scale, rp = r/scale;

  // construct the sphere
  sphere_lin_.set_radius(rp);
  sphere_lin_.set_centre(vgl_point_3d<T>(x0p, y0p, z0p));
  // compute average distance error
  double dsum = 0.0;
  for (unsigned i=0; i<n; i++) {
    vgl_point_3d<T> p(points_[i]);
    double d = vgl_distance(p, sphere_lin_);
    dsum += d;
  }
  return static_cast<T>(dsum/n);
}
template <class T>
T vgl_fit_sphere_3d<T>::fit(std::ostream* outstream, bool verbose){

  T error = this->fit_linear(outstream);
  T lin_radius = sphere_lin_.radius();

  if(error == T(-1) || error> T(0.1)*lin_radius){
    if(outstream)
      *outstream << " Linear fit failed - non-linear fit abandoned\n";
    return T(-1);
  }
  unsigned n = static_cast<unsigned>(points_.size());
    // normalize the points
  vgl_norm_trans_3d<T> norm;
  if (!norm.compute_from_points(points_) && outstream) {
    *outstream << "there is a problem with norm transform\n";
    return T(-1);
  }
  vnl_matrix_fixed<T,4,4> H = norm.get_matrix();
  T scale = H[0][0];
  T tx = H[0][3];  T ty= H[1][3]; T tz= H[2][3];
  // normalize linear fit sphere
  lin_radius *= scale;
  // scale center
  vgl_point_3d<T> c = sphere_lin_.centre();
  T lin_x0 = scale*c.x()+tx,  lin_y0 = scale*c.y()+ty,  lin_z0 = scale*c.z()+tz;

  std::vector<vgl_homg_point_3d<double> > pts;
  for(unsigned i = 0; i<n; ++i){
    vgl_homg_point_3d<T> hp = norm(points_[i]);//normalize
    vgl_homg_point_3d<double> hpd(static_cast<double>(hp.x()),
                                  static_cast<double>(hp.y()),
                                  static_cast<double>(hp.z()),
                                  static_cast<double>(hp.w()));
    pts.push_back(hpd);
  }
  sphere_residual_function srf(pts);
  vnl_levenberg_marquardt lm(srf);

  vnl_vector<double> x_init(4);
  x_init[0]=static_cast<double>(lin_x0);x_init[1]=static_cast<double>(lin_y0);
  x_init[2]=static_cast<double>(lin_z0);x_init[3]=static_cast<double>(lin_radius);

  lm.minimize(x_init);
  if(outstream && verbose)
    lm.diagnose_outcome(*outstream);

  vnl_nonlinear_minimizer::ReturnCodes code = lm.get_failure_code();
  if((code==vnl_nonlinear_minimizer::CONVERGED_FTOL||
       code==vnl_nonlinear_minimizer::CONVERGED_XTOL||
       code==vnl_nonlinear_minimizer::CONVERGED_XFTOL||
       code==vnl_nonlinear_minimizer::CONVERGED_GTOL)){
    T x0 = static_cast<T>(x_init[0]), y0 = static_cast<T>(x_init[1]), z0 = static_cast<T>(x_init[2]);
    T nr = static_cast<T>(x_init[3]);
    // scale back to original coordinates
    T x0p = (x0-tx)/scale,  y0p = (y0-ty)/scale,  z0p = (z0-tz)/scale, rp = nr/scale;

    vgl_point_3d<T> cf(x0p, y0p, z0p);
    sphere_non_lin_.set_centre(cf);
    sphere_non_lin_.set_radius(rp);
  }else{//non linear optimize failed -- use linear fit result
    sphere_non_lin_ = sphere_lin_;
  }
  // compute average distance error
  double dsum = 0.0;
  for (unsigned i=0; i<n; i++) {
    vgl_point_3d<T> p(points_[i]);
    double d = vgl_distance(p, sphere_non_lin_);
    dsum += d;
  }
  return static_cast<T>(dsum/n);
}
template <class T>
std::vector<vgl_point_3d<T> > vgl_fit_sphere_3d<T>::get_points() const{
  std::vector<vgl_point_3d<T> > ret;
  const unsigned n = static_cast<unsigned>(points_.size());
  for (unsigned i=0; i<n; i++){
    vgl_point_3d<T> p(points_[i]);
    ret.push_back(p);
  }
  return ret;
}
template <class T>
bool vgl_fit_sphere_3d<T>::fit_linear(const T error_marg, std::ostream* outstream){
  T error = fit_linear(outstream);
  return (error<error_marg);
}

template <class T>
bool vgl_fit_sphere_3d<T>::fit(const T error_marg, std::ostream* outstream, bool verbose){
  T error = fit(outstream, verbose);
  return (error<error_marg);
}
//--------------------------------------------------------------------------
#undef VGL_FIT_SPHERE_3D_INSTANTIATE
#define VGL_FIT_SPHERE_3D_INSTANTIATE(T) \
template class vgl_fit_sphere_3d<T >

#endif // vgl_fit_sphere_3d_hxx_
