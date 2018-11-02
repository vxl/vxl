// This is brl/bbas/bsta/algo/bsta_mean_shift.hxx
#ifndef bsta_mean_shift_hxx_
#define bsta_mean_shift_hxx_
//:
// \file
#include <iostream>
#include <map>
#include "bsta_mean_shift.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class T, unsigned n>
bool
bsta_mean_shift<T,n>::find_modes(bsta_sample_set<T,n>& set, vnl_random & rng, float percentage, T epsilon)
{
  typedef typename bsta_parzen_sphere<T,n>::vector_type vect_t;

  // initialize seeds by picking given percentage of the sample set randomly
  int size = set.size();
  int seed_size = (int)std::ceil(percentage*size/100.0f);
  std::cout << "size: " << size << " seed_size: " << seed_size << std::endl;

  set.initialize_assignments();

  std::cout << "initialized assignment_, its size: " << set.assignments().size()  << std::endl;

  for (int i = 0; i < seed_size; i++)
  {
    // randomly pick one of the samples as seed
    double rn = rng.drand32();
    int s_id = (int)std::floor(rn*(size-1)+0.5f);
    if (set.assignment(s_id) >= 0) { // has already been assigned to a mode
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
      // found a stable mode
      modes_.push_back(current);
      set.set_assignment(s_id, modes_.size()-1);
    }
  }

  return true;
}

//: use all the samples to get its mode, no need for random seed picking
template <class T, unsigned n>
bool
bsta_mean_shift<T,n>::find_modes(bsta_sample_set<T,n>& set, T epsilon)
{
  typedef typename bsta_distribution<T,n>::vector_type vect_t;

  int size = set.size();
  set.initialize_assignments();

  for (int i = 0; i < size; i++)
  {
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
      // found a stable mode
      modes_.push_back(current);
      set.set_assignment(i, modes_.size()-1);
    }
  }

  return true;
}

//: merge the mode with samples less than cnt to one of the modes depending on its samples mean-shift paths
template <class T, unsigned n>
bool
bsta_mean_shift<T,n>::merge_modes(bsta_sample_set<T,n>& set, int cnt, T epsilon)
{
  typedef typename bsta_distribution<T,n>::vector_type vect_t;
  std::vector<bool> eliminated_modes(modes_.size(), false);

  for (unsigned m = 0; m < modes_.size(); m++)
  {
    if (set.mode_size(m) < cnt)
    {
      // eliminate this mode
      eliminated_modes[m] = true;
      for (unsigned i = 0; i < set.size(); i++) {
        if (set.assignment(i) == (int)m) {
          // find a new mode using mean-shift procedure
          vnl_vector_fixed<T,n> dif(T(10e4));
          vect_t current = set.sample(i);
          unsigned count = 0;
          while (dif.magnitude() > epsilon) {
            vect_t mean;
            if (set.mean(current, mean)) {
              vect_t v_dif = mean-current;
              dif = vnl_vector_fixed<T,n>(v_dif);
              current = mean;
            }
            count++;
            if (count > max_iter_)
              break;
          }
          // found a mode, check which other modes, this one is most closest to
          vnl_vector_fixed<T,n> dif_min(T(10e4));
          unsigned mm_min = m;
          for (unsigned mm = 0; mm < modes_.size(); mm++) {
            if (eliminated_modes[mm])
              continue;

            vect_t v_dif = modes_[mm]- current;
            vnl_vector_fixed<T,n> dif(v_dif);
            if (dif.magnitude() < dif_min.magnitude()) {
              dif_min = dif; mm_min = mm;
            }
          }
          set.set_assignment(i, mm_min);
        }
      }
    }
  }

  // re-arrange the assignment vector with the new mode ids
  std::vector<vect_t > new_modes;
  for (unsigned i = 0; i < modes_.size(); i++) {
    if (!eliminated_modes[i]) {
      new_modes.push_back(modes_[i]);
      for (unsigned jj = 0; jj < set.assignments().size(); jj++) {
        if (set.assignment(jj) == (int)i)
          set.set_assignment(jj, new_modes.size()-1);
      }
    }
  }

  modes_.clear();
  modes_ = new_modes;

  return true;
}


template <class T, unsigned n>
bool
bsta_mean_shift<T,n>::trim_modes(bsta_sample_set<T,n>& set, T epsilon)
{
  typedef typename bsta_distribution<T,n>::vector_type vect_t;

  if (!modes_.size())
    return false;

  std::vector<bool> trimmed(modes_.size(), false);
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
        // assign all the samples of the trimmed mode to this new mode v_i
        for (unsigned kk = 0; kk < set.assignments().size(); kk++) {
          if (set.assignment(kk) == (int)j) {
            set.set_assignment(kk, i);
          }
        }
      }
    }
  }
  std::vector<vect_t > new_modes;
  for (unsigned i = 0; i < modes_.size(); i++) {
    if (!trimmed[i]) {
      new_modes.push_back(modes_[i]);
      for (unsigned jj = 0; jj < set.assignments().size(); jj++) {
        if (set.assignment(jj) == (int)i)
          set.set_assignment(jj, new_modes.size()-1);
      }
    }
  }

  modes_.clear();
  modes_ = new_modes;
  return true;
}

#define BSTA_SAMPLE_SET_INSTANTIATE(T,n) \
template class bsta_sample_set<T,n >

#define BSTA_MEAN_SHIFT_INSTANTIATE(T,n) \
template class bsta_mean_shift<T,n >


#endif // bsta_mean_shift_hxx_
