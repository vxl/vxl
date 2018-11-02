#ifndef brad_phongs_model_est_h_
#define brad_phongs_model_est_h_

#include <iostream>
#include <vector>
#include <vnl/vnl_least_squares_function.h>
#include <vnl/vnl_double_3.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
class brad_phongs_model
{
 public:
  brad_phongs_model(float kd, float ks, float gamma, float normal_elev, float normal_azim);
  ~brad_phongs_model()= default;

  float val(float view_elev, float view_azim, float sun_elev, float sun_azim);
  float val(vnl_double_3 view_dir, float sun_elev, float sun_azim);

  float ks(){return ks_;}
  float kd(){return kd_;}
  float gamma(){return gamma_;}
  float normal_elev(){return normal_elev_;}
  float normal_azim(){return normal_azim_;}
  vnl_double_3 normal(){return normal_;}
 protected:

     float kd_;
     float ks_;
     float gamma_;
     float normal_elev_;
     float normal_azim_;

     vnl_double_3 normal_;
};

class brad_phongs_model_est : public vnl_least_squares_function
{
 public:
  brad_phongs_model_est(double sun_elev,
                        double sun_azim,
                        vnl_vector<double> & camera_elev,
                        vnl_vector<double> & camera_azim,
                        vnl_vector<double> & obs,
                        vnl_vector<double> & obs_weights,
                        bool with_grad);
  brad_phongs_model_est(double sun_elev,
                        double sun_azim,
                        std::vector<vnl_double_3> & viewing_dir,
                        vnl_vector<double> & obs,
                        vnl_vector<double> & obs_weights,
                        bool with_grad);

  void f(vnl_vector<double> const& x, vnl_vector<double>& y) override;
  float error_var(vnl_vector<double> const& x);

  void gradf(vnl_vector<double> const& x, vnl_matrix<double> &J) override;

 protected:
  double sun_elev_;
  double sun_azim_;
  std::vector<vnl_double_3> viewing_dirs_;
  vnl_vector<double>  camera_elev_;
  vnl_vector<double>  camera_azim_;
  vnl_vector<double>  obs_;
  vnl_vector<double>  obs_weights_;
};

class brad_phongs_model_approx
{
 public:
  brad_phongs_model_approx(float kd, float ks, float gamma, float normal_elev, float normal_azim);
  ~brad_phongs_model_approx()= default;

  float val(float view_elev, float view_azim, float sun_elev, float sun_azim);
  float val(vnl_double_3 view_dir, float sun_elev, float sun_azim);
 protected:

  float kd_;
  float ks_;
  float gamma_;
  vnl_double_3 normal_;
};

class brad_phongs_model_approx_est : public vnl_least_squares_function
{
 public:
  brad_phongs_model_approx_est(double sun_elev,
                        double sun_azim,
                        vnl_vector<double> & camera_elev,
                        vnl_vector<double> & camera_azim,
                        vnl_vector<double> & obs,
                        vnl_vector<double> & obs_weights,
                        bool with_grad);
  brad_phongs_model_approx_est(double sun_elev,
                        double sun_azim,
                        std::vector<vnl_double_3> & viewing_dir,
                        vnl_vector<double> & obs,
                        vnl_vector<double> & obs_weights,
                        bool with_grad);

  void f(vnl_vector<double> const& x, vnl_vector<double>& y) override;
  float error_var(vnl_vector<double> const& x);

  void gradf(vnl_vector<double> const& x, vnl_matrix<double> &J) override;

 protected:
  double sun_elev_;
  double sun_azim_;
  std::vector<vnl_double_3> viewing_dirs_;
  vnl_vector<double>  camera_elev_;
  vnl_vector<double>  camera_azim_;
  vnl_vector<double>  obs_;
  vnl_vector<double>  obs_weights_;
};
#endif // brad_phongs_model_est_h_
