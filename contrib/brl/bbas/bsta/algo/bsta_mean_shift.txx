// This is brl/bbas/bsta/algo/bsta_mean_shift.txx
#ifndef bsta_mean_shift_txx_
#define bsta_mean_shift_txx_
//:
// \file
#include "bsta_mean_shift.h"

//: Compute the mean in a window around the given pt, the window size is the bandwidth
//  if there are no points within bandwidth of the input pt, return false
template <class T, unsigned n>
bool
bsta_mean_shift_sample_set<T,n>::mean(typename bsta_parzen_sphere<T,n>::vector_type const& pt, typename bsta_parzen_sphere<T,n>::vector_type& out)
{
  typedef typename bsta_distribution<T,n>::vector_type vect_t;
  typedef typename vcl_vector<vect_t >::const_iterator sit_t;
  typedef typename vcl_vector<T >::const_iterator wit_t;

  if (samples_.size() != weights_.size()) {
    weights_.clear();
    weights_ = vcl_vector<T>(samples_.size(), T(1.0));
  }

  vect_t sum(T(0));
  sit_t sit = bsta_parzen<T,n>::samples_.begin();
  wit_t wit = weights_.begin();
  T nsamp = 0;
  for (; sit != bsta_parzen<T,n>::samples_.end(); ++sit, ++wit) {

    vect_t s = *sit;
    vect_t dif = s-pt;
    vnl_vector_fixed<T,n> dummy(dif);
    T d = dummy.magnitude();
    if (d < bandwidth_) { // this sample is within window of the given point, use it to calculate mean
      sum += (*wit)*(*sit);
      nsamp += (*wit);
    }
  }
  if (nsamp > 0) {
    out = sum / nsamp;
    return true;
  }
    
  return false;
}

//: Insert a weighted sample into the distribution
template <class T, unsigned n>
void 
bsta_mean_shift_sample_set<T,n>::insert_w_sample(typename bsta_parzen_sphere<T,n>::vector_type const& sample, T weight)  
{ 
  bsta_parzen<T,n>::samples_.push_back(sample); 
  weights_.push_back(weight); 
}

//: one may need to normalize the weights after the insertion is over
template <class T, unsigned n>
void 
bsta_mean_shift_sample_set<T,n>::normalize_weights() 
{
  T sum(0);
  for (unsigned i = 0; i < weights_.size(); i++) {
    sum += weights_[i];
  }

  if (sum > T(0)) {
    for (unsigned i = 0; i < weights_.size(); i++) {
      weights_[i] = weights_[i]/sum;
    }
  }

}
  

template <class T, unsigned n>
bool bsta_mean_shift<T,n>::find_modes(bsta_mean_shift_sample_set<T,n>& set, vnl_random & rng, float percentage, T epsilon)
{
  typedef typename bsta_distribution<T,n>::vector_type vect_t;

  //: initialize seeds by picking given percentage of the sample set randomly
  int size = set.size();
  int seed_size = (int)vcl_ceil(percentage*size/100.0f);
  vcl_cout << "size: " << size << " seed_size: " << seed_size << vcl_endl;

  if (!assignment_.size()) {
    vcl_pair<bool, unsigned> p(false, 0);
    assignment_ = vcl_vector<vcl_pair<bool, unsigned> >(size, p);
  }

  vcl_cout << "initialized assignment_, its size: " << assignment_.size()  << vcl_endl;

  for (int i = 0; i < seed_size; i++) {
    //: randomly pick one of the samples as seed
    double rn = rng.drand32();
    int s_id = (int)vcl_floor(rn*(size-1)+0.5f);
    if (assignment_[s_id].first) {
      i--;
      continue;
    }
    vnl_vector_fixed<T,n> dif(T(10e4));
    
    vect_t current = set.sample(s_id);
    
    unsigned cnt = 0;
    while (dif.magnitude() > epsilon) {
      vect_t mean;
      if (set.mean(current, mean)) {
        vect_t v_dif = mean-current;
        dif = vnl_vector_fixed<T,n>(v_dif);
        current = mean;
      }
      cnt++;
      if (cnt > max_iter_)
        break;
    }
    if (cnt < max_iter_) {
      //: found a stable mode
      modes_.push_back(current);
      vcl_pair<bool, unsigned> p(true, modes_.size()-1);
      assignment_[s_id] = p;
    }
  }

  return true;
}

