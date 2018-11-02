// This is brl/bbas/bsta/algo/bsta_sample_set.hxx
#ifndef bsta_sample_set_hxx_
#define bsta_sample_set_hxx_
//:
// \file
#include <iostream>
#include <map>
#include "bsta_sample_set.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Compute the mean in a window around the given pt, the window size is the bandwidth
//  If there are no points within bandwidth of the input \a pt, return false
template <class T, unsigned n>
bool
bsta_sample_set<T,n>::mean(typename bsta_parzen_sphere<T,n>::vector_type const& pt, typename bsta_parzen_sphere<T,n>::vector_type& out)
{
  typedef typename bsta_parzen_sphere<T,n>::vector_type vect_t;
  typedef typename std::vector<vect_t >::const_iterator sit_t;
  typedef typename std::vector<T >::const_iterator wit_t;

  vect_t sum(T(0));
  sit_t sit = bsta_parzen<T,n>::samples_.begin();
  wit_t wit = weights_.begin();
  T nsamp = 0;
  for (; sit != bsta_parzen<T,n>::samples_.end(); ++sit, ++wit) {
    vect_t s = *sit;
    vect_t dif = s-pt;
    vnl_vector_fixed<T,n> dummy(dif);
    T d = dummy.magnitude();
    if (d < bsta_parzen_sphere<T,n>::bandwidth_) { // this sample is within window of the given point, use it to calculate mean
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

//: compute the mean of a particular assignment/mode/cluster
template <class T, unsigned n>
bool
bsta_sample_set<T,n>::mode_mean(int mode, vector_& out) const
{
  typedef typename bsta_parzen_sphere<T,n>::vector_type vect_t;
  typedef typename std::vector<vect_t >::const_iterator sit_t;
  typedef typename std::vector<T >::const_iterator wit_t;
  typedef typename std::vector<int >::const_iterator ait_t;

  if (bsta_parzen<T,n>::samples_.size() != weights_.size() || bsta_parzen<T,n>::samples_.size() != assignments_.size()) {
    std::cout << "Error in - bsta_sample_set<T,n>::mean() : assignments not initialized!\n";
    return false;
  }

  bool found_one = false;
  vect_t sum(T(0));
  sit_t sit = bsta_parzen<T,n>::samples_.begin();
  wit_t wit = weights_.begin();
  ait_t ait = assignments_.begin();
  T nsamp = 0;
  for (; sit != bsta_parzen<T,n>::samples_.end(); ++sit, ++wit, ++ait) {
    if (*ait != mode)
      continue;

    found_one = true;
    sum += (*wit)*(*sit);
    nsamp += (*wit);
  }
  if (found_one && nsamp > 0) {
    out = sum / nsamp;
    return true;
  }

  return false;
}

//: return number of assignments to this mode
template <class T, unsigned n>
int
bsta_sample_set<T,n>::mode_size(int mode) const
{
  typedef typename std::vector<int >::const_iterator ait_t;
  ait_t ait = assignments_.begin();
  int cnt = 0;
  for (; ait != assignments_.end(); ++ait) {
    if (*ait == mode)
      cnt++;
  }

  return cnt;
}

//: return total weight of assignments to this mode
template <class T, unsigned n>
T
bsta_sample_set<T,n>::mode_weight(int mode) const
{
  typedef typename std::vector<int >::const_iterator ait_t;
  typedef typename std::vector<T >::const_iterator wit_t;

  ait_t ait = assignments_.begin();
  wit_t wit = weights_.begin();

  T sum = T(0);
  for (; ait != assignments_.end(); ++ait, ++wit) {
    if (*ait == mode)
      sum += *wit;
  }

  return sum;
}

//: return total weight of all assignments
template <class T, unsigned n>
T
bsta_sample_set<T,n>::total_weight() const
{
  typedef typename std::vector<T >::const_iterator wit_t;

  wit_t wit = weights_.begin();

  T sum = T(0);
  for (; wit != weights_.end(); ++wit) {
    sum += *wit;
  }

  return sum;
}

//: return number of modes in the current assignment vector
template <class T, unsigned n>
unsigned
bsta_sample_set<T,n>::mode_cnt() const
{
  typedef typename std::vector<int >::const_iterator ait_t;
  ait_t ait = assignments_.begin();
  std::map<int, bool> modes;
  for (; ait != assignments_.end(); ++ait) {
    std::map<int, bool>::iterator it = modes.find(*ait);
    if (it == modes.end()) {
      modes[*ait] = true;
    }
  }
  return modes.size();
}

//: Insert a weighted sample into the distribution
template <class T, unsigned n>
void
bsta_sample_set<T,n>::insert_sample(typename bsta_parzen_sphere<T,n>::vector_type const& sample, T weight)
{
  bsta_parzen<T,n>::samples_.push_back(sample);
  weights_.push_back(weight);
}

//: one may need to normalize the weights after the insertion is over
template <class T, unsigned n>
void
bsta_sample_set<T,n>::normalize_weights()
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

//: must call this method before using the assignment vector
template <class T, unsigned n>
void
bsta_sample_set<T,n>::initialize_assignments()
{
  assignments_.clear();
  assignments_ = std::vector<int>(bsta_parzen<T,n>::samples_.size(), -1);
}

#define BSTA_SAMPLE_SET_INSTANTIATE(T,n) \
template class bsta_sample_set<T,n >

#endif // bsta_sample_set_hxx_
