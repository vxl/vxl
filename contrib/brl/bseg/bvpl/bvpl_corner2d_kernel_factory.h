// This is brl/bseg/bvpl/bvpl_corner2d_kernel_factory.h
#ifndef bvpl_corner2d_kernel_factory_h
#define bvpl_corner2d_kernel_factory_h
//:
// \file
// \brief A factory that creates volumetric 2-d edges
// \author Isabel Restrepo mir@lems.brown.edu
//
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
// The weights are not normalized. i.e minimun response is not zero
class bvpl_corner2d_kernel_factory : public bvpl_kernel_factory
{
 public:

  //:Default Constructor
  bvpl_corner2d_kernel_factory();

  //: Constructs a kernel form three values, lenght, widht and thickness
  bvpl_corner2d_kernel_factory(unsigned lenght, unsigned widht, unsigned thickness);

  virtual ~bvpl_corner2d_kernel_factory() {}

 private:

  //:Creates a 2d edge kernel
  virtual void create_canonical();

  float length_;
  float width_;
  float thickness_;

  static const unsigned max_size_ = 71;
};

#endif
