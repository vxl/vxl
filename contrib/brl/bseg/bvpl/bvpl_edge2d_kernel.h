// This is brl/bseg/bvpl/bvpl_edge2d_kernel.h
#ifndef bvpl_edge2d_kernel_h_
#define bvpl_edge2d_kernel_h_
//:
// \file
// \brief
// \author Isabel Restrepo mir@lems.brown.edu
// \date  May 29, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include "bvpl_kernel_base.h"
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_rotation_matrix.h>
#include <vnl/vnl_vector_fixed.h>
#include <vcl_iostream.h>


class bvpl_edge2d_kernel : public bvpl_kernel_base
{
 public:
  //:Default Constructor
  bvpl_edge2d_kernel();
  
  //: Constructor using rotation axis and angle
  bvpl_edge2d_kernel(unsigned height, unsigned width, vnl_vector_fixed<double,3> axis, double angle);
  
  //: Constructor using rotation euler angles, where R = Rz*Ry*Rx
  bvpl_edge2d_kernel(unsigned height, unsigned width, vnl_vector_fixed<double,3> rotation);

  //:Creates a 2d edge kernel
  bool create();
  
  unsigned height() {return height_;}
  
  unsigned width() {return width_;}
  
  virtual vcl_string name() {return "edge2d";};

 private:

  unsigned height_;
  unsigned width_;
  static const unsigned max_size_ = 100;
};

#endif
