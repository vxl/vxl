// This is core/vpdl/vpdt/vpdt_update_mog.h
#ifndef vpdt_update_mog_h_
#define vpdt_update_mog_h_
//:
// \file
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date March 8, 2009
// \brief Iterative updating of a mixture of Gaussians

#include <utility>
#include <vpdl/vpdt/vpdt_field_traits.h>
#include <vpdl/vpdt/vpdt_gaussian.h>
#include <vpdl/vpdt/vpdt_mixture_of.h>
#include <vpdl/vpdt/vpdt_update_gaussian.h>
#include <vpdl/vpdt/vpdt_mog_fitness.h>
#include <vpdl/vpdt/vpdt_num_obs.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

//: A mixture of Gaussians updater
//  Base class for common functionality in adaptive updating schemes
template <class mog_type>
class vpdt_mog_updater
{
 public:
  typedef mog_type distribution_type;
  typedef typename mog_type::field_type field_type;

 private:
  typedef typename mog_type::component_type gaussian_type;
  typedef typename gaussian_type::field_type F;
  typedef typename vpdt_field_traits<F>::scalar_type T;

 protected:
  //: Constructor
  vpdt_mog_updater(const gaussian_type& init_gaussian,
                   unsigned int max_cmp = 5)
  : init_gaussian_(init_gaussian), max_components_(max_cmp) {}

  //: insert a sample in the mixture
  // \param sample A Gaussian is inserted with a mean of \a sample and a
  //               covariance from \a init_gaussian_
  // \param init_weight The normalized weight the resulting sample
  //                    should have after insertion
  void insert(mog_type& mixture, const F& sample, T init_weight) const
  {
    // Ensure that the number of components does not exceed the maximum.
    // Remove the last component if necessary to make room for the new one.
    // Components should be sorted such that the last component is least important.
    while (mixture.num_components() >= max_components_)
    {
      mixture.remove_last();
    }

    // this correction accounts for the fact that the remaining components
    // are not normalized to 1-init_weight
    if (mixture.num_components() > 0)
      init_weight /= (1-init_weight)*mixture.norm_const();
    else
      init_weight = T(1);

    // the mean is moved to the sample point (initial covariance is fixed)
    init_gaussian_.mean = sample;
    mixture.insert(init_gaussian_,init_weight);
  }

  //: Return the index of the first Gaussian within the threshold distance
  //  The threshold \a gt2 is on the square distance.
  //  The computed square distance is returned by reference in \a sqr_dist
  //  If there are no matches return the number of components (last index + 1)
  unsigned int match( const mog_type& mix, const F& sample,
                      const T& gt2, T& sqr_dist ) const
  {
    const unsigned int mix_nc = mix.num_components();
    for (unsigned int i=0; i<mix_nc; ++i) {
      const gaussian_type& g = mix.distribution(i);
      sqr_dist = g.sqr_mahal_dist(sample);
      if (sqr_dist < gt2)
        return i;
    }
    return mix_nc;
  }

  //: A model for new Gaussians inserted
  mutable gaussian_type init_gaussian_;
  //: The maximum number of components in the mixture
  unsigned int max_components_;
};


//: A mixture of Gaussians adaptive updater based on Stauffer-Grimson.
//  Using the S-G approximation to prior probabilities
// This algorithm is based on: C. Stauffer and W.E.L. Grimson,
// "Adaptive background mixture models for real-time tracking", CVPR 1999
template <class mog_type>
class vpdt_mog_sg_updater : public vpdt_mog_updater<mog_type>
{
 public:
  typedef typename mog_type::component_type gaussian_type;
  typedef typename gaussian_type::field_type F;
  typedef typename vpdt_field_traits<F>::scalar_type T;

  //: Constructor
  vpdt_mog_sg_updater(const gaussian_type& init_gaussian,
                      unsigned int max_cmp = 5,
                      T g_thresh = T(2.5),
                      T alpha = T(0.1),
                      T init_weight = T(0.1),
                      T min_stdev = T(0.16) )
  : vpdt_mog_updater<mog_type>(init_gaussian, max_cmp),
    gt2_(g_thresh*g_thresh), alpha_(alpha), init_weight_(init_weight),
    min_var_(min_stdev*min_stdev) {}

  //: The main function
  void operator() ( mog_type& mix, const F& sample ) const
  {
    this->update(mix, sample, alpha_);
  }

 protected:

