#ifndef boxm2_vecf_mouth_params_h_
#define boxm2_vecf_mouth_params_h_
//:
// \file
// \brief  Parameters for the mouth model
//
// \author J.L. Mundy
// \date   30 December 2015
//
#include <iostream>
#include "boxm2_vecf_articulated_params.h"
#include <vgl/vgl_vector_3d.h>
#include <vnl/vnl_vector_fixed.h>
class boxm2_vecf_mouth_params : public boxm2_vecf_articulated_params{
 public:

 boxm2_vecf_mouth_params(): offset_(vgl_vector_3d<double>(0.0, 0.0, 0.0)), t_min_(0.0), t_max_(1.0), x_min_(-32), x_max_(32.0), show_mouth_region_(false){
  }
  bool show_mouth_region_;
  // min and max t for validity
  double t_min_, t_max_;
  double x_min_, x_max_;
  vgl_vector_3d<double> offset_;

  //: vector of  monomials to form the polynomial when multiplied by the coefficients
  vnl_vector_fixed<double, 3> m(double xp) const{
    vnl_vector_fixed<double, 3> q;
    q[0]=1.0; q[1] = xp; q[2]=q[1]*q[1];
    return q;
  }
  //: upper lip polynomial (t = 0)
  vnl_vector_fixed<double, 3> lip_t0() const{
    double coefs[3]={-59.3316, -0.00238891, -0.00608941};
    vnl_vector_fixed<double, 3> temp = vnl_vector_fixed<double, 3>(coefs);
    return temp;}
  //: lower lip polynomial (t = 1)
  vnl_vector_fixed<double, 3> lip_t1() const{
    double coefs[3]={-91.7383, -0.00794587, 0.0269943};
    vnl_vector_fixed<double, 3> temp = vnl_vector_fixed<double, 3>(coefs);
    return temp;}

  double t(double xp, double y) const{
    vnl_vector_fixed<double, 3> c0 = lip_t0();
    vnl_vector_fixed<double, 3> c1 = lip_t1();
    vnl_vector_fixed<double, 3> mm =  m(xp);
    double temp0 = dot_product(mm, c0);
    double temp1 = dot_product(mm, c1);
    double neu = y - temp0;
    double den = temp1 - temp0;
    return neu/den;
  }
};
std::ostream&  operator << (std::ostream& s, boxm2_vecf_mouth_params const& pr);
std::istream&  operator >> (std::istream& s, boxm2_vecf_mouth_params& pr);

#endif// boxm2_vecf_mouth_params
