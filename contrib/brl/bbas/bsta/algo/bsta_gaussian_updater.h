// This is brcv/seg/bsta/algo/bsta_gaussian_updater.h
#ifndef bsta_gaussian_updater_h_
#define bsta_gaussian_updater_h_

//:
// \file
// \brief Iterative updating of Gaussians
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 2/22/06
//
// \verbatim
//  Modifications
// \endverbatim

#include <bsta/bsta_gaussian_sphere.h>
#include <bsta/bsta_gaussian_indep.h>
#include <bsta/bsta_gaussian_full.h>
#include <bsta/bsta_gaussian_angles_1d.h>
#include <bsta/bsta_gaussian_x_y_theta.h>
#include <bsta/bsta_mixture.h>
#include <bsta/bsta_attributes.h>
#include <vcl_algorithm.h>

void
bsta_update_gaussian(bsta_gaussian_angles_1d & gaussian, float rho,
                       const float& sample,float min_var );


void
bsta_update_gaussian(bsta_gaussian_angles_1d & gaussian, float rho,
                       const float& sample);

void
bsta_update_gaussian(bsta_gaussian_x_y_theta & gaussian, float rho,
                       const vnl_vector_fixed<float,3>& sample,float min_var );


void
bsta_update_gaussian(bsta_gaussian_x_y_theta & gaussian, float rho,
                       const vnl_vector_fixed<float,3>& sample);

//: Update the statistics given a 1D Gaussian distribution and a learning rate
// \note if rho = 1/(num observations) then this just an online cumulative average
template <class T>
void bsta_update_gaussian(bsta_gaussian_sphere<T,1>& gaussian, T rho,
                            const T& sample )
{
  // the complement of rho (i.e. rho+rho_comp=1.0)
  T rho_comp = 1.0 - rho;
  // compute the updated mean
  const T& old_mean = gaussian.mean();

  T diff = sample - old_mean;
  T new_var = rho_comp * gaussian.var();
  new_var += (rho * rho_comp) * diff*diff;

  gaussian.set_var(new_var);
  gaussian.set_mean((old_mean) +  (rho * diff));
}


//: Update the statistics given a Gaussian distribution and a learning rate
// \note if rho = 1/(num observations) then this just an online cumulative average
template <class T, unsigned n>
void bsta_update_gaussian(bsta_gaussian_sphere<T,n>& gaussian, T rho,
                            const vnl_vector_fixed<T,n>& sample )
{
  // the complement of rho (i.e. rho+rho_comp=1.0)
  T rho_comp = 1.0 - rho;
  // compute the updated mean
  const vnl_vector_fixed<T,n>& old_mean = gaussian.mean();

  vnl_vector_fixed<T,n> diff(sample - old_mean);
  T new_var = rho_comp * gaussian.var();
  new_var += (rho * rho_comp) * dot_product(diff,diff);

  gaussian.set_var(new_var);
  gaussian.set_mean((old_mean) +  (rho * diff));
}


//: Update the statistics given a Gaussian distribution and a learning rate
// \note if rho = 1/(num observations) then this just an online cumulative average
template <class T, unsigned n>
void bsta_update_gaussian(bsta_gaussian_indep<T,n>& gaussian, T rho,
                            const vnl_vector_fixed<T,n>& sample )
{
  // the complement of rho (i.e. rho+rho_comp=1.0)
  T rho_comp = 1.0 - rho;
  // compute the updated mean
  const vnl_vector_fixed<T,n>& old_mean = gaussian.mean();

  vnl_vector_fixed<T,n> diff(sample - old_mean);

  vnl_vector_fixed<T,n> new_covar(rho_comp * gaussian.diag_covar());
  new_covar += (rho * rho_comp) * element_product(diff,diff);

  gaussian.set_covar(new_covar);
  gaussian.set_mean((old_mean) +  (rho * diff));
}


