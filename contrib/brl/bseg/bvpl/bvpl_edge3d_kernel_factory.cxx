#include "bvpl_edge3d_kernel_factory.h"
//:
// \file

#include <vcl_algorithm.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_float_3.h>

// Default Constructor
bvpl_edge3d_kernel_factory::bvpl_edge3d_kernel_factory()
{
  height_=0;
  width_=0;
  length_=0;
  angular_resolution_ = 0;
  canonical_rotation_axis_[0] = 0.0f; canonical_rotation_axis_[1] = 0.0f; canonical_rotation_axis_[2] = 0.0f;
  canonical_parallel_axis_[0] = 0.0f; canonical_parallel_axis_[1] = 0.0f; canonical_parallel_axis_[2] = 0.0f;
  rotation_axis_ = canonical_rotation_axis_;
  //parallel_axis_ = canonical_parallel_axis_;
  angle_ = 0.0f;
}

//: Constructs an edge of dimensions height*width*length. The canonical form of this edge is along the y - axis
// + + 0 - -
// + + 0 - -
// + + 0 - -
// + + 0 - -
bvpl_edge3d_kernel_factory::bvpl_edge3d_kernel_factory(unsigned length, unsigned width, unsigned height)
{
  //set dimensions
  length_=length;
  height_=height;
  width_=width;


  //Determine angular resolition based on size of kernel
  //If this was 2D, then the angular resolution would be 180/(2l -2) (Recusive Binary Dilation... Desikachari Nadadur)
  angular_resolution_=0;
  //set canonical axis to x-axis
  canonical_rotation_axis_[0] = 1.0f; canonical_rotation_axis_[1] = 0.0f; canonical_rotation_axis_[2] = 0.0f;

  canonical_parallel_axis_[0] = 0.0f; canonical_parallel_axis_[1] = 1.0f; canonical_parallel_axis_[2] = 0.0f;
  angle_ = 0.0f;
  rotation_axis_ = canonical_rotation_axis_;
  //parallel_axis_ = canonical_paralell_axis_;
  create_canonical();
}

void bvpl_edge3d_kernel_factory::create_canonical()
{

  //The size of the kernel is limited. If widht or height of the kernel is too large,
  //the user should subsample the image/grid
  if (height_ > max_size_)
  {
    vcl_cerr<< "Warning, height of kernel is too large. It has been set to is max value of" << max_size_ << vcl_endl;
    height_ = max_size_;
  }
  if (width_ > max_size_)
  {
    vcl_cerr<< "Warning, weight of kernel is too large. It  has been set to is max value of" << max_size_ << vcl_endl;
    width_ = max_size_;
  }

  //create a basic centered edge with 1, 0, -1, if width is odd.  The edge is creates on the x-z plane
  // 1 1 -1 -1
  // 1 1 -1 -1
  // 1 1 -1 -1
  // 1 1 -1 -1

  typedef vgl_point_3d<float> point_3d;
  typedef bvpl_kernel_dispatch dispatch;

  int min_x= -1*(length_/2);
  int max_x =vcl_ceil((float)length_/2);
  int min_y= -1*(width_/2);
  int max_y =vcl_ceil((float)width_/2);
  int min_z= -1*(height_/2);
  int max_z =vcl_ceil((float)height_/2);

  for (int x=min_x; x< max_x; x++)
  {
    for (int y= min_y; y< max_y; y++)
    {
      for (int z= min_z; z< max_z; z++)
      {
        if (x < 0)
          canonical_kernel_.push_back(vcl_pair<point_3d,dispatch>(point_3d(float(x),float(y),float(z)), dispatch(-1)));
        else if (x >=  0)
          canonical_kernel_.push_back(vcl_pair<point_3d,dispatch>(point_3d(float(x),float(y),float(z)), dispatch(1)));
  
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


