#include "bvpl_edge2d_kernel.h"
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_rotation_matrix.h>
#include <vnl/vnl_vector_fixed.h>
#include <vcl_iostream.h>

bvpl_edge2d_kernel::bvpl_edge2d_kernel()
{
  height_=0;
  width_=0;
  R_.fill(0.0);
  angular_resolution_x_=0;
  angular_resolution_y_=0;
  angular_resolution_z_=0;
  kernel_;
}

bvpl_edge2d_kernel::bvpl_edge2d_kernel(unsigned height, unsigned width,vnl_vector_fixed<double,3> const& rotations)
{
  //The size of the kernel is limited. If widht or height of the kernel is too large,
  //the user should subsample the image/grid
  if(height > max_size_)
  {
    vcl_cerr<< "Warning, height of kernel is too large. It has been set to is max value of" << max_size_ << vcl_endl;
    height = max_size_;
  }
  if(width > max_size_)
  {
    vcl_cerr<< "Warning, weight of kernel is too large. It  has been set to is max value of" << max_size_ << vcl_endl;
    width = max_size_;
  }
  
  height_=height;
  width_ = width;
  
  //create a basic centered edge with 1, 0, -1, if width is odd
  // 1 1 0 -1 -1
  // 1 1 0 -1 -1

  typedef vgl_point_3d<int> point_3d;
  typedef bvpl_kernel_dispatch dispatch;
  if((width_ % 2))
  {
    for(unsigned r=0; r< height; ++r)
    {
      for (unsigned c=0; r <width; ++c)
      {
        if(c <=  width/2)
          kernel_.push_back(vcl_pair<point_3d,dispatch>(point_3d(c,r,0), dispatch('+')));
        if(c ==  (width/2 + 1))
          kernel_.push_back(vcl_pair<point_3d,dispatch>(point_3d(c,r,0), dispatch('0')));
        if(c >  width/2 + 1)
        kernel_.push_back(vcl_pair<point_3d,dispatch>(point_3d(c,r,0), dispatch('-')));
      }
    }
    
    //create a basic uncentered edge with 1, -1, if width is eve
    // 1 1 -1 -1
    // 1 1 -1 -1
    
    if(!(width_ % 2))
    {
      for(unsigned r=0; r<height; ++r)
      {
        for (unsigned c=0; c<width; ++c)
        {
          if(c <=  width/2)
            kernel_.push_back(vcl_pair<point_3d,dispatch>(point_3d(c,r,0), dispatch('+')));
          if(c ==  width/2 + 1)
            kernel_.push_back(vcl_pair<point_3d,dispatch>(point_3d(c,r,0), dispatch('0')));
          if(c >  (width/2 + 1))
          kernel_.push_back(vcl_pair<point_3d,dispatch>(point_3d(c,r,0), dispatch('-')));
        }
      }
      
      //TO DO: determine angular resolition based on size of kernel
      
      //Now rotate the basic kernel according to user specifications
      vnl_rotation_matrix(rotations, R_);
      
      if(rotations != vnl_vector_fixed<double,3>(0.0, 0.0, 0.0))
      {
        this->warp_nearest_neighbor();
      }
      
    }
  }
}
  
