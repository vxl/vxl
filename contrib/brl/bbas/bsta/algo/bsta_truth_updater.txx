// This is brl/bbas/bsta/algo/bsta_truth_updater.txx
#ifndef bsta_truth_updater_txx_
#define bsta_truth_updater_txx_
//:
// \file
#include "bsta_truth_updater.h"
#include <vcl_cassert.h>


//: The main function
template <class T>
void
bsta_truth_updater<T>::operator() ( bsta_distribution<T>& d,
                                    const vnl_vector<T>& sample ) const
{
  assert(dynamic_cast<bsta_mixture<T>*>(&d));
  bsta_mixture<T>& mixture = static_cast<bsta_mixture<T>&>(d);
  assert(model_dist_->dim()+1 == sample.size());

  // The actual sample is stored in the first n-1 dimensions
  vnl_vector<T> data(model_dist_->dim(),model_dist_->dim(),sample.data_block());

  // Get the index from the last dimension
  unsigned int index = static_cast<unsigned int>(sample[model_dist_->dim()]);

  while (mixture.num_components() <= index)
    mixture.insert(*model_dist_,T(0));

  (*updater_)(mixture.distribution(index),data);
}

#define BSTA_TRUTH_UPDATER_INSTANTIATE(T) \
template class bsta_truth_updater<T >

#endif // bsta_truth_updater_txx_
