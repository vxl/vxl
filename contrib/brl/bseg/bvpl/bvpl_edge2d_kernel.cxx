#include "bvpl_edge2d_kernel.h"
//:
// \file

//: Default Constructor
bvpl_edge2d_kernel::bvpl_edge2d_kernel()
{
  height_=0;
  width_=0;
  R_.fill(0.0);
  angular_resolution_x_=0;
  angular_resolution_y_=0;
  angular_resolution_z_=0;
}


//: Constructor using rotation axis and angle
bvpl_edge2d_kernel::bvpl_edge2d_kernel(unsigned height, unsigned width, vnl_vector_fixed<double,3> axis, double angle)
{
  //set dimensions
  height_=height;
  width_=width;

  //set rotation matrix. vnl_rotation_matrix(vnl_vector_fixed<double,3> const& axis),
  //is rotation around axis by and angle ||axis||

  //make sure the axis is unit norm
  double norm = vcl_sqrt(axis[0]*axis[0] + axis[1]*axis[1] + axis[2]*axis[2]);
  if (norm > 1){
    axis[0] /=norm;
    axis[1] /=norm;
    axis[2] /=norm;
  }

  //multiplicate by angle
  axis *= angle;
  vnl_rotation_matrix(axis, R_);

  //TO DO: determine angular resolition based on size of kernel
  // angular_resolution_x_=0;
  // angular_resolution_y_=0;
  // angular_resolution_z_=0;
}


//: Constructor using rotation euler angles, where R = Rz*Ry*Rx
bvpl_edge2d_kernel::bvpl_edge2d_kernel(unsigned height, unsigned width, vnl_vector_fixed<double,3> rotation)
{
  //set dimensions
  height_=height;
  width_=width;

  //set rotation matrix  R = Rz*Ry*Rx
  this->set_rotation(rotation);

  //TO DO: determine angular resolition based on size of kernel
  // angular_resolution_x_=0;
  // angular_resolution_y_=0;
  // angular_resolution_z_=0;
}


bool
bvpl_edge2d_kernel::create()
{
  if ( !(height_ % 2))
  {
    vcl_cerr << "Warning, height of kernel is even. It has been increased by one\n";
    height_++;
  }
  if (!(width_ % 2))
  {
    vcl_cerr << "Warning, width of kernel is even. It has been increased by one\n";
    width_++;
  }
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

  //create a basic centered edge with 1, 0, -1, if width is odd
  // 1 1 0 -1 -1
  // 1 1 0 -1 -1
  // 1 1 0 -1 -1
  // 1 1 0 -1 -1

  typedef vgl_point_3d<int> point_3d;
  typedef bvpl_kernel_dispatch dispatch;

  int min_y= -1*(height_/2);
  int max_y =(height_/2);
  int min_x= -1*(width_/2);
  int max_x =(width_/2);

  for (int r= min_y; r<= max_y; r++)
  {
    for (int c=min_x; c<= max_x; c++)
    {
      if (c < 0)
        kernel_.push_back(vcl_pair<point_3d,dispatch>(point_3d(c,r,0), dispatch(-1)));
      else if (c >  0)
        kernel_.push_back(vcl_pair<point_3d,dispatch>(point_3d(c,r,0), dispatch(1)));
    }
  }

  this->warp_nearest_neighbor();

  return true;
}
