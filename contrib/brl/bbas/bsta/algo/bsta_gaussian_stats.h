// This is brcv/seg/bsta/algo/bsta_gaussian_stats.h
#ifndef bsta_gaussian_stats_h_
#define bsta_gaussian_stats_h_

//:
// \file
// \brief Statistics using Gaussians 
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 1/26/06
//
// \verbatim
//  Modifications
// \endverbatim

#include <bsta/bsta_gaussian_sphere.h>
#include <bsta/bsta_gaussian_indep.h>
#include <bsta/bsta_gaussian_full.h>
#include <bsta/bsta_mixture.h>
#include <bsta/bsta_attributes.h>


//: An updater that normalizes weights of a mixture based on number of observations
// Each components is weighted as num_observation / total_observations
// The original weights are ignored 
template <class _comp_dist >
class bsta_mixture_weight_by_obs_updater
{
  private:
    typedef bsta_num_obs<_comp_dist> _comp_obs_dist;
    typedef bsta_mixture<_comp_obs_dist> _mix_dist;
    typedef typename _comp_dist::math_type T;

  public:
    //: The main function
    void operator() ( _mix_dist& mix ) const
    {
      T total = 0;
      vcl_vector<T> new_weights(mix.num_components(),T(0));
      for(unsigned int i=0; i<new_weights.size(); ++i){
        _comp_obs_dist& d = mix.distribution(i);
        total += d.num_observations;
        new_weights[i] = d.num_observations;
      }
      for(unsigned int i=0; i<new_weights.size(); ++i)
        mix.set_weight(i, new_weights[i]/total);
    }

};


//: Unroll the power calculation
template <class T, unsigned n>
struct bsta_compute_pow
{
  static inline T value(const T& v)
  { return v * bsta_compute_pow<T,n-1>::value(v); }
};

//: base case
// this is partial specialization: expect MSVC6 to complain
template <class T>
struct bsta_compute_pow<T,0>
{
  static inline T value(const T& v)
  { return 1; }
};



//: Used to sort a mixture of gaussians in decreasing order of fitness
template <class _gaussian>
struct bsta_gaussian_fitness
{
  private:
    typedef typename _gaussian::math_type T;
    enum { n = _gaussian::dimension };
  public:
    static bool order (const _gaussian& d1, const T& w1,
                       const _gaussian& d2, const T& w2)
    {
      return bsta_compute_pow<T,n>::value(w1*w1)/d1.det_covar() >
             bsta_compute_pow<T,n>::value(w2*w2)/d2.det_covar();
    }
};


//: Used to sort a mixture of gaussians in decreasing order of fitness
// Partial specialization optimized for the spherical case
template <class T, unsigned n>
struct bsta_gaussian_fitness<bsta_gaussian_sphere<T,n> >
{
  static bool order (const bsta_gaussian_sphere<T,n>& d1, const T& w1,
                     const bsta_gaussian_sphere<T,n>& d2, const T& w2)
  {
    return w1*w1/d1.var() > w2*w2/d2.var();
  }
};



#endif // bsta_gaussian_stats_h_
