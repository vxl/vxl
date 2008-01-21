// This is brcv/seg/bsta/algo/bsta_adaptive_updater.h
#ifndef bsta_adaptive_updater_h_
#define bsta_adaptive_updater_h_

//:
// \file
// \brief Adaptive updaters for mixtures of Gaussians (i.e. Grimson and similar) 
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 07/13/05
//
// This file contains updaters based on 
// "Adaptive background mixture models for real-time tracking"
// Grimson et. al.
//
// \verbatim
//   Modifications
// \endverbatim

#include <bsta/bsta_distribution.h>
#include <bsta/bsta_mixture.h>
#include <bsta/bsta_mixture_fixed.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_gauss_ff3.h>
#include <vnl/vnl_matrix.h>
#include "bsta_adaptive_updater.h"
#include "bsta_gaussian_updater.h"
#include "bsta_gaussian_stats.h"

#include <vcl_iostream.h>
#include <vcl_limits.h>
//#include "bsta_gaussian_stats.h"


//: A mixture of Gaussians adaptive updater
// base class for common functionality in adaptive updating schemes
template <class _mix_dist>
class bsta_mg_adaptive_updater
{
  private:  
    typedef typename _mix_dist::dist_type _obs_gaussian;
    typedef typename _obs_gaussian::contained_type _gaussian;
    typedef typename _gaussian::math_type T;
    typedef typename _gaussian::vector_type _vector;

  protected:
    //: Constructor
    bsta_mg_adaptive_updater(const _gaussian& model,
                               unsigned int max_cmp = 5)
     : init_gaussian_(model,T(1)),
       max_components_(max_cmp) {}

    void insert(_mix_dist& mixture, const _vector& sample, T init_weight) const
    {
      bool removed = false;
      if(mixture.num_components() >= max_components_){
        removed = true;
        do{
          mixture.remove_last(); 
        }while(mixture.num_components() >= max_components_);
      }

      // if a mixture is removed renormalize the rest
      if(removed){
        T adjust = T(0);
        for(unsigned int i=0; i<mixture.num_components(); ++i)
          adjust += mixture.weight(i);
        adjust = (T(1)-init_weight) / adjust;
        for(unsigned int i=0; i<mixture.num_components(); ++i)
          mixture.set_weight(i, mixture.weight(i)*adjust);
      }
      init_gaussian_.set_mean(sample);
      mixture.insert(init_gaussian_,init_weight);
    }

    //: A model for new Gaussians inserted
    mutable _obs_gaussian init_gaussian_;
    //: The maximum number of components in the mixture
    unsigned int max_components_;

};


//: A mixture of Gaussians statistical updater
// This updater treats all data equally
template <class _mix_dist>
class bsta_mg_statistical_updater : public bsta_mg_adaptive_updater<_mix_dist>
{
  public:
    typedef typename _mix_dist::dist_type _obs_gaussian;
    typedef typename _obs_gaussian::contained_type _gaussian;
    typedef typename _gaussian::math_type T;
    typedef typename _gaussian::vector_type _vector;
    typedef bsta_num_obs<_mix_dist> _obs_mix_dist;

    enum { data_dimension = _gaussian::dimension };

    //: Constructor
    bsta_mg_statistical_updater(const _gaussian& model,
                                  unsigned int max_cmp = 5,
                                  T g_thresh = T(3),
                                  T min_stdev = T(0))
      : bsta_mg_adaptive_updater<_mix_dist>(model, max_cmp),
        gt2_(g_thresh*g_thresh), min_var_(min_stdev*min_stdev) {}

    //: The main function
    void operator() ( _obs_mix_dist& mix, const _vector& sample ) const
    {
      mix.num_observations += T(1);
      this->update(mix, sample, T(1)/mix.num_observations);
    }

    void update( _mix_dist& mix, const _vector& sample, T alpha ) const;

   // void update( _mix_dist& mix, const T & sample, T alpha ) const;
    //: Squared Gaussian Mahalanobis distance threshold
    T gt2_;
    //: Minimum variance allowed in each Gaussian component
    T min_var_;
};


//: A mixture of Gaussians statistical updater 
// This updater treats all data equally
template <class _mix_dist>
class bsta_mg_window_updater : public bsta_mg_statistical_updater<_mix_dist>
{
  public:
    typedef typename _mix_dist::dist_type _obs_gaussian;
    typedef typename _obs_gaussian::contained_type _gaussian;
    typedef typename _gaussian::math_type T;
    typedef typename _gaussian::vector_type _vector;
    typedef bsta_num_obs<_mix_dist> _obs_mix_dist;

    enum { data_dimension = _gaussian::dimension };

    //: Constructor
    bsta_mg_window_updater(const _gaussian& model,
                             unsigned int max_cmp = 5,
                             T g_thresh = T(3),
                             T min_stdev = T(0),
                             unsigned int window_size = 40)
  : bsta_mg_statistical_updater<_mix_dist>(model, max_cmp, g_thresh, min_stdev),
        window_size_(window_size) {}

    //: The main function
    void operator() ( _obs_mix_dist& mix, const _vector& sample ) const
    {
      if(mix.num_observations < window_size_)
        mix.num_observations += T(1);
      this->update(mix, sample, T(1)/mix.num_observations);
    }

  protected:
    unsigned int window_size_;
};

