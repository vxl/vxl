#ifndef brad_phongs_model_est_h_
#define brad_phongs_model_est_h_

#include <vnl/vnl_least_squares_function.h>
#include <vnl/vnl_double_3.h>

class brad_phongs_model
{
 public:
  brad_phongs_model(float kd, float ks, float gamma, float normal_elev, float normal_azim);
  ~brad_phongs_model(){}

  float val(float view_elev, float view_azim, float sun_elev, float sun_azim);
 protected:

  float kd_;
  float ks_;
  float gamma_;
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

  void f(vnl_vector<double> const& x, vnl_vector<double>& y);

  void gradf(vnl_vector<double> const& x, vnl_matrix<double> &J);

 protected:
  double sun_elev_;
  double sun_azim_;
  vnl_vector<double>  camera_elev_;
  vnl_vector<double>  camera_azim_;
  vnl_vector<double>  obs_;
  vnl_vector<double>  obs_weights_;
};

#endif // brad_phongs_model_est_h_
