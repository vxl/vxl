#ifndef bvpl_opinion_functor_h_
#define bvpl_opinion_functor_h_
//:
// \file
// \brief Functor to apply a kernel to opinion values (see bvxm_opinion class)
//
// \author Gamze Tunali (gamze_tunali@brown.edu)
// \date June 22, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bvpl/kernels/bvpl_kernel_iterator.h>
#include <bvxm/grid/bvxm_opinion.h>

class bvpl_opinion_functor
{
 public:
  //: Default constructor
  bvpl_opinion_functor();

  //: Destructor
  ~bvpl_opinion_functor() = default;

  //: Apply a given operation to value val, depending on the dispatch character
  void apply(bvxm_opinion& val, bvpl_kernel_dispatch& d);

  //: Returns the final operation of this functor
  bvxm_opinion result();

 private:
  float b_;  // accumulates belief
  float u_;  // accumulates uncertainty
  float n_m_;  // accumulates the number of apply called
  float n_p_;  // accumulates uncertainty
  //: Initializes class variables
  void init();
};

#endif
