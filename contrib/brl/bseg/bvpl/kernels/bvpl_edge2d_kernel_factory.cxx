#include <iostream>
#include <algorithm>
#include "bvpl_edge2d_kernel_factory.h"
//:
// \file
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_math.h>
#include <vnl/vnl_float_3.h>

// Default Constructor
bvpl_edge2d_kernel_factory::bvpl_edge2d_kernel_factory()
{
  height_=0;
  width_=0;
  angular_resolution_ = 0;
  rotation_axis_ = canonical_rotation_axis_;
  //parallel_axis_ = canonical_parallel_axis_;
  angle_ = 0.0f;
}

//: Constructs an edge of dimensions height*width. The canonical form of this edge is along the y - axis
// + + 0 - -
// + + 0 - -
// + + 0 - -
// + + 0 - -
bvpl_edge2d_kernel_factory::bvpl_edge2d_kernel_factory(unsigned height, unsigned width)
{
  //set dimensions
  height_=height;
  width_=width;


  //Determine angular resolition based on size of kernel
  //If this was 2D, then the angular resolution would be 180/(2l -2) (Recusive Binary Dilation... Desikachari Nadadur)
  angular_resolution_=float( vnl_math::pi) / (2.0f * float(width) - 2.0f);
  //set canonical axis to x-axis
  angle_ = 0.0f;
  rotation_axis_ = canonical_rotation_axis_;
  //parallel_axis_ = canonical_paralell_axis_;
  create_canonical();
}

void bvpl_edge2d_kernel_factory::create_canonical()
{
  if ( !(height_ % 2))
  {
    std::cerr << "Warning, height of kernel is even. It has been increased by one\n";
    height_++;
  }
  if (!(width_ % 2))
  {
    std::cerr << "Warning, width of kernel is even. It has been increased by one\n";
    width_++;
  }
  //The size of the kernel is limited. If width or height of the kernel is too large,
  //the user should subsample the image/grid
  if (height_ > max_size_)
  {
    std::cerr<< "Warning, height of kernel is too large. It has been set to is max value of" << max_size_ << std::endl;
    height_ = max_size_;
  }
  if (width_ > max_size_)
  {
    std::cerr<< "Warning, weight of kernel is too large. It  has been set to is max value of" << max_size_ << std::endl;
    width_ = max_size_;
  }

  //create a basic centered edge with 1, 0, -1, if width is odd.  The edge is creates on the x-z plane
  // 1 1 0 -1 -1
  // 1 1 0 -1 -1
  // 1 1 0 -1 -1
  // 1 1 0 -1 -1

  typedef vgl_point_3d<float> point_3d;
  typedef bvpl_kernel_dispatch dispatch;

  int min_x= -1*(height_/2);
  int max_x =(height_/2);
  int min_y= -1*(width_/2);
  int max_y =(width_/2);

  int z = 0;
  for (int x=min_x; x<= max_x; x++)
  {
    for (int y= min_y; y<= max_y; y++)
    {
      if (y < 0)
        canonical_kernel_.emplace_back(point_3d(float(x),float(y),float(z)), dispatch(-1));
      else if (y >  0)
        canonical_kernel_.emplace_back(point_3d(float(x),float(y),float(z)), dispatch(1));
    }
  }

  //set the dimension of the 3-d grid
  max_point_.set(max_x,max_y,z);
  min_point_.set(min_x,min_y,z);

  //set the current kernel
  kernel_ = canonical_kernel_;

  return;
}

#if 0

/******************Batch Methods ***********************/

//: Creates a vector of kernels with azimuthal and elevation resolution equal to pi/4. And angle of rotation= angular_resolution_
bvpl_kernel_vector_sptr bvpl_edge2d_kernel_factory::create_kernel_vector()
{
  bvpl_kernel_vector_sptr kernels = new bvpl_kernel_vector();
  float theta_res = float(vnl_math::pi_over_4); //azimuth
  float phi_res = float(vnl_math::pi_over_4);   //zenith  (from the pole)

  std::cout << "Crating and vector of 2d-edge kernels\n"
           << "phi_res: "<< phi_res << std::endl
           << "theta_res: "<< theta_res << std::endl
           << "angle_res: "<< angular_resolution_ << std::endl;
  vnl_float_3 axis;

  float theta = 0.0f;
  float phi = 0.0f;

  //when zenith angle is 0
  axis[0] =0.0f;
  axis[1] =0.0f;
  axis[2] =1.0f;
  this->set_rotation_axis(axis);
  for (float angle = 0.0f; angle < 2.0f * float(vnl_math::pi); angle+=this->angular_resolution_)
  {
    this->set_angle(angle);
    kernels->kernels_.push_back(std::make_pair(axis*angle , new bvpl_kernel(this->create())));
  }

  //when zenith is pi/4 travers all hemisphere

  phi = float(vnl_math::pi_over_4);
  for (;theta < 2.0f*float(vnl_math::pi); theta +=theta_res)
  {
    axis[0] = std::cos(theta) * std::sin(phi);
    axis[1] = std::sin(theta) * std::sin(phi);
    axis[2] = std::cos(phi);
    this->set_rotation_axis(axis);
    for (float angle = 0.0f; angle < 2.0f * float(vnl_math::pi); angle+=this->angular_resolution_)
    {
      this->set_angle(angle);
      kernels->kernels_.push_back(std::make_pair(axis*angle , new bvpl_kernel(this->create())));
    }
  }


  //when zenith is pi/2 we only traverse half a hemisphere
  phi = float(vnl_math::pi_over_2);
  theta =float(vnl_math::pi_over_2);
  for (;theta < float(vnl_math::pi_over_2); theta +=theta_res)
  {
    axis[0] = float(std::cos(theta) * std::sin(phi));
    axis[1] = float(std::sin(theta) * std::sin(phi));
    axis[2] = float(std::cos(phi));
    this->set_rotation_axis(axis);
    for (float angle = 0.0f; angle < 2.0f * float(vnl_math::pi); angle+=this->angular_resolution_)
    {
      this->set_angle(angle);
      kernels->kernels_.push_back(std::make_pair(axis , new bvpl_kernel(this->create())));
    }
  }
  return kernels;
}

//: Creates a vector of kernels according to given  azimuthal and elevation resolution, and angle of rotation= angular_resolution_
bvpl_kernel_vector_sptr bvpl_edge2d_kernel_factory::create_kernel_vector(float pi, float phi)
{
  //to be implemented
  return 0;
}

//: Creates a vector of kernels  according to given azimuthal, levation, and angular resolution
bvpl_kernel_vector_sptr bvpl_edge2d_kernel_factory::create_kernel_vector(float pi, float phi, float angular_res)
{
  // to be implemented - FIXME
  return 0;
}

#endif
