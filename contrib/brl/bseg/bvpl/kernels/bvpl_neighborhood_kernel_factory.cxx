#include <iostream>
#include <algorithm>
#include "bvpl_neighborhood_kernel_factory.h"
//:
// \file

#include <vnl/vnl_math.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// Default Constructor
bvpl_neighborhood_kernel_factory::bvpl_neighborhood_kernel_factory()
{
  min_length_ = 0;
  max_length_ = 0;
  min_width_ =0;
  max_width_ = 0;
  min_thickness_ = 0;
  max_thickness_ = 0;
  angular_resolution_ = 0;
  rotation_axis_ = canonical_rotation_axis_;
  angle_ = 0.0f;
}

//: Constructs a kernel from length, width and thickness
bvpl_neighborhood_kernel_factory::bvpl_neighborhood_kernel_factory(int min_length, int max_length,
                                                                   int min_width, int max_width,
                                                                   int min_thickness, int max_thickness)
{
  //set variables
  min_length_ = min_length;
  max_length_ = max_length;
  min_width_ = min_width;
  max_width_ = max_width;
  min_thickness_ = min_thickness;
  max_thickness_ = max_thickness;


  //this kernel is not symmetric around main axis
  angular_resolution_= float(vnl_math::pi_over_4);

  //initialize variables
  angle_ = 0.0f;
  rotation_axis_ = canonical_rotation_axis_;

  //create the default kernel
  create_canonical();
}

void bvpl_neighborhood_kernel_factory::create_canonical()
{
  typedef vgl_point_3d<float> point_3d;
  typedef bvpl_kernel_dispatch dispatch;

  for (int x=min_thickness_; x<=max_thickness_; x++)
  {
    for (int z=min_length_; z<=max_length_; z++)
    {
      for (int y=min_width_; y<=max_width_; y++)
      {
        canonical_kernel_.emplace_back(point_3d(float(x),float(y),float(z)), dispatch(1.0f));
      }
    }
  }

  //set the dimension of the 3-d grid
  max_point_.set(max_thickness_,max_width_,max_length_);
  min_point_.set(min_thickness_,min_width_,min_length_);

  //set the current kernel
  kernel_ = canonical_kernel_;

  return;
}
