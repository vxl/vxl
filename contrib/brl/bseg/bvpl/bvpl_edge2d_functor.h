#ifndef bvpl_edge2d_functor_h_
#define bvpl_edge2d_functor_h_

#include "bvpl_kernel_iterator.h"

template <class T>
class bvpl_edge2d_functor {
public:
  bvpl_edge2d_functor();
  ~bvpl_edge2d_functor();
  void apply(T& val, bvpl_kernel_dispatch& d);
  T result() { return result_; }
private:
  T result_;
};

#endif