#ifndef bvpl_edge2d_functor_h_
#define bvpl_edge2d_functor_h_

#include "bvpl_kernel_iterator.h"

template <class T>
class bvpl_edge2d_functor {
public:
  bvpl_edge2d_functor(): P_(0), P1_(0), P0_(0), P05_(0) {}
  ~bvpl_edge2d_functor() {}
  void apply(T& val, bvpl_kernel_dispatch& d);
  T result();
private:
  T P_;   // probability based on kernel
  T P1_;  // probability of all 1s
  T P0_;  // probability of all 1s
  T P05_;  // probability of all 0.5
};

#endif