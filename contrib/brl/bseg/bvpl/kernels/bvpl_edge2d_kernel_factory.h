// This is brl/bseg/bvpl/kernels/bvpl_edge2d_kernel_factory.h
#ifndef bvpl_edge2d_kernel_h_
#define bvpl_edge2d_kernel_h_
//:
// \file
// \brief  A class for creating edge kernels. The edge lies on a single plane.
// This class specializes bvpl_kernel_factory. Please refer to such class for usage details.
// \author Isabel Restrepo mir@lems.brown.edu
// \date  May 29, 2009
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


class bvpl_edge2d_kernel_factory : public bvpl_kernel_factory
{
 public:
  //:Default Constructor
  bvpl_edge2d_kernel_factory();

  //: Constructs an edge of dimensions height*width. The canonical form of this edge is along the y-axis
  // + + 0 - -
  // + + 0 - -
  // + + 0 - -
  // + + 0 - -
  bvpl_edge2d_kernel_factory(unsigned height, unsigned width);

  ~bvpl_edge2d_kernel_factory() override = default;

  //Height of edge
  unsigned height() const { return height_; }

  //Width of edge
  unsigned width() const { return width_; }

 private:

  //:Creates a 2d edge kernel
  void create_canonical() override;

  unsigned height_;
  unsigned width_;

  static const unsigned max_size_ = 71;
};


#endif
