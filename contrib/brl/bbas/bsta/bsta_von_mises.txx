// This is brl/bbas/bsta/bsta_von_mises.txx
#ifndef bsta_von_mises_txx_
#define bsta_von_mises_txx_
//:
// \file
// do not remove the following text
// Approved for public release, distribution unlimited (DISTAR Case 14389)
//

#include "bsta_von_mises.h"
#include <vcl_cassert.h>
#include <vcl_limits.h>
#include <vnl/vnl_math.h> // for pi

namespace
{
  //: Unroll the dot product of two vectors
  template <class T, unsigned n, unsigned index>
  struct compute_dot
  {
    static inline T value(const vnl_vector_fixed<T,n>& v0,
                          const vnl_vector_fixed<T,n>& v1)
    {
      return v0[index-1]*v1[index-1]
           + compute_dot<T,n,index-1>::value(v0, v1);
    }
  };

  //: base case
  // this is partial specialization: expect MSVC6 to complain
  template <class T, unsigned n>
  struct compute_dot<T,n,0>
  {
    static inline T value(const vnl_vector_fixed<T,n>& v0,
                          const vnl_vector_fixed<T,n>& v1)
    { return 0; }
  };

  //: base case
  // this is partial specialization: expect MSVC6 to complain
  template <class T>
      struct compute_dot<T,1,1>
  {
    static inline T value(const T& v0, const T& v1)
    { return v0*v1; }
  };

double pow_di (double ap, int bp)
{
  double pow, x;
  int n;
  unsigned long u;

  pow = 1;
  x = ap;
  n = bp;
  if (n != 0)
  {
    if (n < 0)
    {
      n = -n;
      x = 1 / x;
    }
    for (u = n;;)
    {
      if (u & 01)
        pow *= x;
      if (u >>= 1)
        x *= x;
      else
        break;
    }
  }
  return pow;
}

//       =============================================================
//       Purpose: This program computes the modified Bessel function
//                I0(x)
//       Input :  x
//       Output:  I0(x)
//
//       Example:
//         x      I0(x)
//       -------------------
//        1.0  .1266066E+01
//       10.0  .2815717E+04
//       20.0  .4355828E+08
//       30.0  .7816723E+12
//       40.0  .1489477E+17
//       50.0  .2932554E+21
//       =============================================================

bool I0(double x, double& bi0)
{
    // Initialized data

    static double a[12] = { .125,.0703125,.0732421875,.11215209960938,
                            .22710800170898,.57250142097473,1.7277275025845,6.0740420012735,
                            24.380529699556,110.01714026925,551.33589612202,3038.0905109224 };
    int i__1;
    double d__1;

    // Local variables
    static int k;
    static double r__;
    static int k0;

    static double x2 = x * x;
    if (x == 0.) {
      bi0 = 1.;
      return true;
    }
    else if (x <= 18.) {
      bi0 = 1.;
      r__ = 1.;
      for (k = 1; k <= 50; ++k) {
        r__ = r__ * .25 * x2 / (k * k);
        bi0 += r__;
        if ((d__1 = r__ / bi0, vcl_abs(d__1)) < 1e-15)
        return true;
      }
    }
    else {
      k0 = 12;
      if (x >= 35.0) {
        k0 = 9;
      }
      if (x >= 50.0) {
        k0 = 7;
      }
      static double ca = vcl_exp(x) / vcl_sqrt(vnl_math::pi * 2. * x);
      bi0 = 1.;
      static double xr = 1. / x;
      i__1 = k0;
      for (k = 1; k <= i__1; ++k)
        bi0 += a[k - 1] * pow_di(xr, k);
      bi0 = ca * bi0;
      return true;
    }
    return false;
}

}//namespace

template <class T>
T bsta_von_mises<T,3>::prob_density( typename bsta_von_mises<T,3>::vector_type const& v) const
{
  T dpt = compute_dot<T,3,3>::value(mean_, v);
  double dp = static_cast<double>(dpt);
  double k = static_cast<double>(kappa_);
  double ex = vcl_exp(k*dp);
  double norm = k/(vcl_exp(k)-vcl_exp(-k));
  norm /= 2.0*vnl_math::pi;
  return static_cast<T>(norm*ex);
}

template <class T>
T bsta_von_mises<T,3>::probability(typename bsta_von_mises<T,3>::vector_type const& v,const T theta_max) const
{
  //get gamma, the angle between v and the mean
  // make sure v is normalized
  vector_type nv = v;
  nv.normalize();
  double cos_gam = static_cast<double>(compute_dot<T,3,3>::value(mean_, nv));
  if (cos_gam>1.0) cos_gam=1.0;
  if (cos_gam<-1.0) cos_gam=-1.0;
  double gam = vcl_acos(cos_gam);
  double sin_gam = vcl_sin(gam),cos_2_gam = vcl_cos(2.0*gam);
  double cos_4_gam = vcl_cos(4.0*gam), tan_gam = sin_gam/cos_gam;
  double cos_gam_3 = cos_gam*cos_gam*cos_gam;
  double cos_theta_m = vcl_cos(theta_max);
  double cos_2_theta_m = vcl_cos(2.0*theta_max);
  double kap = static_cast<double>(kappa_);
  double prob = 0.0;
  if (kap<25) {
    double e_kap = vcl_exp(kap);
    double e_2_kap = e_kap*e_kap;
    double e_kap_cos_gam = vcl_exp(kap*cos_gam);
    double e_kap_cgam_cthm = vcl_exp(kap*cos_gam*cos_theta_m);
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
    double e_kap_cos_gam = vcl_exp(kap*(cos_gam-1.0));
    double e_kap_cgam_cthm = vcl_exp(kap*(cos_gam*cos_theta_m-1.0));
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
  T dpt = compute_dot<T,2,2>::value(mean_, v);
  double dp = static_cast<double>(dpt);
  double k = static_cast<double>(kappa_);
  double ex = vcl_exp(k*dp);
  double i0 = 1.0;
  if (!I0(k,i0)) return T(0);
  double norm = 1.0/i0;
  norm /= 2.0*vnl_math::pi;
  return static_cast<T>(norm*ex);
}

#define BSTA_VON_MISES_INSTANTIATE(T,n) \
template class bsta_von_mises<T,n >


#endif // bsta_von_mises_txx_
