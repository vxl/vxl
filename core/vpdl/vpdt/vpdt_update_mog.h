// This is core/vpdl/vpdt/vpdt_update_mog.h
#ifndef vpdt_update_mog_h_
#define vpdt_update_mog_h_
//:
// \file
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date March 8, 2009
// \brief Iterative updating of a mixture of Gaussians
//
// \endverbatim


#include <vpdl/vpdt/vpdt_field_traits.h>
#include <vpdl/vpdt/vpdt_gaussian.h>
#include <vpdl/vpdt/vpdt_mixture_of.h>
#include <vpdl/vpdt/vpdt_update_gaussian.h>
#include <vpdl/vpdt/vpdt_mog_fitness.h>


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
    bool removed = false;
    while (mixture.num_components() >= max_components_)
    {
      mixture.remove_last();
      removed = true;
    }
    
    // this correction accounts for the fact that the remaining components
    // are not normalized to 1-init_weight
    if(mixture.num_components() > 0)
      init_weight /= (1-init_weight)*mixture.norm_const();
    else
      init_weight = T(1);
    
    // the mean is moved to the sample point (initial covariance is fixed)
    init_gaussian_.mean = sample;
    mixture.insert(init_gaussian_,init_weight);
  }
  
  //: A model for new Gaussians inserted
  mutable gaussian_type init_gaussian_;
  //: The maximum number of components in the mixture
  unsigned int max_components_;
};



//: A mixture of Gaussians Stauffer-Grimson adaptive updater
//  Using the S-G approximation to prior probablilities
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
                      T min_stdev = T(0) )
  : vpdt_mog_updater<mog_type>(init_gaussian, max_cmp),
    gt2_(g_thresh*g_thresh), alpha_(alpha), init_weight_(init_weight), 
    min_var_(min_stdev*min_stdev) {}
  
  //: The main function
  void operator() ( mog_type& mix, const F& sample ) const
  {
    this->update(mix, sample, alpha_);
  }
  
protected:
  
  //: Return the index of the first Gaussian within the threshold distance
  //  The distance is returned by reference in \a sqr_dist
  //  If there are no matches return the number of components (last index + 1)
  unsigned int match( mog_type& mix, const F& sample, T& sqr_dist ) const
  {
    unsigned int mix_nc = mix.num_components();
    for (unsigned int i=0; i<mix_nc; ++i) {
      gaussian_type& g = mix.distribution(i);
      sqr_dist = g.sqr_mahal_dist(sample);
      if (sqr_dist < gt2_)
        return i;
    }
    return mix_nc;
  }
  
  
  //: Update the mixture with \a sample using learning rate \a alpha
  void update( mog_type& mix, const F& sample, T alpha ) const
  {
    T sqr_dist;
    unsigned int i = match(mix,sample,sqr_dist);
    if (i<mix.num_components())
    {
      gaussian_type& g = mix.distribution(i);
      T rho = alpha * vcl_exp(-sqr_dist/2) * g.norm_const();
      if(min_var_ > T(0))
        vpdt_update_gaussian(g, rho, sample, min_var_);
      else
        vpdt_update_gaussian(g, rho, sample);
      // w_i /= 1-alpha for this i only is equivalent to 
      // w_j = (1-alpha)*w_j + alpha*(j==i?1:0) for all j, but without normalization
      mix.set_weight(i, mix.weight(i)/(1-alpha));
    }
    else
    {
      insert(mix,sample,init_weight_);
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


#endif // vpdt_update_mog_h_
