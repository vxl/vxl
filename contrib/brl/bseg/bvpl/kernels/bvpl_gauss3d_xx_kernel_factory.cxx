#include <iostream>
#include <algorithm>
#include <cmath>
#include "bvpl_gauss3d_xx_kernel_factory.h"
//:
// \file

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_math.h>
#include <vnl/vnl_float_3.h>

// Default Constructor
bvpl_gauss3d_xx_kernel_factory::bvpl_gauss3d_xx_kernel_factory()
{
  sigma1_ = 0.0f;
  sigma2_ = 0.0f;
  sigma3_ = 0.0f;
  angular_resolution_ = 0;
  rotation_axis_ = canonical_rotation_axis_;
  angle_ = 0.0f;
}

//: Constructs a kernel form gaussian spheroid with sigma parameters s1 and s2. i.e. Cov is diagonal with entries s1, s2, s2
bvpl_gauss3d_xx_kernel_factory::bvpl_gauss3d_xx_kernel_factory(float s1, float s2)
{
  //set variances of kernel
  sigma1_ = s1;
  sigma2_ = s2;
  sigma3_ = s2;

  //this skernel is symmetric around main axis
  angular_resolution_=0;

   //initialize variables
  angle_ = 0.0f;
  rotation_axis_ = canonical_rotation_axis_;

  //create the default kernel
  create_canonical();
}

//: Constructs a kernel form gaussian ellipsoid with sigma parameters s1, s2 and s3. i.e. Cov is diagonal with entries s1, s2, s3
bvpl_gauss3d_xx_kernel_factory::bvpl_gauss3d_xx_kernel_factory(float s1, float s2, float s3)
{
  //set variances of kernel
  sigma1_ = s1;
  sigma2_ = s2;
  sigma3_ = s3;

  //this value is a meant as a limit there is not theoretical meaning to it
  angular_resolution_= float(vnl_math::pi/16.0);

  //initialize variables
  angle_ = 0.0f;
  rotation_axis_ = canonical_rotation_axis_;

  //create the default kernel
  create_canonical();
}

static inline float second_power(float x) { return x*x; }
static inline float third_power(float x) { return x*x*x; }

void bvpl_gauss3d_xx_kernel_factory::create_canonical()
{
  float var1 = second_power(sigma1_);
  float var2 = second_power(sigma2_);
  float var3 = second_power(sigma3_);

  //The size of the kernel is limited. If width or height of the kernel is too large,
  //the user should subsample the image/grid
  if ( (sigma1_ > max_size_) || (sigma2_ > max_size_) || (sigma3_ > max_size_) )
  {
    std::cerr<< "Warning, kernel is too large. You should subsample image. Processing may take a long time.\n";
  }

  typedef vgl_point_3d<float> point_3d;
  typedef bvpl_kernel_dispatch dispatch;

  int min_x = -int(4.0f*sigma1_+0.01f);
  int max_x =  int(4.0f*sigma1_+0.01f);
  int min_y = -int(4.0f*sigma2_+0.01f);
  int max_y =  int(4.0f*sigma2_+0.01f);
  int min_z = -int(4.0f*sigma3_+0.01f);
  int max_z =  int(4.0f*sigma3_+0.01f);

  // Compute the first entry on the Hessian of an independent Gaussian with variances (v1, v, v)

  //constant expressions
  float w = std::sqrt(third_power(2.0f*float(vnl_math::pi))*(1.0f/var1*var2*var3));
  float one_over_v1= 1.0f/var1;
  for (int x=min_x; x<= max_x; x++)
  {
    float x_2_over_v1 = second_power(float(x))/var1;
    float diff_term =  one_over_v1 * (x_2_over_v1 - 1.0f) ;

    for (int y= min_y; y<= max_y; y++)
    {
      for (int z= min_z; z<= max_z; z++)
      {
        float exponential = std::exp( -0.5f * (x_2_over_v1 + second_power(float(y))/var2 + second_power(float(z))/var3));
        canonical_kernel_.emplace_back(point_3d(float(x),float(y),float(z)), dispatch((exponential/w)*diff_term));
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

#if 0 // commented out

/******************Batch Methods ***********************/

//: Creates a vector of kernels with azimuthal(\theta) and elevation(\phi) resolution equal to pi/4.
//  This uses spherical coordinates where \theta \in  [0,2\pi) and \phi \in [0,pi/2)
//  This batch method is specific to a kernel with two equal sides. the reason for this is that in current
//  applications there is no preference in direction other that the orientation of the kernel.
//  A batch method for a "scalene" kernel requires rotation around its main axis.
bvpl_kernel_vector_sptr bvpl_gauss3d_xx_kernel_factory::create_kernel_vector()
{
  bvpl_kernel_vector_sptr kernels = new bvpl_kernel_vector();
  float theta_res = float(vnl_math::pi_over_4); //azimuth; phi_res = zenith (from the pole)
  vnl_float_3 axis;
  float theta = 0.0f;
  float phi = 0.0f;

  //when zenith angle is 0
  axis[0] =0.0f;
  axis[1] =0.0f;
  axis[2] =1.0f;
  this->set_rotation_axis(axis);
  kernels->kernels_.push_back(std::make_pair(axis , new bvpl_kernel(this->create())));

  //when zenith is pi/4 traverse all hemisphere
  phi = float(vnl_math::pi_over_4);
  for (;theta < 2.0f*float(vnl_math::pi)-1e-5; theta +=theta_res)
  {
    axis[0] = std::cos(theta) * std::sin(phi);
    axis[1] = std::sin(theta) * std::sin(phi);
    axis[2] = std::cos(phi);
    this->set_rotation_axis(axis);
    kernels->kernels_.push_back(std::make_pair(axis , new bvpl_kernel(this->create())));
  }

  //when zenith is pi/2 we only traverse half a hemisphere
  phi = float(vnl_math::pi_over_2);
  theta =0.0f;
  for (;theta < float(vnl_math::pi)-1e-5; theta +=theta_res)
  {
    axis[0] = float(std::cos(theta) * std::sin(phi));
    axis[1] = float(std::sin(theta) * std::sin(phi));
    axis[2] = float(std::cos(phi));
    this->set_rotation_axis(axis);
    kernels->kernels_.push_back(std::make_pair(axis , new bvpl_kernel(this->create())));
   }

  return kernels;
}

//: Creates a vector of kernels according to given azimuthal and elevation resolution, and with angle of rotation = angular_resolution_
bvpl_kernel_vector_sptr bvpl_gauss3d_xx_kernel_factory::create_kernel_vector(float pi, float phi)
{
  //to be implemented
  return 0;
}

//: Creates a vector of kernels  according to given azimuthal, levation, and angular resolution
bvpl_kernel_vector_sptr bvpl_gauss3d_xx_kernel_factory::create_kernel_vector(float pi, float phi, float angular_res)
{
  //to be implemented
  return 0;
}

#endif // 0
