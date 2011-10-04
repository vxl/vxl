#include <brad/brad_synoptic_function_1d.h>
#include <vcl_fstream.h>
#include <vcl_cmath.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_4x4.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_inverse.h>
#include <vnl/algo/vnl_svd.h>
bool brad_synoptic_function_1d::load_samples(vcl_string const& path)

{
  vcl_ifstream is(path.c_str());
  if(!is.is_open())
    return false;
  unsigned npts;
  is >> npts;
  if(npts==0)
    return false;
  elev_.resize(npts,0.0);
  azimuth_.resize(npts,0.0);
  vis_.resize(npts,0.0);
  intensity_.resize(npts,0.0);
  double x, y, z;
  is >> x >> y >> z;//skip point for now
  vcl_string img_name;
  for(unsigned i = 0; i<npts; ++i){
    is >> img_name >> intensity_[i] >>vis_[i]  >> elev_[i] >>azimuth_[i] ;
  }
  this->fit_intensity_cubic();
  return true;
}

double brad_synoptic_function_1d::
angle(double elev0, double az0, double elev1, double az1){
  double se0 = vcl_sin(elev0), ce0 = vcl_cos(elev0);
  double sa0 = vcl_sin(az0), ca0 = vcl_cos(az0);
  double se1 = vcl_sin(elev1), ce1 = vcl_cos(elev1);
  double sa1 = vcl_sin(az1), ca1 = vcl_cos(az1);
  vnl_double_3 v0(se0*ca0,se0*sa0,ce0), v1(se1*ca1,se1*sa1,ce1);
  double dp = dot_product(v0, v1);
  return vcl_acos(dp);
}


double brad_synoptic_function_1d::arc_length(unsigned index)
{
  unsigned n = this->size();
  if(n==0) return 0.0;
  if(index==0) return 0.0;
  double sum = 0.0;
  for(unsigned k = 1; k<=index; k++)
    sum += brad_synoptic_function_1d::angle(elev_[k-1],azimuth_[k-1],
                                            elev_[k],azimuth_[k]);
  return sum;
}

void brad_synoptic_function_1d::fit_intensity_cubic(){
  unsigned n = this->size();
  // form X, W matrices, and y vector
  vnl_matrix<double> X(n,4, 1.0);
  vnl_matrix<double> W(n,n, 0.0);
  vnl_vector<double> y(n);
  for(unsigned r = 0; r<n;++r){
    double s = this->arc_length(r);
    X[r][1]=s; X[r][2]=s*s; X[r][3]=X[r][2]*s;
    W[r][r] = vis_[r]; y[r]=intensity_[r];
  }
  //compute cubic coefficients
  vnl_matrix<double> Xt = X.transpose();
  vnl_matrix<double> Xtw = Xt*W;
  vnl_double_4x4 M = Xtw*X;
  vnl_double_4 q = Xtw*y;
  vnl_double_4x4 Minv = vnl_inverse(M);
  cubic_coef_int_ = Minv*q;
  vnl_vector<double> error = W*(y-X*cubic_coef_int_);
  fit_error_ = error.squared_magnitude();
}


double brad_synoptic_function_1d::cubic_interp_inten(double arc_length)
{
  double s = arc_length;
  double s2 = s*s, s3 = s2*s;
  double v = cubic_coef_int_[0];
  v += cubic_coef_int_[1]*s;
  v += cubic_coef_int_[2]*s2;
  v += cubic_coef_int_[3]*s3;
  return v;
}