//: use all the samples to get its mode, no need for random seed picking
template <class T, unsigned n>
bool bsta_mean_shift<T,n>::find_modes(bsta_mean_shift_sample_set<T,n>& set, T epsilon = 10e-3)
{
  typedef typename bsta_distribution<T,n>::vector_type vect_t;

  int size = set.size();

  if (!assignment_.size()) {
    vcl_pair<bool, unsigned> p(false, 0);
    assignment_ = vcl_vector<vcl_pair<bool, unsigned> >(size, p);
  }

  for (int i = 0; i < size; i++) {
    
    vnl_vector_fixed<T,n> dif(T(10e4));
    
    vect_t current = set.sample(i);
    
    unsigned cnt = 0;
    while (dif.magnitude() > epsilon) {
      vect_t mean;
      if (set.mean(current, mean)) {
        vect_t v_dif = mean-current;
        dif = vnl_vector_fixed<T,n>(v_dif);
        current = mean;
      }
      cnt++;
      if (cnt > max_iter_)
        break;
    }
    if (cnt < max_iter_) {
      //: found a stable mode
      modes_.push_back(current);
      vcl_pair<bool, unsigned> p(true, modes_.size()-1);
      assignment_[i] = p;
    }
  }

  return true;
}

template <class T, unsigned n>
bool bsta_mean_shift<T,n>::trim_modes(T epsilon)
{
  typedef typename bsta_distribution<T,n>::vector_type vect_t;

  if (!modes_.size())
    return false;

  vcl_vector<bool> trimmed(modes_.size(), false);
  for (unsigned i = 0; i < modes_.size(); i++) {
    if (trimmed[i])
      continue;

    vect_t v_i = modes_[i];
    for (unsigned j = i+1; j < modes_.size(); j++) {
      if (trimmed[j])
        continue;
      vect_t v_j = modes_[j];
      vect_t dif = v_i - v_j;
      vnl_vector_fixed<T,n> v_dif(dif);
      if (v_dif.magnitude() < epsilon) {
        trimmed[j] = true;
        //: assign all the samples of the trimmed mode to this new mode v_i
        for (unsigned kk = 0; kk < assignment_.size(); kk++) {
          if (assignment_[kk].first && assignment_[kk].second == j) {
            assignment_[kk].second = i;
          }
        }
      }
    }
  }
  vcl_vector<vect_t > new_modes;
  for (unsigned i = 0; i < modes_.size(); i++) {
    if (!trimmed[i]) {
      new_modes.push_back(modes_[i]);
      for (unsigned jj = 0; jj < assignment_.size(); jj++) {
        if (assignment_[jj].first && assignment_[jj].second == i)
          assignment_[jj].second = new_modes.size()-1;
      }
    }
  }

  modes_.clear();
  modes_ = new_modes;
  return true;
}

template <class T, unsigned n>
bool bsta_mean_shift<T,n>::recompute_modes(bsta_mean_shift_sample_set<T,n>& set)
{
  typedef typename bsta_distribution<T,n>::vector_type vect_t;

  for (unsigned i = 0; i < modes_.size(); i++) {
    vect_t sum(T(0));
    T cnt = T(0);
    for (unsigned jj = 0; jj < assignment_.size(); jj++) {
      if (assignment_[jj].first && assignment_[jj].second == i) {
        T w = set.weight(jj);
        sum += w*set.sample(jj);
        cnt += w;
      }
    }
    if (cnt == 0) 
      return false;
    modes_[i] = sum/T(cnt);
  }
  return true;
}



#define BSTA_MEAN_SHIFT_SAMPLE_SET_INSTANTIATE(T,n) \
template class bsta_mean_shift_sample_set<T,n >; 

#define BSTA_MEAN_SHIFT_INSTANTIATE(T,n) \
template class bsta_mean_shift<T,n >; 


#endif // bsta_mean_shift_txx_
