// This is brl/bseg/bvpl/kernels/bvpl_gauss3d_kernel_factory.h
#ifndef bvpl_gauss3d_kernel_factory_h
#define bvpl_gauss3d_kernel_factory_h
//:
// \file
// \brief A factory of the kernel of type first (x) derivative of 3d gaussian
// \author Isabel Restrepo mir@lems.brown.edu
// \date  October 7, 2010
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include "bvpl_kernel_factory.h"

//: A class to generate kernels of the  first derivative of gaussian.
// This kernel can be constructed from 2 sigma values or 3 sigma values. In the first case the gaussian is a spheroid,
// i.e. 2 of the radii are equal. In such case the kernel is completely described by an axis of rotation (which is aligned with the non-equal radii)
// and two variance parameters, sigma1 and sigma2
// In the second case when all 3 sigmas are different then the gaussian kernel is a "triaxial ellipsoid" and it is
// determined by axis of rotation, angle of rotation about such axis, sigma1, sigma2, sigma3.
// The case of an isotropic has not been implemented because is it not needed at the moment, but the code is reusable for such case
class bvpl_gauss3d_kernel_factory : public bvpl_kernel_factory
{
 public:

  //:Default Constructor
  bvpl_gauss3d_kernel_factory();

  //: Constructs a kernel form gaussian spheroid with sigma parameters s1 and s2. i.e. Cov is diagonal with entries s1, s2, s2
  bvpl_gauss3d_kernel_factory(float s1, float s2);

  //: Constructs a kernel form gaussian ellipsoid with sigma parameters s1, s2 and s3. i.e. Cov is diagonal with entries s1, s2,s3
  bvpl_gauss3d_kernel_factory(float s1, float s2, float s3,float supp1 = 2.0, float supp2 = 1.0, float supp3 = 1.0);

  ~bvpl_gauss3d_kernel_factory() override = default;

  static std::string name() {return "gauss"; }

#if 0
  /******************Batch Methods ***********************/
  //: Creates a vector of kernels with azimuthal and elevation resolution equal to pi/4, and with angle of rotation = angular_resolution_
  virtual bvpl_kernel_vector_sptr create_kernel_vector();

  //: Creates a vector of kernels according to given  azimuthal and elevation resolution, and with angle of rotation = angular_resolution_
  virtual bvpl_kernel_vector_sptr create_kernel_vector(float pi, float phi);

  //: Creates a vector of kernels  according to given azimuthal, levation, and angular resolution
  virtual bvpl_kernel_vector_sptr create_kernel_vector(float pi, float phi, float angular_res);
#endif
 private:

  //:Creates a 2d edge kernel
  void create_canonical() override;

  float sigma1_;
  float sigma2_;
  float sigma3_;
  float supp1_;
  float supp2_;
  float supp3_;

  static const unsigned max_size_ = 71;
};


#endif
