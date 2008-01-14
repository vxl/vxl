// This is brcv/seg/bsta/algo/bsta_adaptive_updater.h
#ifndef bsta_adaptive_updater_h_
#define bsta_adaptive_updater_h_

//:
// \file
// \brief Adaptive updaters (i.e. Grimson and similar) 
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 07/13/05
//
// This file contains updaters based on 
// "Adaptive background mixture models for real-time tracking"
// Grimson et. al.
//
// \verbatim
//  Modifications
// \endverbatim

#include <bsta/bsta_distribution.h>
#include <bsta/bsta_mixture.h>
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
template <class _gaussian>
class bsta_mg_adaptive_updater
{
  private:
    typedef bsta_num_obs<_gaussian> _obs_gaussian;
    typedef bsta_mixture<_obs_gaussian> _mix_dist;
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
template <class _gaussian>
class bsta_mg_statistical_updater : public bsta_mg_adaptive_updater<_gaussian>
{
  public:
    typedef bsta_num_obs<_gaussian> _obs_gaussian;
    typedef bsta_mixture<_obs_gaussian> _mix_dist;
    typedef bsta_num_obs<_mix_dist> _obs_mix_dist;
    typedef typename _gaussian::math_type T;
    typedef typename _gaussian::vector_type _vector;

    enum { data_dimension = _gaussian::dimension };

    //: Constructor
    bsta_mg_statistical_updater(const _gaussian& model,
                                  unsigned int max_cmp = 5,
                                  T g_thresh = T(3),
                                  T min_stdev = T(0))
      : bsta_mg_adaptive_updater<_gaussian>(model, max_cmp),
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