//: Update the statistics given a Gaussian distribution and a learning rate
// \note if rho = 1/(num observations) then this just an online cumulative average
template <class T, unsigned n>
void bsta_update_gaussian(bsta_gaussian_full<T,n>& gaussian, T rho,
                            const vnl_vector_fixed<T,n>& sample )
{
  // the complement of rho (i.e. rho+rho_comp=1.0)
  T rho_comp = 1.0 - rho;
  // compute the updated mean
  const vnl_vector_fixed<T,n>& old_mean = gaussian.mean();

  vnl_vector_fixed<T,n> diff(sample - old_mean);

  vnl_matrix_fixed<T,n,n> new_covar(rho_comp * gaussian.covar());
  new_covar += (rho * rho_comp) * outer_product(diff,diff);

  gaussian.set_covar(new_covar);
  gaussian.set_mean((old_mean) +  (rho * diff));
}



//-----------------------------------------------------------------------------
// The following versions allow for a lower limit on variances.
// If the same sample is observed repeatedly, the variances will
// converge to the minimum value parameter rather than zero.

//: Update the statistics given a 1D Gaussian distribution and a learning rate
// \param min_var forces the variance to stay above this limit
// \note if rho = 1/(num observations) then this just an online cumulative average
template <class T>
void bsta_update_gaussian(bsta_gaussian_sphere<T,1>& gaussian, T rho,
                            const T& sample, T min_var)
    {
  // the complement of rho (i.e. rho+rho_comp=1.0)
  T rho_comp = T(1) - rho;
  // compute the updated mean
  const T& old_mean = gaussian.mean();

  T diff = sample - old_mean;
  T new_var = rho_comp * gaussian.var();
  new_var += (rho * rho_comp) * vcl_max(diff*diff,min_var);
  gaussian.set_var(new_var);
  gaussian.set_mean((old_mean) +  (rho * diff));
}


//: Update the statistics given a Gaussian distribution and a learning rate
// \param min_var forces the variance to stay above this limit
// \note if rho = 1/(num observations) then this just an online cumulative average
template <class T, unsigned n>
void bsta_update_gaussian(bsta_gaussian_sphere<T,n>& gaussian, T rho,
                            const vnl_vector_fixed<T,n>& sample,
                            T min_var)
{
  // the complement of rho (i.e. rho+rho_comp=1.0)
  T rho_comp = (T)1 - rho;
  // compute the updated mean
  const vnl_vector_fixed<T,n>& old_mean = gaussian.mean();

  vnl_vector_fixed<T,n> diff(sample - old_mean);

  T new_var = rho_comp * gaussian.var();
  new_var += (rho * rho_comp) * vcl_max(dot_product(diff,diff),min_var);

  gaussian.set_var(new_var);
  gaussian.set_mean((old_mean) +  (rho * diff));
}



//: element-wise minimum of vector.
template <class T, unsigned n>
vnl_vector_fixed<T,n> element_max(const vnl_vector_fixed<T,n>& a_vector,
                                  const vnl_vector_fixed<T,n>& b_vector)
{
  vnl_vector_fixed<T,n> min_vector;
  T* r = min_vector.data_block();
  const T* a = a_vector.data_block();
  const T* b = b_vector.data_block();
  for(unsigned i=0; i<n; ++i, ++r, ++a, ++b)
    *r = vcl_max(*a,*b);
  return min_vector;
}


//: element-wise minimum of vector.
template <class T, unsigned n>
vnl_matrix_fixed<T,n,n> element_max(const vnl_matrix_fixed<T,n,n>& a_matrix,
                                    const vnl_matrix_fixed<T,n,n>& b_matrix)
{
  vnl_matrix_fixed<T,n,n> min_matrix;
  T* r = min_matrix.data_block();
  const T* a = a_matrix.data_block();
  const T* b = b_matrix.data_block();
  for(unsigned i=0; i<n*n; ++i, ++r, ++a, ++b)
    *r = vcl_max(*a,*b);
  return min_matrix;
}


//: Update the statistics given a Gaussian distribution and a learning rate
// \param min_vars forces the variances to stay above this limit
// \note if rho = 1/(num observations) then this just an online cumulative average
template <class T, unsigned n>
void bsta_update_gaussian(bsta_gaussian_indep<T,n>& gaussian, T rho,
                            const vnl_vector_fixed<T,n>& sample,
                            const vnl_vector_fixed<T,n>& min_vars)
{
  // the complement of rho (i.e. rho+rho_comp=1.0)
  T rho_comp = T(1) - rho;
  // compute the updated mean
  const vnl_vector_fixed<T,n>& old_mean = gaussian.mean();

  vnl_vector_fixed<T,n> diff(sample - old_mean);

  vnl_vector_fixed<T,n> new_covar(rho_comp * gaussian.diag_covar());
  new_covar += (rho * rho_comp) * element_max(element_product(diff,diff),min_vars);

  gaussian.set_covar(new_covar);
  gaussian.set_mean((old_mean) +  (rho * diff));
}

