// This is brl/bseg/bvpl/kernels/bvpl_corner2d_kernel_factory.h
#ifndef bvpl_corner2d_kernel_factory_h
#define bvpl_corner2d_kernel_factory_h
//:
// \file
// \brief A factory that creates volumetric 2-d edges
// \author Isabel Restrepo mir@lems.brown.edu
// \date  August 25, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#include "bvpl_kernel_factory.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: A class to generate kernels of corners with thickness.
// The general form of the corner is of the following type
// + + + +
// + - - -
// + - - -
// + - - -
// The weights are not normalized. I.e., minimum response is not zero
class bvpl_corner2d_kernel_factory : public bvpl_kernel_factory
{
 public:

  //:Default Constructor
  bvpl_corner2d_kernel_factory();

  //: Constructs a kernel form three values, length, width and thickness
  bvpl_corner2d_kernel_factory(unsigned length, unsigned width, unsigned thickness);

  ~bvpl_corner2d_kernel_factory() override = default;

 private:

  //:Creates a 2d edge kernel
  void create_canonical() override;

  unsigned length_;
  unsigned width_;
  unsigned thickness_;

  static const unsigned max_size_ = 71;
};

#endif