//: A mixture of Gaussians weighted statistical updater
// This updater treats data according to a specified weigth
template <class _mix_dist>
class bsta_mg_weighted_updater : bsta_mg_statistical_updater<_mix_dist>
{
  public:
    typedef typename _mix_dist::dist_type _obs_gaussian;
    typedef typename _obs_gaussian::contained_type _gaussian;
    typedef typename _gaussian::math_type T;
    typedef typename _gaussian::vector_type _vector;
    typedef bsta_num_obs<_mix_dist> _obs_mix_dist;

    enum { data_dimension = _gaussian::dimension };

    //: Constructor
    bsta_mg_weighted_updater(const _gaussian& model,
                                  unsigned int max_cmp = 5,
                                  T g_thresh = T(3),
                                  T min_stdev = T(0))
      : bsta_mg_adaptive_updater<_mix_dist>(model, max_cmp),
        gt2_(g_thresh*g_thresh), min_var_(min_stdev*min_stdev) {}

    //: The main function
    void operator() ( _obs_mix_dist& mix, const _vector& sample, const T weight ) const
    {
      mix.num_observations += weight;
      this->update(mix, sample, weight/mix.num_observations);
    }

    void update( _mix_dist& mix, const _vector& sample, T alpha ) const;

   // void update( _mix_dist& mix, const T & sample, T alpha ) const;
    //: Squared Gaussian Mahalanobis distance threshold
    T gt2_;
    //: Minimum variance allowed in each Gaussian component
    T min_var_;
};


//: A mixture of Gaussians statistical updater
// using the grimson approximation to prior probablilities
template <class _mix_dist>
class bsta_mg_grimson_statistical_updater : public bsta_mg_adaptive_updater<_mix_dist>
{
  public:
    typedef typename _mix_dist::dist_type _obs_gaussian;
    typedef typename _obs_gaussian::contained_type _gaussian;
    typedef typename _gaussian::math_type T;
    typedef typename _gaussian::vector_type _vector;
    typedef bsta_num_obs<_mix_dist> _obs_mix_dist;

    enum { data_dimension = _gaussian::dimension };

    //: Constructor
    bsta_mg_grimson_statistical_updater(const _gaussian& model,
                                          unsigned int max_cmp = 5,
                                          T g_thresh = T(3),
                                          T min_stdev = T(0) )
      : bsta_mg_adaptive_updater<_mix_dist>(model, max_cmp),
        gt2_(g_thresh*g_thresh), min_var_(min_stdev*min_stdev) {}

    //: The main function
    void operator() ( _obs_mix_dist& mix, const _vector& sample ) const
    {
      mix.num_observations += T(1);
      this->update(mix, sample, T(1)/mix.num_observations);
    }

    void update( _mix_dist& mix, const _vector& sample, T alpha ) const;

    //: Squared Gaussian Mahalanobis distance threshold
    T gt2_;
    //: Minimum variance allowed in each Gaussian component
    T min_var_;
};

//: A mixture of Gaussians window updater 
// using the grimson approximation to prior probablilities
template <class _mix_dist>
class bsta_mg_grimson_window_updater : public bsta_mg_grimson_statistical_updater<_mix_dist>
{
  public:
    typedef typename _mix_dist::dist_type _obs_gaussian;
    typedef typename _obs_gaussian::contained_type _gaussian;
    typedef typename _gaussian::math_type T;
    typedef typename _gaussian::vector_type _vector;
    typedef bsta_num_obs<_mix_dist> _obs_mix_dist;

    enum { data_dimension = _gaussian::dimension };

    //: Constructor
    bsta_mg_grimson_window_updater(const _gaussian& model,
                                     unsigned int max_cmp = 5,
                                     T g_thresh = T(3),
                                     T min_stdev = T(0),
                                     unsigned int window_size = 40) 
      : bsta_mg_grimson_statistical_updater<_mix_dist>(model, max_cmp, g_thresh, min_stdev),
        window_size_(window_size) {}

    //: The main function
    void operator() ( _obs_mix_dist& mix, const _vector& sample ) const
    {
      if(mix.num_observations < window_size_)
        mix.num_observations += T(1);
      this->update(mix, sample, T(1)/mix.num_observations);
    }

  protected:
    unsigned int window_size_;
};

//: A mixture of Gaussians statistical weighted updater
// using the grimson approximation to prior probablilities
template <class _mix_dist>
class bsta_mg_grimson_weighted_updater : bsta_mg_grimson_statistical_updater<_mix_dist>
{
  public:
    typedef typename _mix_dist::dist_type _obs_gaussian;
    typedef typename _obs_gaussian::contained_type _gaussian;
    typedef typename _gaussian::math_type T;
    typedef typename _gaussian::vector_type _vector;
    typedef bsta_num_obs<_mix_dist> _obs_mix_dist;

    enum { data_dimension = _gaussian::dimension };

    //: Constructor
    bsta_mg_grimson_weighted_updater(const _gaussian& model,
                                          unsigned int max_cmp = 5,
                                          T g_thresh = T(3),
                                          T min_stdev = T(0) )
      : bsta_mg_adaptive_updater<_mix_dist>(model, max_cmp),
        gt2_(g_thresh*g_thresh), min_var_(min_stdev*min_stdev) {}

    //: The main function
    void operator() ( _obs_mix_dist& mix, const _vector& sample, const T weight ) const
    {
      mix.num_observations += weight;
      this->update(mix, sample, weight/mix.num_observations);
    }

    void update( _mix_dist& mix, const _vector& sample, T alpha ) const;

    //: Squared Gaussian Mahalanobis distance threshold
    T gt2_;
    //: Minimum variance allowed in each Gaussian component
    T min_var_;
};


#endif // bsta_adaptive_updater_h_
