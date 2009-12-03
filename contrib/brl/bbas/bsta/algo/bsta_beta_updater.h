// This is brl/bbas/bsta/algo/bsta_beta_updater.h
#ifndef bsta_beta_updater_h_
#define bsta_beta_updater_h_
//:
// \file
// \brief  Iterative updating of beta distribution
// \author Gamze Tunali (gtunali@brown.edu)
// \date   Nov 17, 2009
//
// \verbatim
//  Modifications
// \endverbatim

#include <bsta/bsta_beta.h>
#include <bsta/bsta_attributes.h>
#include <vcl_algorithm.h>
#include <vcl_iostream.h>

//: Update the statistics given a 1D beta distribution and a learning rate
// \note if rho = 1/(num observations) then this just an online cumulative average
template <class T>
void bsta_update_beta(bsta_beta<T>& beta_dist, T rho, const T& sample )
{
  // the complement of rho (i.e. rho+rho_comp=1.0)
  T rho_comp = 1.0f - rho;
  // compute the updated mean
  const T& old_mean = beta_dist.mean();

  T diff = sample - old_mean;
  T new_var = rho_comp * beta_dist.var();
  new_var += (rho * rho_comp) * diff*diff;

  T new_mean = (old_mean) +  (rho * diff);

  T t = (new_mean*(1-new_mean)/new_var)-1;
  T alpha=new_mean*t;
  T beta=(1-new_mean)*t;
  beta_dist.set_alpha_beta(alpha, beta);
}

template <class beta_>
struct bsta_beta_fitness
{
 private:
  typedef typename beta_::math_type T;
  enum { n = beta_::dimension };
 public:
  static bool order (const beta_& d1, const T& w1,
                     const beta_& d2, const T& w2)
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
 private:
  typedef typename mix_dist_::dist_type obs_dist_; //mixture comp type
  typedef typename obs_dist_::contained_type dist_; //num_obs parent
  typedef typename dist_::math_type T;//the field type, e.g. float
  typedef typename dist_::vector_type vector_;// the vector type
  typedef bsta_num_obs<mix_dist_> obs_mix_dist_;


 public:
  //: Constructor
  bsta_mix_beta_updater(const dist_& model, T thresh,  T var, unsigned int max_cmp = 5)
   : init_dist_(model,T(1)), max_components_(max_cmp), p_thresh_(thresh), var_(var) {}

  //: for compatiblity with vpdl/vpdt
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
    T lower = (1.0-vcl_sqrt(1.0-4.0*var_))/2.0;
    T upper = (1.0+vcl_sqrt(1.0-4.0*var_))/2.0;

    vector_ val = sample;
    T alpha, beta;
    if (sample < lower)
      val = lower+0.000001;
    else if (sample > upper)
      val = upper-0.000001;

    bsta_beta<T>::bsta_beta_from_moments(val, var_,alpha, beta);
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
