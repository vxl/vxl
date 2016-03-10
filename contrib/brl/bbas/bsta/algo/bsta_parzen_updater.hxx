// This is brl/bbas/bsta/algo/bsta_parzen_updater.hxx
#ifndef bsta_parzen_updater_hxx_
#define bsta_parzen_updater_hxx_
//:
// \file
#include "bsta_parzen_updater.h"

#if 0 // needs to be restructured in light of scalar samples
//: The main function
template <class parzen_dist_>
void bsta_parzen_updater<parzen_dist_>::
operator() ( parzen_dist_& pdist, const typename bsta_parzen_updater<parzen_dist_>::vector_& sample) const
{
  typedef typename parzen_dist_::math_type T;
  unsigned n = pdist.size();
  T min_dist = vnl_numeric_traits<T>::maxval;
  unsigned closest_index = 0; // initialise to avoid compiler warning
  for (unsigned i = 0; i<n; ++i)
  {
    vector_ s = pdist.sample(i);
    vector_ dif = sample-s;
    T mag = dif.magnitude();
    if (mag<tol_) // no need to insert a sample within tol of and
      return;    // existing sample
    if (mag<min_dist)
    {
      min_dist = mag;
      closest_index = i;
    }
  }
  if (n==max_samples_) // replace closest parzen sample with new sample
    pdist.remove_sample(closest_index);
  pdist.insert_sample(sample);
}

template <class parzen_dist_>
void bsta_parzen_adapt_bw_updater<parzen_dist_>::
operator() ( parzen_dist_& pdist,
             const typename bsta_parzen_adapt_bw_updater<parzen_dist_>::vector_& sample) const
{
  typedef typename parzen_dist_::math_type T;
  if (pdist.bandwidth_adapted())
    return;//don't update after bandwidth is set
  unsigned n = pdist.size();
  //set the bandwidth when max samples is reached
  if (n == max_samples_&&!pdist.bandwidth_adapted())
  {
    std::vector<vector_> all_samples = pdist.samples();
    // sort the samples. vless provides a descending
    // order based on the probability density of a sample, given
    // the Parzen distribution of the samples
    vless<T, data_dimension> pred(&pdist);
    std::sort(all_samples.begin(), all_samples.end(), pred);
    unsigned nback = static_cast<unsigned>(n*frac_background_);
    T fr = T(1)/static_cast<T>(nback);
    vector_ mean, xsq, mean_sq;
    mean.fill(T(0)), xsq.fill(T(0));
    for (unsigned i = 0; i<nback; ++i)
    {
      vector_ s = all_samples[i];
      mean += s;
      xsq += element_product(s,s);
    }
    mean_sq = element_product(mean,mean);
    vector_ var = fr*xsq - fr*fr*mean_sq;
    double max = 0;
    for (unsigned id = 0; id<data_dimension; ++id)
    {
      double v = static_cast<double>(var[id]);
      if (v>max)
        max = v;
    }
    double sd = std::sqrt(max);
    float bandwidth = static_cast<float>(2.65*sd*std::pow(double(nback), 0.2));
    pdist.set_bandwidth(bandwidth);
    pdist.set_bandwidth_adapted(true);
  }
  T min_dist = vnl_numeric_traits<T>::maxval;
  unsigned closest_index = 0; // initialise to avoid compiler warning
  for (unsigned i = 0; i<n; ++i)
  {
    vector_ s = pdist.sample(i);
    vector_ dif = sample-s;
    T mag = dif.magnitude();
    if (mag<tol_) // no need to insert a sample within tol of and
      return;    // existing sample
    if (mag<min_dist)
    {
      min_dist = mag;
      closest_index = i;
    }
  }
  if (n==max_samples_)//replace closest parzen sample with new sample
    pdist.remove_sample(closest_index);
  pdist.insert_sample(sample);
}
#else

template <class parzen_dist_>
void bsta_parzen_updater<parzen_dist_>::
operator() ( parzen_dist_&, const typename bsta_parzen_updater<parzen_dist_>::vector_&) const {}

template <class parzen_dist_>
void bsta_parzen_adapt_bw_updater<parzen_dist_>::
operator() ( parzen_dist_&, const typename bsta_parzen_adapt_bw_updater<parzen_dist_>::vector_&) const {}
#endif

#define BSTA_PARZEN_UPDATER_INSTANTIATE(T) \
template class bsta_parzen_updater<T >; \
template class bsta_parzen_adapt_bw_updater<T >

#endif // bsta_parzen_updater_hxx_
