// This is brl/bseg/bvpl/functors/bvpl_find_surface_functor.h
#ifndef bvpl_find_surface_functor_h
#define bvpl_find_surface_functor_h
//:
// \file
// \brief A functor to find the geometric mean of occupancy on a >0 kernel
// \author Isabel Restrepo mir@lems.brown.edu
// \date  Sept 20, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bvpl/kernels/bvpl_kernel_iterator.h>


class bvpl_find_surface_functor
{
 public:
  //: Default constructor
  bvpl_find_surface_functor();

  //: Destructor
  ~bvpl_find_surface_functor() = default;

  //: Apply a given operation to value val, depending on the dispatch character
  void apply(float& val, bvpl_kernel_dispatch& d);

  //: Returns the final operation of this functor
  float result();

 private:
  float P1_;  // probability of all 1s
  float P0_;  // probability of all 1s


  unsigned n1_;
  unsigned n0_;
  //: Initializes class variables
  void init();
};


#endif
