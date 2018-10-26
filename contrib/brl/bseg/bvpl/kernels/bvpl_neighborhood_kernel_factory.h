// This is brl/bseg/bvpl/kernels/bvpl_neighborhood_kernel_factory.h
#ifndef bvpl_neighborhood_kernel_factory_h
#define bvpl_neighborhood_kernel_factory_h
//:
// \file
// \author Isabel Restrepo mir@lems.brown.edu
// \date  October 6, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include "bvpl_kernel_factory.h"

class bvpl_neighborhood_kernel_factory : public bvpl_kernel_factory
{
 public:

  //:Default Constructor
  bvpl_neighborhood_kernel_factory();

  //: Constructs a kernel form three values, length, width and thickness
  bvpl_neighborhood_kernel_factory(int min_length, int max_length,
                                   int min_width, int max_width,
                                   int min_thickness, int max_thickness);

  ~bvpl_neighborhood_kernel_factory() override = default;

 private:

  //:Creates a 2d edge kernel
  void create_canonical() override;

  int min_length_;
  int max_length_;
  int min_width_;
  int max_width_;
  int min_thickness_;
  int max_thickness_;

  static const unsigned max_size_ = 71;
};

#endif
