#ifndef bvpl_edge_algebraic_mean_functor_h_
#define bvpl_edge_algebraic_mean_functor_h_
//:
// \file
// \brief Functor to find the 2D edges with computing expected values
//
// \author Vishal Jain (vj@lems.brown.edu)
// \date June 29, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bvpl/kernels/bvpl_kernel_iterator.h>

template <class T>
class bvpl_edge_algebraic_mean_functor
{
 public:
  //: Default constructor
  bvpl_edge_algebraic_mean_functor();

  //: Destructor
  ~bvpl_edge_algebraic_mean_functor() = default;

  //: Apply a given operation to value val, depending on the dispatch character
  void apply(T& val, bvpl_kernel_dispatch& d);

  //: Returns the final operation of this functor
  T result();

 private:
  T P1_;  // probability of all 1s
  T P0_;  // probability of all 1s


  unsigned n1_;
  unsigned n0_;
  //: Initializes class variables
  void init();
};

#endif
