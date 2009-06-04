// This is//Projects/vxl/src/contrib/brl/bseg/bvpl/bvpl_edge2d_kernel.h
#ifndef bvpl_edge2d_kernel_h_
#define bvpl_edge2d_kernel_h_
//
//:
// \file
// \brief 
//
// \author Isabel Restrepo mir@lems.brown.edu
//
// \date  5/29/09
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


enum edge2d_type {
  SYMMETRIC =0,
  UNKNOWN
};



template <edge2d_type> 
class bvpl_edge2d_kernel;

//Template specialization of 2d_edge kernels
template <>
class bvpl_edge2d_kernel<SYMMETRIC> : public bvpl_kernel_base
{

public: 
  //:Default Constructors
  bvpl_edge2d_kernel();

  //:Creates a 2d edge kernel of specified type
  bool create(unsigned height, unsigned width, vnl_vector_fixed<double,3> const& rotations);
  unsigned height() {return height_;}
  unsigned width() {return width_;}
  virtual vcl_string name() {return "edge2d";};
  
private:
 
  unsigned height_;
  unsigned width_;
  static const unsigned max_size_ = 15;
  
};

//:Default Constructor
bvpl_edge2d_kernel<SYMMETRIC>::bvpl_edge2d_kernel()
{
  height_=0;
  width_=0;
  R_.fill(0.0);
  angular_resolution_x_=0;
  angular_resolution_y_=0;
  angular_resolution_z_=0;
}

bool
bvpl_edge2d_kernel<SYMMETRIC>::create(unsigned height, unsigned width,vnl_vector_fixed<double,3> const& rotations)
{

  
  if( !(height % 2))
  {
    vcl_cerr<< "Warning, height of kernel is even. It has been increased by one"<< vcl_endl;
    height = height++;
  }
  if(!(width % 2))
  {
    vcl_cerr<< "Warning, width of kernel is even. It has been increased by one"<< vcl_endl;
    width =width++;
  }
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
  // 1 1 0 -1 -1
  // 1 1 0 -1 -1
  
  typedef vgl_point_3d<int> point_3d;
  typedef bvpl_kernel_dispatch dispatch;
  
  int min= -1*(height/2);
  int max =(height/2);
  
    for(int r= min; r<= max; r++)
    {
      for (int c=min; c<= max; c++)
      {
        if(c < 0)
          kernel_.push_back(vcl_pair<point_3d,dispatch>(point_3d(c,r,0), dispatch(-1)));
        if(c >  0)
          kernel_.push_back(vcl_pair<point_3d,dispatch>(point_3d(c,r,0), dispatch(1)));
      }
    }
   
     //TO DO: determine angular resolition based on size of kernel
      
      //Rotate the basic kernel according to user specifications
      vnl_rotation_matrix(rotations, R_);
      
      if(rotations != vnl_vector_fixed<double,3>(0.0, 0.0, 0.0))
      {
        this->warp_nearest_neighbor();
      }
  return true;
      
}


#endif
  