//: Update the statistics given a Gaussian distribution and a learning rate
// \param min_var forces all the variances to stay above this limit
// \note if rho = 1/(num observations) then this just an online cumulative average
template <class T, unsigned n>
void bsta_update_gaussian(bsta_gaussian_indep<T,n>& gaussian, T rho,
                            const vnl_vector_fixed<T,n>& sample,
                            T min_var)
{
  bsta_update_gaussian(gaussian,rho,sample,vnl_vector_fixed<T,n>(min_var));
}


//: Update the statistics given a Gaussian distribution and a learning rate
// \param min_covar forces the covariance to stay above this limit
// \note if rho = 1/(num observations) then this just an online cumulative average
template <class T, unsigned n>
void bsta_update_gaussian(bsta_gaussian_full<T,n>& gaussian, T rho,
                            const vnl_vector_fixed<T,n>& sample,
                            const vnl_matrix_fixed<T,n,n>& min_covar)
{
  // the complement of rho (i.e. rho+rho_comp=1.0)
  T rho_comp = T(1) - rho;
  // compute the updated mean
  const vnl_vector_fixed<T,n>& old_mean = gaussian.mean();

  vnl_vector_fixed<T,n> diff(sample - old_mean);

  vnl_matrix_fixed<T,n,n> new_covar(rho_comp * gaussian.covar());
  new_covar += (rho * rho_comp) * element_max(outer_product(diff,diff),min_covar);

  gaussian.set_covar(new_covar);
  gaussian.set_mean(( old_mean) +  (rho * diff));
}

//: Update the statistics given a Gaussian distribution and a learning rate
// \param min_var forces the diagonal covariance to stay above this limit
// \note if rho = 1/(num observations) then this just an online cumulative average
template <class T, unsigned n>
void bsta_update_gaussian(bsta_gaussian_full<T,n>& gaussian, T rho,
                            const vnl_vector_fixed<T,n>& sample,
                            T min_var)
{
  vnl_matrix_fixed<T,n,n> covar(T(0));
  for(unsigned i=0; i<n; ++i) covar(i,i) = min_var;
  bsta_update_gaussian(gaussian,rho,sample,covar);
}


//-----------------------------------------------------------------------------


//: An updater for statistically updating Gaussian distributions
template <class _gauss>
class bsta_gaussian_updater
{
  private:
    typedef bsta_num_obs<_gauss> _obs_gauss;
    typedef typename _gauss::math_type T;
    typedef vnl_vector_fixed<T,_gauss::dimension> _vector;
  public:

    //: The main function
    // make the appropriate type casts and call a helper function
    void operator() ( _obs_gauss& d, const _vector& sample ) const
    {
      d.num_observations += T(1);
      bsta_update_gaussian(d, T(1)/d.num_observations, sample);
    }
};


//: An updater for updating Gaussian distributions with a moving window
// When the number of samples exceeds the window size the most recent
// samples contribute more toward the distribution.
template <class _gauss>
class bsta_gaussian_window_updater
{
  private:
    typedef bsta_num_obs<_gauss> _obs_gauss;
    typedef typename _gauss::math_type T;
    typedef vnl_vector_fixed<T,_gauss::dimension> _vector;
  public:

    //: Constructor
    bsta_gaussian_window_updater(unsigned int ws) : window_size(ws) {}

    //: The main function
    // make the appropriate type casts and call a helper function
    void operator() ( _obs_gauss& d, const _vector& sample) const
    {
      if(d.num_observations < window_size)
        d.num_observations += T(1);
      bsta_update_gaussian(d, T(1)/d.num_observations, sample);
    }

    unsigned int window_size;
};



#endif // bsta_gaussian_updater_h_
