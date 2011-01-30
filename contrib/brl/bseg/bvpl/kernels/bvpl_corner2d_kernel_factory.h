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

#include "bvpl_kernel_factory.h"
#include <vcl_iostream.h>

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

  virtual ~bvpl_corner2d_kernel_factory() {}

 private:

  //:Creates a 2d edge kernel
  virtual void create_canonical();

  unsigned length_;
  unsigned width_;
  unsigned thickness_;

  static const unsigned max_size_ = 71;
};

#endif
