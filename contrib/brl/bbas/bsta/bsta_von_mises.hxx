// This is brl/bbas/bsta/bsta_von_mises.hxx
#ifndef bsta_von_mises_hxx_
#define bsta_von_mises_hxx_
//:
// \file
// do not remove the following text
// Approved for public release, distribution unlimited (DISTAR Case 14389)
//

#include <iostream>
#include <limits>
#include "bsta_von_mises.h"
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_math.h> // for pi

namespace
{
  //: Unroll the dot product of two vectors
  template <class T, unsigned n, unsigned index>
  struct bsta_von_mises_compute_dot
  {
    static inline T value(const vnl_vector_fixed<T,n>& v0,
                          const vnl_vector_fixed<T,n>& v1)
    {
      return v0[index-1]*v1[index-1]
           + bsta_von_mises_compute_dot<T,n,index-1>::value(v0, v1);
    }
  };

  //: base case
  // this is a partial specialization
  template <class T, unsigned n>
  struct bsta_von_mises_compute_dot<T,n,0>
  {
    static inline T value(const vnl_vector_fixed<T,n>& /*v0*/,
                          const vnl_vector_fixed<T,n>& /*v1*/)
    { return 0; }
  };

  //: base case
  // this is partial specialization
  template <class T>
      struct bsta_von_mises_compute_dot<T,1,1>
  {
    static inline T value(const T& v0, const T& v1)
    { return v0*v1; }
  };

  double I0(double X)
  {
    double Y,P1,P2,P3,P4,P5,P6,P7,Q1,Q2,Q3,Q4,Q5,Q6,Q7,Q8,Q9,AX,BX;
    P1=1.0; P2=3.5156229; P3=3.0899424; P4=1.2067429;
    P5=0.2659732; P6=0.360768e-1; P7=0.45813e-2;
    Q1=0.39894228; Q2=0.1328592e-1; Q3=0.225319e-2;
    Q4=-0.157565e-2; Q5=0.916281e-2; Q6=-0.2057706e-1;
    Q7=0.2635537e-1; Q8=-0.1647633e-1; Q9=0.392377e-2;
    if (std::fabs(X) < 3.75) {
      Y=(X/3.75)*(X/3.75);
      return P1+Y*(P2+Y*(P3+Y*(P4+Y*(P5+Y*(P6+Y*P7)))));
    }
    else {
      AX=std::fabs(X);
      Y=3.75/AX;
      BX=std::exp(AX)/std::sqrt(AX);
      AX=Q1+Y*(Q2+Y*(Q3+Y*(Q4+Y*(Q5+Y*(Q6+Y*(Q7+Y*(Q8+Y*Q9)))))));
      return AX*BX;
    }
  }
}//namespace


template <class T>
T bsta_von_mises<T,3>::prob_density( typename bsta_von_mises<T,3>::vector_type const& v) const
{
  T dpt = bsta_von_mises_compute_dot<T,3,3>::value(mean_, v);
  double dp = static_cast<double>(dpt);
  double k = static_cast<double>(kappa_);
  double ex = std::exp(k*dp);
  double norm = k/(std::exp(k)-std::exp(-k));
  norm /= vnl_math::twopi;
  return static_cast<T>(norm*ex);
}

template <class T>
T bsta_von_mises<T,3>::probability(typename bsta_von_mises<T,3>::vector_type const& v,const T theta_max) const
{
  //get gamma, the angle between v and the mean
  // make sure v is normalized
  vector_type nv = v;
  nv.normalize();
  double cos_gam = static_cast<double>(bsta_von_mises_compute_dot<T,3,3>::value(mean_, nv));
  if (cos_gam>1.0) cos_gam=1.0;
  if (cos_gam<-1.0) cos_gam=-1.0;
  double gam = std::acos(cos_gam);
  double sin_gam = std::sin(gam),cos_2_gam = std::cos(2.0*gam);
  double cos_4_gam = std::cos(4.0*gam), tan_gam = sin_gam/cos_gam;
  double cos_gam_3 = cos_gam*cos_gam*cos_gam;
  double cos_theta_m = std::cos(theta_max);
  double cos_2_theta_m = std::cos(2.0*theta_max);
  double kap = static_cast<double>(kappa_);
  double prob = 0.0;
  if (kap<25) {
    double e_kap = std::exp(kap);
    double e_2_kap = e_kap*e_kap;
    double e_kap_cos_gam = std::exp(kap*cos_gam);
    double e_kap_cgam_cthm = std::exp(kap*cos_gam*cos_theta_m);
    double t1 = -1.0/(64.0*(e_2_kap -1));
    double t2 = (e_kap_cos_gam - e_kap_cgam_cthm);
    double t3 = (e_kap_cos_gam - cos_2_theta_m*e_kap_cgam_cthm);
    double t3a = (e_kap_cos_gam - cos_theta_m*e_kap_cgam_cthm);
    double t4 = (-16 + kap*kap*(cos_4_gam -1.0)-48.0*cos_2_gam);
    double t5 = t2*t4/cos_gam_3;
    double t6 = kap*t3*sin_gam - 4.0*tan_gam*t3a;
    double t7 = 8.0*kap*tan_gam*t6;
    double t8 = e_kap*(t5+t7);
    prob = t1*t8;
  }
  else {
    double e_kap_cos_gam = std::exp(kap*(cos_gam-1.0));
    double e_kap_cgam_cthm = std::exp(kap*(cos_gam*cos_theta_m-1.0));
    double t1 = -1.0/(64.0);
    double t2 = (e_kap_cos_gam - e_kap_cgam_cthm);
    double t3 = (e_kap_cos_gam - cos_2_theta_m*e_kap_cgam_cthm);
    double t3a = (e_kap_cos_gam - cos_theta_m*e_kap_cgam_cthm);
    double t4 = (-16 + kap*kap*(cos_4_gam -1.0)-48.0*cos_2_gam);
    double t5 = t2*t4/cos_gam_3;
    double t6 = kap*t3*sin_gam - 4.0*tan_gam*t3a;
    double t7 = 8.0*kap*tan_gam*t6;
    double t8 = t5+t7;
    if (t8==0)
      prob = t8;
    else
      prob = t1*t8;
  }
  return static_cast<T>(prob);
}

template <class T>
T bsta_von_mises<T,2>::prob_density(typename bsta_von_mises<T,2>::vector_type const& v) const
{
  T dpt = bsta_von_mises_compute_dot<T,2,2>::value(mean_, v);
  double dp = static_cast<double>(dpt);
  double k = static_cast<double>(kappa_);
  double ex = std::exp(k*dp);
  double i0 = I0(k);
  double norm = 1.0/i0;
  norm /= vnl_math::twopi;
  return static_cast<T>(norm*ex);
}


#define BSTA_VON_MISES_INSTANTIATE(T,n) \
template class bsta_von_mises<T,n >


#endif // bsta_von_mises_hxx_
