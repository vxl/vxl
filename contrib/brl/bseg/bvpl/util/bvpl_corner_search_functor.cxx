#include <iostream>
#include <algorithm>
#include "bvpl_corner_search_functor.h"
//:
// \file

#include <bvpl/kernels/bvpl_corner2d_kernel_factory.h>
#include <vnl/vnl_math.h>
#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// Default Constructor
bvpl_corner2d_kernel_factory::bvpl_corner2d_kernel_factory()
{
  length_ = 0;
  width_ = 0;
  thickness_ = 0;
  angular_resolution_ = 0;
  rotation_axis_ = canonical_rotation_axis_;
  angle_ = 0.0f;
}

//: Constructs a kernel from length, width and thickness
bvpl_corner2d_kernel_factory::bvpl_corner2d_kernel_factory(unsigned length, unsigned width , unsigned thickness)
{
  //set variables
  length_ = length;
  width_ = width;
  thickness_ = thickness;

  //this kernel is not symmetric around main axis
  angular_resolution_= float(vnl_math::pi_over_4);

 //initialize variables
  angle_ = 0.0f;
  rotation_axis_ = canonical_rotation_axis_;

  //create the default kernel
  create_canonical();
}

void bvpl_corner2d_kernel_factory::create_canonical()
{
  //The size of the kernel is limited. If width or height of the kernel is too large,
  //the user should subsample the image/grid
  if ( (length_ > max_size_) || (width_ > max_size_) || (thickness_ > max_size_) )
  {
    std::cerr<< "Warning, kernel is too large. You should subsample world. Processing may take a long time.\n";
  }

  typedef vgl_point_3d<float> point_3d;
  typedef bvpl_kernel_dispatch dispatch;

  int min_x = -1 * int(thickness_);
  int max_x =  int(thickness_);
  int min_y = -1 * int(width_);
  int max_y =  int(width_);
  int min_z = -1 * int(length_);
  int max_z =  int(length_);

  int n0=0;
  int n1=0;
  for (int x=min_x; x<=max_x; x++)
  {
    for (int z=min_z+1; z<=max_z; z++)
    {
      for (int y=min_y+1; y<=max_y; y++)
      {
        //if ((y==0) && (z==0));
        if (z <= 0)
          n1++;
        else if (y <= 0)
          n1++;
        else
          n0++;
      }
    }
  }

  for (int x=min_x; x<=max_x; x++)
  {
    for (int z=min_z+1; z<=max_z; z++)
    {
      for (int y=min_y+1; y<=max_y; y++)
      {
        //if ((y==0) && (z==0))
        //  canonical_kernel_.push_back(std::pair<point_3d,dispatch>(point_3d(float(x),float(y),float(z)), dispatch(0.0f)));
        if (z <= 0)
          canonical_kernel_.emplace_back(point_3d(float(x),float(y),float(z)), dispatch(1.0f/float(n1)));
        else if (y <= 0)
          canonical_kernel_.emplace_back(point_3d(float(x),float(y),float(z)), dispatch(1.0f/float(n1)));
        else
          canonical_kernel_.emplace_back(point_3d(float(x),float(y),float(z)), dispatch(-1.0f/float(n0)));
      }
    }
  }

  //set the dimension of the 3-d grid
  max_point_.set(max_x,max_y,max_z);
  min_point_.set(min_x,min_y,min_z);

  //set the current kernel
  kernel_ = canonical_kernel_;

  return;
}

