// This is//projects/vxl/src/contrib/brl/bseg/bvpl/bvpl_corner_search_kernel.h
#ifndef bvpl_corner_search_kernel_h
#define bvpl_corner_search_kernel_h
//:
// \file
// \brief  A class to determine how to find a corner according to bvpl_weighted_cubel_kernel
// \author Isabel Restrepo mir@lems.brown.edu
// \date  September 23, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#include <bvpl/kernels/bvpl_kernel_factory.h>
#include <bvpl/kernels/bvpl_create_directions.h>
#include <bvpl/kernels/bvpl_kernel_iterator.h>
#include <vgl/vgl_line_3d_2_points.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class bvpl_corner_search_functor
{
 public:
  //: Default Constructor
  bvpl_corner_search_functor() : this_id_(-1) {}

  //: Constructor
  bvpl_corner_search_functor(int this_id, std::vector<vnl_float_3> axes, std::vector<float> angles );

  //: Destructor
  ~bvpl_corner_search_functor() = default;

  //: Apply a given operation to value val, depending on the dispatch character
  void apply(int& id, bvpl_kernel_dispatch& d, vgl_point_3d<int> p1, vgl_point_3d<int> p2);

  //: Returns the final operation of this functor
  float result();

 private:
  int this_id_;
  std::vector<vnl_float_3> axes_;
  std::vector<float> angles_;
  std::vector<vgl_line_3d_2_points<int> > lines_;
};


bvpl_corner_search_functor::bvpl_corner_search_functor(int this_id, std::vector<vnl_float_3> axes, std::vector<float> angles)
{
  this_id_ = this_id;
  axes_ =  axes;
  angles_= angles;
}


void bvpl_corner_search_functor::apply(int& id, bvpl_kernel_dispatch& /*d*/, vgl_point_3d<int> p1, vgl_point_3d<int> p2)
{
  if (axes_[id]==axes_[this_id_]) // same plane; now look for opposite diagonal corners
    if (std::abs(std::abs(angles_[id] - angles_[this_id_]) - vnl_math::pi_over_2) > 1e-7)
      lines_.emplace_back(p1, p2);
}

#endif // bvpl_corner_search_kernel_h
