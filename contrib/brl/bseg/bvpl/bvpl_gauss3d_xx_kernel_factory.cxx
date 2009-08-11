#include "bvpl_gauss3d_xx_kernel_factory.h"
//:
// \file

#include <vcl_algorithm.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_float_3.h>

// Default Constructor
bvpl_gauss3d_xx_kernel_factory::bvpl_gauss3d_xx_kernel_factory()
{
  sigma1_ = 0.0f;
  sigma2_ = 0.0f;
  sigma3_ = 0.0f;
  angular_resolution_ = 0;
  canonical_rotation_axis_[0] = 0.0f; canonical_rotation_axis_[1] = 0.0f; canonical_rotation_axis_[2] = 0.0f;
  canonical_parallel_axis_[0] = 0.0f; canonical_parallel_axis_[1] = 0.0f; canonical_parallel_axis_[2] = 0.0f;
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

  //set canonical axis to x-axis
  canonical_rotation_axis_[0] = 1.0f; canonical_rotation_axis_[1] = 0.0f; canonical_rotation_axis_[2] = 0.0f;

  //since rotation around x-axis is symmetric, the parallel axis is of non importance
  canonical_parallel_axis_[0] = 0.0f; canonical_parallel_axis_[1] = 0.0f; canonical_parallel_axis_[2] = 0.0f;

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
  angular_resolution_= vnl_math::pi/16.0 ;

  //set canonical axis to x-axis
  canonical_rotation_axis_[0] = 1.0f; canonical_rotation_axis_[1] = 0.0f; canonical_rotation_axis_[2] = 0.0f;

  //parallel axis is the y-axis
  canonical_parallel_axis_[0] = 0.0f; canonical_parallel_axis_[1] = 1.0f; canonical_parallel_axis_[2] = 0.0f;

  //initialize variables
  angle_ = 0.0f;
  rotation_axis_ = canonical_rotation_axis_;

  //create the default kernel
  create_canonical();
}


void bvpl_gauss3d_xx_kernel_factory::create_canonical()
{
  float var1 = vcl_pow(sigma1_, 2);
  float var2 = vcl_pow(sigma2_, 2);
  float var3 = vcl_pow(sigma3_, 2);

  //The size of the kernel is limited. If widht or height of the kernel is too large,
  //the user should subsample the image/grid
  if ( (sigma1_ > max_size_) || (sigma2_ > max_size_) || (sigma3_ > max_size_) )
  {
    vcl_cerr<< "Warning, kernel is too large. You should subsample image. Processing may take a long time.\n";
  }

  typedef vgl_point_3d<float> point_3d;
  typedef bvpl_kernel_dispatch dispatch;

  int min_x= -1*(4.0*sigma1_);
  int max_x =(4.0*sigma1_);
  int min_y= -1*(4.0*sigma2_);
  int max_y =(4.0*sigma2_);
  int min_z=  -1*(4.0*sigma3_);
  int max_z =(4.0*sigma3_);

  //Compute the first entry on the Hessian of a independent gaussian with variances (v1, v, v)

  //constant expressions
  int dimension = 3;
  float w = vcl_sqrt(vcl_pow(2.0f*float(vnl_math::pi),dimension)* (1.0f/var1*var2*var3) );
  float v1_2 = vcl_pow(var1,2);
  float one_over_v1= 1.0f/var1;
  for (int x=min_x; x<= max_x; x++)
  {
    float x_2_over_v1 = vcl_pow(float(x),2)/var1;
    float diff_term =  one_over_v1 * (x_2_over_v1 - 1.0f) ;

    for (int y= min_y; y<= max_y; y++)
    {
      for (int z= min_z; z<= max_z; z++)
      {
        float exponential = vcl_exp( -0.5 * (x_2_over_v1 + vcl_pow(float(y),2)/var2 + vcl_pow(float(z),2)/var3));
        canonical_kernel_.push_back(vcl_pair<point_3d,dispatch>(point_3d(float(x),float(y),float(z)), dispatch((exponential/w)*diff_term)));
      }
    }
  }

  //set the dimension of the 3-d grid
  max3d_.set(max_x,max_y,max_z);
  min3d_.set(min_x,min_y,min_z);

  //set the current kernel
  kernel_ = canonical_kernel_;

  return;
}


/******************Batch Methods ***********************/

//: Creates a vector of kernels with azimuthal(\phi) and elevation(\theta) resolution equal to pi/4.
//  This uses spherical coordinates where \theta \in  [0,\pi) and \phi \in [0,2\pi)
bvpl_kernel_vector_sptr bvpl_gauss3d_xx_kernel_factory::create_kernel_vector()
{
#if 0
  bvpl_kernel_vector_sptr kernels = new bvpl_kernel_vector();
  vnl_float_3 axis(0.0f, 0.0f, 1.0f);
  //polar phi=0,pi
  this->set_rotation_axis(axis);
  kernels->kernels_.push_back(vcl_make_pair(axis, new bvpl_kernel(this->create())));
  axis = -axis;
  this->set_rotation_axis(axis);
  kernels->kernels_.push_back(vcl_make_pair(axis, new bvpl_kernel(this->create())));

  double theta_res = vnl_math::pi_over_4;
  double phi_res   = vnl_math::pi_over_4;
  // phi=pi/4,pi/2,3pi/4
  for (double phi=vnl_math::pi_over_4; phi<vnl_math::pi-1e-5; phi+=phi_res)
  {
    // theta=0,pi/4,pi/2,3pi/4,pi,5pi/4,3pi/2,7pi/4
    for (double theta=0.0; theta<2.0*vnl_math::pi-1e-5; theta+=theta_res)
    {
      axis[0] = float(vcl_cos(theta) * vcl_sin(phi));
      axis[1] = float(vcl_sin(theta) * vcl_sin(phi));
      axis[2] = float(vcl_cos(phi));
      this->set_rotation_axis(axis);
      kernels->kernels_.push_back(vcl_make_pair(axis, new bvpl_kernel(this->create())));
    }
  }
  return kernels;
#else
  return 0; // to be implemented
#endif
}

//: Creates a vector of kernels according to given  azimuthal and elevation resolutio, and angle of rotation= angular_resolution_
bvpl_kernel_vector_sptr bvpl_gauss3d_xx_kernel_factory::create_kernel_vector(float pi, float phi)
{
  //to be implemented
  return 0;
}

//: Creates a vector of kernels  according to given azimuthal, levation resolutio and angle_res
bvpl_kernel_vector_sptr bvpl_gauss3d_xx_kernel_factory::create_kernel_vector(float pi, float phi, float angular_res)
{
  //to be impemented
  return 0;
}