    typedef bsta_num_obs<bsta_gaussian_full<float,3> > _obs_gaussian_full_float_3;
    typedef bsta_mixture<_obs_gaussian_full_float_3 > _mix_dist_full_float_3;
    typedef bsta_num_obs<_mix_dist_full_float_3> _obs_mix_dist_full_float_3;

//template<>
//class bsta_mg_statistical_updater<bsta_gaussian_full<float,3> > 
//    : public bsta_mg_adaptive_updater<bsta_gaussian_full<float,3> >
//{
//  public:
//
//    enum { data_dimension = 3 };
//
//    //: Constructor
//    bsta_mg_statistical_updater(const bsta_gaussian_full<float,3> & model,
//                                  unsigned int max_cmp = 5,
//                                  float g_thresh = 3,
//                                  float min_stdev = 0)
//      : bsta_mg_adaptive_updater<bsta_gaussian_full<float,3> >(model, max_cmp),
//        gt2_(g_thresh*g_thresh), min_var_(min_stdev*min_stdev) {}
//
//    //: The main function
//    void operator() ( _obs_mix_dist_full_float_3& mix, const vnl_vector_fixed<float,3> & sample,vnl_matrix_fixed<float,3,3> covar ) const
//    {
//      mix.num_observations += float(1);
//      this->update(mix, sample, float(1)/mix.num_observations, covar);
//    }
//
//    void update( _mix_dist_full_float_3& mix, const vnl_vector_fixed<float,3> & sample, float alpha, vnl_matrix_fixed<float,3,3> covar ) const
//    {
//
//  const unsigned num_components = mix.num_components();
//
//  // prune components by mahalanobis distance
//  static vcl_vector<float> probs;
//  probs.resize(num_components,float(0));
//  static vcl_vector<unsigned int> matched;
//  matched.clear();
//
//  for(unsigned int i=0; i<num_components; ++i){
//    
//    float sqr_dist =  mix.distribution(i).sqr_mahalanobis_dist(sample);
//    if(sqr_dist < gt2_){
//      matched.push_back(i);
//      probs[i] = sqr_dist; // initially these are distances, not probabilities
//    }
//  }
//
//  // if no matches add a new component
//  if(matched.empty()){
//      //init_gaussian_.set_covar(covar);
//      //insert(mix,sample,alpha);
//      //mix.normalize_weights();
//
//      bool removed = false;
//      if(mix.num_components() >= max_components_){
//        removed = true;
//        do{
//          mix.remove_last(); 
//        }while(mix.num_components() >= max_components_);
//      }
//
//      // if a mixture is removed renormalize the rest
//      if(removed){
//        float adjust = float(0);
//        for(unsigned int i=0; i<mix.num_components(); ++i)
//          adjust += mix.weight(i);
//        adjust = (float(1)-alpha) / adjust;
//        for(unsigned int i=0; i<mix.num_components(); ++i)
//          mix.set_weight(i, mix.weight(i)*adjust);
//      }
//      bsta_gaussian_full<float,3> comp_gauss(sample,covar);
//      bsta_num_obs<bsta_gaussian_full<float,3> > inigauss(comp_gauss,1);
//      //init_gaussian_.set_mean(sample);
//      //init_gaussian_.set_covar(covar);
//      mix.insert(comp_gauss,alpha);
//      mix.normalize_weights();
//      //if(mix.num_components()>0)
//      //{     
//      //      _obs_gaussian_full_float_3 & g =mix.distribution(mix.num_components()-1);
//      //      g.
// //       
//      // }   
//    
//  }
//  else{
//    // update the weights
//    for(unsigned int i=0; i<num_components; ++i){
//      float weight = (float(1)-alpha) * mix.weight(i);
//      mix.set_weight(i,weight);
//    }
//    // special case of 1 match - don't need to compute probabilites
//    if (matched.size() == 1){
//      unsigned int m_idx = matched.front();
//      mix.set_weight(m_idx,mix.weight(m_idx)+alpha);
//      _obs_gaussian_full_float_3 & g = mix.distribution(m_idx);
//      g.num_observations += float(1);
//      float rho = (1.0-alpha)/g.num_observations + alpha; 
//      bsta_update_gaussian(g, rho, sample, min_var_);
//    }
//    else{
//      // compute probabilites for each match
//      vcl_vector<unsigned int>::iterator itr;
//      float sum_probs = float(0);
//      for( itr = matched.begin(); itr != matched.end(); ++itr){
//        const unsigned int i = *itr;
//        _obs_gaussian_full_float_3 & g = mix.distribution(i);
//        probs[i] = g.dist_probability(probs[i]) * mix.weight(i);
//        sum_probs += probs[i];
//      }
//      // update each match
//      for( itr = matched.begin(); itr != matched.end(); ++itr){
//        const unsigned int i = *itr;
//        probs[i] /= sum_probs;
//        mix.set_weight(i,mix.weight(i)+alpha*probs[i]);
//        _obs_gaussian_full_float_3 & g = mix.distribution(i);
//        g.num_observations += probs[i];
//        float rho = probs[i] * ((1-alpha)/g.num_observations + alpha);
//        bsta_update_gaussian(g, rho, sample);
//      }
//    }
//  }
//
//  mix.sort(bsta_gaussian_fitness<bsta_gaussian_full<float,3> >::order);
//
//  // try to clean up gaussian components with weights that have converged to zero
//  if(mix.weight(mix.num_components()-1) < vcl_numeric_limits<float>::epsilon()){
//    mix.remove_last();
//    float sum = 0;
//    for(unsigned int i=0; i<mix.num_components(); ++i){
//      sum += mix.weight(i);
//    }
//    vcl_cout << "removed, total weight = " << sum << vcl_endl;
//    mix.normalize_weights();
//  }
//    }
//
//
//
//
//
// 
//    //void update( _mix_dist& mix, const T & sample, T alpha ) const;
//    //: Squared Gaussian Mahalanobis distance threshold
//    float gt2_;
//    //: Minimum variance allowed in each Gaussian component
//    float min_var_;
//};
//
//


//: A mixture of Gaussians statistical updater 
// This updater treats all data equally
template <class _gaussian>
class bsta_mg_window_updater : public bsta_mg_statistical_updater<_gaussian>
{
  public:
    typedef bsta_num_obs<_gaussian> _obs_gaussian;
    typedef bsta_mixture<_obs_gaussian> _mix_dist;
    typedef bsta_num_obs<_mix_dist> _obs_mix_dist;
    typedef typename _gaussian::math_type T;
    typedef typename _gaussian::vector_type _vector;

    enum { data_dimension = _gaussian::dimension };