  //: Update the mixture with \a sample using learning rate \a alpha
  void update( mog_type& mix, const F& sample, T alpha ) const
  {
    T sqr_dist;
    unsigned int i = vpdt_mog_updater<mog_type>::match(mix,sample,gt2_,sqr_dist);
    if (i<mix.num_components())
    {
      gaussian_type& g = mix.distribution(i);
      // unlike the original paper, the normal distribution here is unnormalized
      // if normalized, rho can exceed 1 leading to divergence
      T rho = alpha * std::exp(-sqr_dist/2);
      if (min_var_ > T(0))
        vpdt_update_gaussian(g, rho, sample, min_var_);
      else
        vpdt_update_gaussian(g, rho, sample);
      // this is equivalent to
      // w_j = (1-alpha)*w_j + alpha*(j==i?1:0) for all j, but without normalization
      mix.set_weight(i, mix.weight(i) + alpha/((1-alpha)*mix.norm_const()));
    }
    else
    {
      vpdt_mog_updater<mog_type>::insert(mix,sample,init_weight_);
    }
    mix.sort(vpdt_mog_fitness<gaussian_type>::order);
  }

  //: Squared Gaussian Mahalanobis distance threshold
  T gt2_;
  //: The learning rate
  T alpha_;
  //: The initial weight for added Gaussians
  T init_weight_;
  //: Minimum variance allowed in each Gaussian component
  T min_var_;
};


//: A mixture of Gaussians adaptive updater based on D.-S. Lee.
// Modification of the S-G method.  Each Gaussian has its own learning rate
// that adjusts with the number of observations.
// This algorithm requires that the Gaussians in the mixture are wrapped
// in a vpdt_num_obs class to count the number of observations.
// This algorithm is based on: D.-S. Lee.
// "Effective gaussian mixture learning for video background subtraction",
// PAMI, 27:827--832, May 2005.
template <class mog_type>
class vpdt_mog_lee_updater : public vpdt_mog_updater<mog_type>
{
 public:
  typedef typename mog_type::component_type gaussian_type;
  typedef typename gaussian_type::field_type F;
  typedef typename vpdt_field_traits<F>::scalar_type T;

  //: Constructor
  vpdt_mog_lee_updater(const gaussian_type& init_gaussian,
                       unsigned int max_cmp = 5,
                       T g_thresh = T(2.5),
                       T min_stdev = T(0.16),
                       bool use_wta = false )
  : vpdt_mog_updater<mog_type>(init_gaussian, max_cmp),
  gt2_(g_thresh*g_thresh), min_var_(min_stdev*min_stdev),
  use_winner_take_all_(use_wta) {}

  //: The main function
  void operator() ( mog_type& mix, const F& sample ) const
  {
    T num_obs = total_num_obs(mix) + 1;
    this->update(mix, sample, T(1)/num_obs);
  }

 protected:

  //: Count the total number of observations in all components
  T total_num_obs( const mog_type& mix ) const
  {
    T num_obs = T(0);
    const unsigned int mix_nc = mix.num_components();
    for (unsigned int i=0; i<mix_nc; ++i) {
      const gaussian_type& g = mix.distribution(i);
      num_obs += g.num_observations;
    }
    return num_obs;
  }

  //: find all matches within the \c gt2_ threshold and compute the probability of each
  std::vector<std::pair<unsigned int,double> >
  matches(const mog_type& mix, const F& sample) const
  {
    const unsigned int mix_nc = mix.num_components();
    double sum_p = 0.0;
    std::vector<std::pair<unsigned int,double> > matchez;
    // find the square distance to all components, count those below gt2_
    for (unsigned int i=0; i<mix_nc; ++i) {
      const gaussian_type& g = mix.distribution(i);
      double sqr_dist = g.sqr_mahal_dist(sample);
      if (sqr_dist < gt2_)
        matchez.push_back(std::pair<unsigned int,double>(i,sqr_dist));
    }
    // if only one match, it has prob 1
    if (matchez.size() == 1) {
      matchez[0].second = 1.0;
    }
    // find the probability of each match
    else if (matchez.size() > 1) {
      for (unsigned int j=0; j<matchez.size(); ++j) {
        unsigned int& i = matchez[j].first;
        double& p = matchez[j].second;
        const gaussian_type& g = mix.distribution(i);
        p = mix.weight(i) * g.norm_const() * std::exp(-p/2);
        sum_p += p;
      }
      // normalize
      for (unsigned int j=0; j<matchez.size(); ++j) {
        matchez[j].second /= sum_p;
      }
    }
    return matchez;
  }

  //: Apply a winner-take-all strategy to the matches.
  //  Keep only the highest probability match and assign it probability 1
  void winner_take_all(std::vector<std::pair<unsigned int,double> >& m) const
  {
    double max_p = m[0].second;
    unsigned int max_j = 0;
    for (unsigned int j=1; j<m.size(); ++j) {
      if (m[j].second > max_p) {
        max_p = m[j].second;
        max_j = j;
      }
    }
    m[0].first = m[max_j].first;
    m[0].second = 1.0;
    m.resize(1);
  }

