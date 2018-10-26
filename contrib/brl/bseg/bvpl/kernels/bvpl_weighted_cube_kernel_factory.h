// This is//projects/vxl/src/contrib/brl/bseg/bvpl/bvpl_weighted_cube_kernel_factory.h
#ifndef bvpl_weighted_cube_kernel_factory_h
#define bvpl_weighted_cube_kernel_factory_h

//:
// \file
// \brief
// \author Isabel Restrepo mir@lems.brown.edu
// \date  9/23/09
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include "bvpl_kernel_factory.h"

class bvpl_weighted_cube_kernel_factory : public bvpl_kernel_factory
{
 public:

  //: Default Constructor
  bvpl_weighted_cube_kernel_factory();

  //: Constructs a kernel form three values, length, width and thickness
  bvpl_weighted_cube_kernel_factory(unsigned length, unsigned width, unsigned thickness);

  ~bvpl_weighted_cube_kernel_factory() override = default;

 private:

  //:Creates a 2d edge kernel
  void create_canonical() override;

  unsigned length_;
  unsigned width_;
  unsigned thickness_;

  static const unsigned max_size_ = 71;
};
#endif
