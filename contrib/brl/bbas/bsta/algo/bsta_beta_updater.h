// This is brl/bbas/bsta/algo/bsta_beta_updater.h
#ifndef bsta_beta_updater_h_
#define bsta_beta_updater_h_
//:
// \file
// \brief  Iterative updating of beta distribution
// \author Gamze Tunali (gtunali@brown.edu)
// \date   Nov 17, 2009
//
// In this implementation $\alpha>=1$ and $\beta>=1$.
// In order to ensure this
// $ \mu(\mu(1-\mu)/var-1)>1 $ and
// $ (1-\mu)(\mu(1-\mu)/var-1)>1 $
//
// The distance of beta distribution is given as
// $$  -(\alpha-1)log(x/\mu)-(\beta-1)\log((1-x)/(1-\mu)) > 3  $$
//
// \verbatim
//  Modifications
// \endverbatim

#include <iostream>
#include <algorithm>
#include <bsta/bsta_beta.h>
#include <bsta/bsta_attributes.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Update the statistics given a 1D beta distribution and a learning rate
// \note if rho = 1/(num observations) then this is just an online cumulative average
template <class T>
void bsta_update_beta(bsta_beta<T>& beta_dist, T rho, const T& sample )
{
  // the complement of rho (i.e. rho+rho_comp=1)
  T rho_comp = 1 - rho;
  T old_mean;
#if 0
  if (beta_dist.alpha()<1)
    old_mean=2*(beta_dist.alpha()-0.5);
  else if (beta_dist.beta()<1)
    old_mean=1-2*(beta_dist.beta()-0.5);
  else
#endif // 0
    old_mean = beta_dist.mean();

  T diff = sample - old_mean;
  T new_var = rho_comp * beta_dist.var();
  new_var += (rho * rho_comp) * diff*diff;

  T new_mean = (old_mean) +  (rho * diff);

  T alpha,beta;
  if (!bsta_beta<T>::bsta_beta_from_moments(new_mean,new_var,alpha,beta))
    return;
  //T t = (new_mean*(1-new_mean)/new_var)-1;
  //T alpha=new_mean*t;
  //T beta=(1-new_mean)*t;

  if (alpha<1 && beta <1)
    std::cout<<"Mean : "<<new_mean<< "  Var: "<<new_var<<'\n';
  beta_dist.set_alpha_beta(alpha, beta);
}

template <class T>
void bsta_update_beta(bsta_beta<T>& beta_dist, T rho, const T& sample , const T & min_var)
{
  // the complement of rho (i.e. rho+rho_comp=1.0)
  T rho_comp = 1.0f - rho;

  T old_mean;
  if (beta_dist.alpha()<1)
    old_mean=2*(beta_dist.alpha()-0.5);
  else if (beta_dist.beta()<1)
    old_mean=1-2*(beta_dist.beta()-0.5);
  else
    old_mean = beta_dist.mean();

  T diff = sample - old_mean;
  T new_var = rho_comp * beta_dist.var();
  new_var += (rho * rho_comp) * diff*diff;

  if (min_var > new_var) new_var=min_var;
  T new_mean = (old_mean) +  (rho * diff);

  T alpha,beta;
  if (!bsta_beta<T>::bsta_beta_from_moments(new_mean,new_var,alpha,beta))
    return;

  beta_dist.set_alpha_beta(alpha, beta);
}

template <class beta_>
struct bsta_beta_fitness
{
 private:
  typedef typename beta_::math_type T;
  enum { n = beta_::dimension };
 public:
  static bool order (const beta_& /*d1*/, const T& w1,
                     const beta_& /*d2*/, const T& w2)
  {
    return w1>w2;
  }
};

//: An updater for statistically updating beta distributions
template <class beta_>
class bsta_beta_updater
{
  typedef bsta_num_obs<beta_> obs_beta_;
  typedef typename beta_::math_type T;
  typedef typename beta_::vector_type vector_;
 public:

  //: for compatibility with vpdl/vpdt
  typedef typename beta_::field_type field_type;
  typedef beta_ distribution_type;


  //: The main function
  // make the appropriate type casts and call a helper function
  void operator() ( obs_beta_& d, const vector_& sample ) const
  {
    d.num_observations += T(1);
    bsta_update_beta(d, T(1)/d.num_observations, sample);
  }
};

template <class mix_dist_>
class bsta_mix_beta_updater
{
  typedef typename mix_dist_::dist_type obs_dist_; //mixture comp type
  typedef typename obs_dist_::contained_type dist_; //num_obs parent
  typedef typename dist_::math_type T;//the field type, e.g. float
  typedef typename dist_::vector_type vector_;// the vector type
  typedef bsta_num_obs<mix_dist_> obs_mix_dist_;

 public:
  //: Constructor
  bsta_mix_beta_updater(const dist_& model, T thresh,  T var, unsigned int max_cmp = 5)
   : init_dist_(model,T(1)), max_components_(max_cmp), p_thresh_(thresh), var_(var) {}

  //: for compatibility with vpdl/vpdt
  typedef typename dist_::field_type field_type;
  typedef mix_dist_ distribution_type;

  //: The main function
  void operator() ( obs_mix_dist_& mix, const vector_& sample) const
  {
    mix.num_observations += T(1);
    this->update(mix, sample, T(1)/mix.num_observations);
  }

  void update( mix_dist_& mix, const vector_& sample, T alpha ) const;

 protected:

  //: insert a sample in the mixture
  void insert(mix_dist_& mixture, const vector_& sample, T init_weight) const
  {
    bool removed = mixture.num_components() >= max_components_;
    while (mixture.num_components() >= max_components_)
    {
      mixture.remove_last();
    }

    // if a mixture is removed renormalize the rest
    if (removed) {
      T adjust = T(0);
      for (unsigned int i=0; i<mixture.num_components(); ++i)
        adjust += mixture.weight(i);
      adjust = (T(1)-init_weight) / adjust;
      for (unsigned int i=0; i<mixture.num_components(); ++i)
        mixture.set_weight(i, mixture.weight(i)*adjust);
    }

    //T var = T(0.05);
#if 0
    T t = (sample*(1-sample)/var_)-1;
    T alpha=sample*t;
    T beta=(1-sample)*t;
    init_dist_.set_alpha_beta(alpha,beta); ///??? this was setting mean
#endif
    //T lower = T(0.5-std::sqrt(1-4*var_)/2);
    //T upper = T(0.5+std::sqrt(1-4*var_)/2);

    //vector_ val = sample;
    //if (sample < lower)
    //    val = lower+T(1e-6);
    //else if (sample > upper)
    //    val = upper-T(1e-6);

    T alpha, beta;
    bsta_beta<T>::bsta_beta_from_moments(sample, var_,alpha, beta);
    init_dist_.set_alpha_beta(alpha,beta);
    mixture.insert(init_dist_,init_weight);
  }

  //: A model for new beta inserted
  mutable obs_dist_ init_dist_;
  //: The maximum number of components in the mixture
  unsigned int max_components_;
  //: probability threshold
  T p_thresh_;
  T var_;
};

#endif
