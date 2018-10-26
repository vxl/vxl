#ifndef bvpl_edge2d_functor_h_
#define bvpl_edge2d_functor_h_
//:
// \file
// \brief Functor to find the 2D edges
//
// \author Gamze Tunali (gamze_tunali@brown.edu)
// \date June 02, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bvpl/kernels/bvpl_kernel_iterator.h>

template <class T>
class bvpl_edge2d_functor
{
 public:
  //: Default constructor
  bvpl_edge2d_functor();

  //: Destructor
  ~bvpl_edge2d_functor() = default;

  //: Apply a given operation to value val, depending on the dispatch character
  void apply(T& val, bvpl_kernel_dispatch& d);

  //: Returns the final operation of this functor
  T result();

 private:
  T min_P_;
  T P_;   // probability based on kernel
  T P1_;  // probability of all 1s
  T P0_;  // probability of all 1s
  T P05_;  // probability of all 0.5

  //The next variables are normalization values.
  //the correspond to the values above (P_, P1_ ...) when the are is empty/initial value

  T P_norm;
  T P1_norm;
  T P0_norm;
  T P05_norm;

  unsigned n_;
  //: Initializes class variables
  void init();
};

#endif
