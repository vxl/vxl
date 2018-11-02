// This is brl/bbas/bsta/algo/bsta_von_mises_updater.hxx
#ifndef bsta_von_mises_updater_hxx_
#define bsta_von_mises_updater_hxx_
//:
// \file
#include <iostream>
#include <limits>
#include "bsta_von_mises_updater.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
//: The main function
template <class von_mises_dist_>
void bsta_von_mises_updater<von_mises_dist_>::update(obs_vm_dist_& pdist, const vect_t& vsum, math_t alpha) const
{
  if(!(data_dimension == 2||data_dimension == 3)){
    std::cerr << "von mises update only implemented for 2-d and 3-d vectors\n";
    return;
  }
  math_t r_mag = vsum.magnitude();
  pdist.set_mean(vsum/r_mag);
  if(alpha ==math_t(1)){
    pdist.set_kappa(initial_kappa_);
    return;
  }
  //  double minv = static_cast<double>(std::numeric_limits<math_t>::min());
  double minv = static_cast<double>(0.00001);
  double r_bar = static_cast<double>(r_mag*alpha);
  assert(r_bar<=1.00001); //round off in vector normalization
  if(r_bar>1.0) r_bar = 1.0;
  if(data_dimension == 3){
  if(r_bar>=0.0&&r_bar<0.2941)
    {
      double k = 16.2227*(0.348127-0.351118*std::sqrt(0.980629-r_bar));
      pdist.set_kappa(static_cast<math_t>(k));
      return;
    }
  if(r_bar>=0.2941&&r_bar<0.5398)
    {
      double k = 9.96768*(0.373669-0.447938*std::sqrt(0.686434-r_bar));
      pdist.set_kappa(static_cast<math_t>(k));
      return;
    }
  if(r_bar>=0.5398&&r_bar<0.6775)
    {
      double k = 12.0976*(0.338089-0.406599*std::sqrt(0.717942-r_bar));
      pdist.set_kappa(static_cast<math_t>(k));
      return;
    }
  if(r_bar>=0.6775)
    {
      double k = 1.0;
      double temp = (1.0-r_bar);
      if(temp<minv)
        k = static_cast<double>(std::numeric_limits<math_t>::max());
      else
        k = 1.0/temp;
      pdist.set_kappa(static_cast<math_t>(k));
      return;
    }
  }
  if(data_dimension == 2){
    double t2 = r_bar*r_bar;
    double t3 = t2*r_bar;
    if(r_bar<0.53)
      {
        double t5 = t2*t3;
        double k = 2.0*r_bar+ t3 + 5.0*t5/6.0;
        pdist.set_kappa(static_cast<math_t>(k));
        return;
      }
    if(r_bar>=0.53&&r_bar<0.85)
      {
        double t1 = 1.0;
        double temp = (1.0-r_bar);
        if(temp<minv)
          t1 = static_cast<double>(std::numeric_limits<math_t>::max());
        else
          t1 = 1.0/temp;
        double k = -0.4 + 1.39*r_bar + 0.43*t1;
        pdist.set_kappa(static_cast<math_t>(k));
        return;
      }
    if(r_bar>=0.85)
      {
        double k = 1.0;
        double temp = t3 - 4.0*t2 + 3.0*r_bar;
        if(temp<minv)
          k = static_cast<double>(std::numeric_limits<math_t>::max());
        else
          k = 1.0/temp;
        pdist.set_kappa(static_cast<math_t>(k));
        return;
      }
  }
}


#define BSTA_VON_MISES_UPDATER_INSTANTIATE(T) \
template class bsta_von_mises_updater<T >

#endif // bsta_von_mises_updater_hxx_
