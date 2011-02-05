// This is brl/bseg/bvpl/functors/bvpl_algebraic_functor.h
#ifndef bvpl_algebraic_functor_h
#define bvpl_algebraic_functor_h
//:
// \file
// \brief A functor that multiplies the value to its weight and adds the result
// \author Isabel Restrepo mir@lems.brown.edu
// \date  11-Oct-2010
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bvpl/kernels/bvpl_kernel_iterator.h>

class bvpl_algebraic_functor
{
 public:
  bvpl_algebraic_functor();

  //: Apply a given operation to value \p val, depending on the dispatch character
  void apply(const float& val, const bvpl_kernel_dispatch& d);

  //: Returns the final operation of this functor
  float result();

 private:

  float result_;

  //: Initializes class variables
  void init();
};
#endif
