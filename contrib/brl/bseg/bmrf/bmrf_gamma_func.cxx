// This is brl/bseg/bmrf/bmrf_gamma_func.cxx
//:
// \file

#include "bmrf_gamma_func.h"
#include "bmrf_epi_seg.h"
#include "bmrf_epi_seg_compare.h"
#include "bmrf_epi_point.h"
#include <vcl_utility.h>



//: Constructor
bmrf_pwl_gamma_func::bmrf_pwl_gamma_func( const bmrf_epi_seg_sptr& ep1, 
                                          const bmrf_epi_seg_sptr& ep2,
                                          double t )
{
  double min_alpha = bmrf_min_alpha(ep1, ep2);
  double max_alpha = bmrf_max_alpha(ep1, ep2);
  double d_alpha =   bmrf_d_alpha(ep1, ep2);

  int i=0;
  double alpha = ep1->p(0)->alpha();
  for (i=0; i<ep1->n_pts() && (alpha = ep1->p(i)->alpha())<min_alpha; ++i);

  for (; i<ep1->n_pts() && (alpha = ep1->p(i)->alpha())<=max_alpha; ++i){
    knots_[alpha] = (1 - (ep1->s(alpha) / ep2->s(alpha)))/t;
  }
}


//: Return the average gamma value at time \p t
//  Here, this value is independent of t
double 
bmrf_pwl_gamma_func::mean(double t) const
{
  if(knots_.empty())
    return 0.0;

  vcl_map<double,double>::const_iterator last_k = knots_.begin(), k = knots_.begin();
  ++k;
  double sum = 0.0;
  double min_alpha = last_k->first;
  for(; k != knots_.end();  ++k){
    double da = k->first - last_k->first;
    sum += (last_k->second + k->second)*da;
    last_k = k;
  }
  double max_alpha = last_k->first;
  return sum/(2.0*(max_alpha - min_alpha));
}


//: Return the gamma value for any \p alpha and time \p t
//  Here, this value is independent of t
double 
bmrf_pwl_gamma_func::value(double alpha, double t) const
{
  if( knots_.empty() )
    return 0.0;

  typedef vcl_map<double,double>::const_iterator k_itr;
  vcl_pair<k_itr,k_itr> rng = knots_.equal_range(alpha);

  // the value is at one of the knots
  if(rng.first != rng.second)
    return rng.first->second;

  // the value is before the first knot
  if(rng.first == knots_.begin())
    return rng.first->second;

  // the value is after the last knot
  if(rng.first == knots_.end())
    return (--rng.first)->second;

  // the value is between knots
  k_itr k2 = rng.first, k1 = --rng.first;
  double p = (alpha - k1->first)/(k2->first - k1->first);
  return (1-p)*k1->second + p*k2->second;
}