  //: Update the mixture with \a sample using learning rate \a alpha
  void update( mog_type& mix, const F& sample, T alpha ) const
  {
    const unsigned int mix_nc = mix.num_components();
    if (mix_nc == 0) {
      insert(mix,sample,alpha);
      return;
    }
    // find all matching components (sqr dist < gt2_) and their probabilites
    std::vector<std::pair<unsigned int,double> > m = matches(mix, sample);

    if (!m.empty())
    {
      if (use_winner_take_all_ && m.size() > 1)
        winner_take_all(m);
      T w_inc = alpha / ((T(1)-alpha)*mix.norm_const());
      for (unsigned int j=0; j<m.size(); ++j) {
        unsigned int i = m[j].first;
        double p = m[j].second;
        gaussian_type& g = mix.distribution(i);
        g.num_observations += T(p);
        T rho = (T(1)-alpha)/g.num_observations + alpha;
        rho *= p;
        if (min_var_ > T(0))
          vpdt_update_gaussian(g, rho, sample, min_var_);
        else
          vpdt_update_gaussian(g, rho, sample);
        // this is equivalent to
        // w_j = (1-alpha)*w_j + alpha*(j==i?1:0) for all j, but without normalization
        mix.set_weight(i, mix.weight(i)+p*w_inc);
      }
    }
    else
    {
      insert(mix,sample,alpha);
    }
    mix.sort(vpdt_mog_fitness<gaussian_type>::order);
  }

  //: Squared Gaussian Mahalanobis distance threshold
  T gt2_;
  //: Minimum variance allowed in each Gaussian component
  T min_var_;

  //: Use a winner-take_all strategy
  bool use_winner_take_all_;
};


//: A mixture of Gaussians adaptive updater base on Leotta-Mundy
// Combines the greedy matching of the S-G method for speed with the
// dynamic learning rate of Lee for faster learning.
// Unnormalized weights serve a dual role with observation counts.
// This algorithm is based on: M. Leotta and J. Mundy,
// "Learning background and shadow appearance with 3-d vehicle models",
// BMVC, 2:649--658, September 2006.
template <class mog_type>
class vpdt_mog_lm_updater : public vpdt_mog_updater<mog_type>
{
 public:
  typedef typename mog_type::component_type gaussian_type;
  typedef typename gaussian_type::field_type F;
  typedef typename vpdt_field_traits<F>::scalar_type T;

  //: Constructor
  vpdt_mog_lm_updater(const gaussian_type& init_gaussian,
                      unsigned int max_cmp = 5,
                      T g_thresh = T(2.5),
                      T min_stdev = T(0.16),
                      unsigned int ws = 300)
  : vpdt_mog_updater<mog_type>(init_gaussian, max_cmp),
  gt2_(g_thresh*g_thresh), min_var_(min_stdev*min_stdev),
  window_size_(ws) {}

  //: The main function
  void operator() ( mog_type& mix, const F& sample, T sample_weight = T(1) ) const
  {
    this->update(mix, sample, sample_weight);
  }

 protected:

  //: Count the total mixture weight
  T total_weight( const mog_type& mix ) const
  {
    T tw = T(0);
    const unsigned int mix_nc = mix.num_components();
    for (unsigned int i=0; i<mix_nc; ++i) {
      tw += mix.weight(i);
    }
    return tw;
  }

  //: Update the mixture with \a sample using learning rate \a alpha
  void update( mog_type& mix, const F& sample, T samp_weight ) const
  {
    assert(samp_weight > 0.0);
    T tw = total_weight(mix);
    tw += samp_weight;

    T alpha = 1/tw;
    T sqr_dist;
    unsigned int i = this->match(mix,sample,gt2_,sqr_dist);
    if (i<mix.num_components())
    {
      gaussian_type& g = mix.distribution(i);
      T w = mix.weight(i);
      w += samp_weight;
      T rho = (T(1)-alpha)/w + alpha;
      if (min_var_ > T(0))
        vpdt_update_gaussian(g, rho, sample, min_var_);
      else
        vpdt_update_gaussian(g, rho, sample);
      mix.set_weight(i,w);
    }
    else
    {
      this->insert(mix,sample,alpha);
    }
    // scale down weights for a moving window effect
    if (tw > window_size_) {
      T scale = window_size_ / tw;
      const unsigned int mix_nc = mix.num_components();
      for (unsigned int i=0; i<mix_nc; ++i) {
        mix.set_weight(i, mix.weight(i)*scale);
      }
    }
    mix.sort(vpdt_mog_fitness<gaussian_type>::order);
  }

  //: Squared Gaussian Mahalanobis distance threshold
  T gt2_;
  //: Minimum variance allowed in each Gaussian component
  T min_var_;
  //: The moving window size in number of frames
  unsigned int window_size_;
};


#endif // vpdt_update_mog_h_