    //: Constructor
    bsta_mg_window_updater(const _gaussian& model,
                             unsigned int max_cmp = 5,
                             T g_thresh = T(3),
                             T min_stdev = T(0),
                             unsigned int window_size = 40)
  : bsta_mg_statistical_updater<_gaussian>(model, max_cmp, g_thresh, min_stdev),
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


//: A mixture of Gaussians statistical updater
// using the grimson approximation to prior probablilities
template <class _gaussian>
class bsta_mg_grimson_statistical_updater : public bsta_mg_adaptive_updater<_gaussian>
{
  public:
    typedef bsta_num_obs<_gaussian> _obs_gaussian;
    typedef bsta_mixture<_obs_gaussian> _mix_dist;
    typedef bsta_num_obs<_mix_dist> _obs_mix_dist;
    typedef typename _gaussian::math_type T;
    typedef typename _gaussian::vector_type _vector;

    enum { data_dimension = _gaussian::dimension };

    //: Constructor
    bsta_mg_grimson_statistical_updater(const _gaussian& model,
                                          unsigned int max_cmp = 5,
                                          T g_thresh = T(3),
                                          T min_stdev = T(0) )
      : bsta_mg_adaptive_updater<_gaussian>(model, max_cmp),
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
template <class _gaussian>
class bsta_mg_grimson_window_updater : public bsta_mg_grimson_statistical_updater<_gaussian>
{
  public:
    typedef bsta_num_obs<_gaussian> _obs_gaussian;
    typedef bsta_mixture<_obs_gaussian> _mix_dist;
    typedef bsta_num_obs<_mix_dist> _obs_mix_dist;
    typedef typename _gaussian::math_type T;
    typedef typename _gaussian::vector_type _vector;

    enum { data_dimension = _gaussian::dimension };

    //: Constructor
    bsta_mg_grimson_window_updater(const _gaussian& model,
                                     unsigned int max_cmp = 5,
                                     T g_thresh = T(3),
                                     T min_stdev = T(0),
                                     unsigned int window_size = 40) 
      : bsta_mg_grimson_statistical_updater<_gaussian>(model, max_cmp, g_thresh, min_stdev),
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

/*template <class _updater, class _gaussian, unsigned n>
class bsta_updater_value_threshold_wrapper
{
  public:
 public:
    typedef bsta_num_obs<_gaussian> _obs_gaussian;
    typedef bsta_mixture<_obs_gaussian> _mix_dist;
    typedef bsta_num_obs<_mix_dist> _obs_mix_dist;
    typedef typename _gaussian::math_type T;
    typedef typename _gaussian::vector_type _vector;

    enum { data_dimension = _gaussian::dimension };

   
    //: Constructor
    bsta_updater_value_threshold_wrapper(_updater &updater,T intensitythresh)
    {
        ithresh_=intensitythresh;
        updater_=&updater;
    }

    //: The main function
    void operator() ( _obs_mix_dist& mix, const _vector& sample ) const
    {
        _vector::iterator iter=sample.begin();
        bool flag=false;
        for(;iter!=sample.end();iter++)
            if(*iter>ithresh_)
                flag=true;
        if(flag)
            *updater_(mix,sample);
    }
  
  protected:
    _vector ithresh_;
    _updater updater_;
};*/
//template <class _updater, class _gaussian>
//class bsta_updater_value_threshold_wrapper<_updater,_gaussian,1>
//{
//  public:
// public:
//    typedef bsta_num_obs<_gaussian> _obs_gaussian;
//    typedef bsta_mixture<_obs_gaussian> _mix_dist;
//    typedef bsta_num_obs<_mix_dist> _obs_mix_dist;
//    typedef typename _gaussian::math_type T;
//    typedef typename _gaussian::vector_type _vector;
//
//    enum { data_dimension = _gaussian::dimension };
//
//   
//    //: Constructor
//    bsta_updater_value_threshold_wrapper(_updater &updater,T  intensitythresh)
//    {
//        ithresh_=intensitythresh;
//        updater_=&updater;
//    }
//
//    //: The main function
//    void operator() ( _obs_mix_dist& mix, const T& sample ) const
//    {
//        if(sample>ithresh_)
//            (*updater_)(mix,sample);
//    }
//  
//  protected:
//    T ithresh_;
//    _updater *updater_;
//};
//
#endif // bsta_adaptive_updater_h_
