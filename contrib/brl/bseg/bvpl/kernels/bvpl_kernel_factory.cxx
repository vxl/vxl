#include <map>
#include <iostream>
#include <algorithm>
#include <limits>
#include <cmath>
#include "bvpl_kernel_factory.h"
//:
// \file
#include <vnl/vnl_vector_fixed.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


//: Returns a kernel along the local rotation_axis_ and rotated around this axis an amount angle_
//  The user can modified the axis and angle using set_rotation_axis() and set_angle()
bvpl_kernel
bvpl_kernel_factory::create()
{
  kernel_type final_float_kernel = rotate(angle_);
  bvpl_kernel kernel(interpolate(final_float_kernel), rotation_axis_, parallel_axis_, angle_,dim(), min_point_, max_point_,factory_name_,voxel_length_);
  kernel.float_kernel_ = final_float_kernel;
  kernel.set_xml_element(xml_element());

  return kernel;
}

//: Returns a kernel along the rotation_axis and rotated around this axis an amount angle
bvpl_kernel
bvpl_kernel_factory::create(vnl_float_3 rotation_axis, float  /*angle*/)
{
  this->set_rotation_axis(rotation_axis);
  kernel_type final_float_kernel = rotate(angle_);
  bvpl_kernel kernel(interpolate(final_float_kernel), rotation_axis_, parallel_axis_, angle_, dim(), min_point_, max_point_,factory_name_,voxel_length_);
  kernel.float_kernel_ = final_float_kernel;
  kernel.set_xml_element(xml_element());

  return kernel;
}

//: Rounds coordinates of kernel to the nearest integer
bvpl_kernel_iterator
bvpl_kernel_factory::interpolate(kernel_type const& kernel)
{
  auto kernel_it = kernel.begin();
  std::vector<std::pair<vgl_point_3d<int>, bvpl_kernel_dispatch> >  kernel_out;

    //Kernels shouldn't get any bigger than this, so this initial values work
  int max_x =-100;
  int max_y =-100;
  int max_z =-100;
  int min_x = 100;
  int min_y = 100;
  int min_z = 100;

  for (; kernel_it != kernel.end(); ++kernel_it)
  {
    int x0 = (int)std::floor((kernel_it->first).x()+0.5f);
    int y0 = (int)std::floor((kernel_it->first).y()+0.5f);
    int z0 = (int)std::floor((kernel_it->first).z()+0.5f);
    kernel_out.emplace_back(vgl_point_3d<int>(x0,y0,z0), (kernel_it->second));

    if ( x0 > max_x) max_x =  x0;
    if ( y0 > max_y) max_y =  y0;
    if ( z0 > max_z) max_z =  z0;

    if ( x0 < min_x) min_x =  x0;
    if ( y0 < min_y) min_y =  y0;
    if ( z0 < min_z) min_z =  z0;
  }

  max_point_.set(max_x,max_y,max_z);
  min_point_.set(min_x,min_y,min_z);

  return kernel_out;
}

//: Rotates  the kernel so that its canonical axis is set to "axis"
void bvpl_kernel_factory::set_rotation_axis( vnl_float_3 rotation_axis)
{
  // rotation axis should be unit vector
  float mag = rotation_axis.magnitude();
  if (mag > std::numeric_limits<float>::epsilon())
    rotation_axis /= mag;
  else {
    std::cout << "Rotation axis magnitude is zero, returning withount modifycation of kernel\n";
    return ;
  }

  rotation_axis_ = rotation_axis;

  //spherical coordinates of the rotation axis.
  float radius = 1.0f,
        theta = std::atan2(rotation_axis_[1],rotation_axis_[0]), //azimuth,
        phi = std::acos(rotation_axis_[2]/radius); //zenith

  //set kernel back to its canonical form, since the rotation is calculated from its canonical position
  kernel_ = canonical_kernel_;

  //construct a rotation to rotate vector a to vector b;
  //if a and b are oposite, then this rotation is ambiguous (infinitely many axes of rotation)
  //and we will choose to reflect
 if (vnl_cross_3d(canonical_rotation_axis_, rotation_axis_).two_norm()<1e-2)
 {
    if (dot_product(canonical_rotation_axis_, rotation_axis_) < 0) //opposite vectors
    {
      vnl_float_3 axis_perp(float(std::cos(theta)*std::sin(phi+vnl_math::pi_over_2)),
                                          float(std::sin(theta)*std::sin(phi+vnl_math::pi_over_2)),
                                          float(std::cos(phi+vnl_math::pi_over_2)));
      vgl_rotation_3d<float> r_align(vnl_quaternion<float>(axis_perp, float(vnl_math::pi)));
      kernel_ = rotate(r_align);
      return;
    }
    else //vectors are identical
      return;
  }

  vgl_rotation_3d<float> r_align(canonical_rotation_axis_, rotation_axis_);

  kernel_= rotate(r_align);

  // Rotate parallel axis to determine the zero rotation along rotation axis
  // This makes no sense if the angular resolution is zero
  if (angular_resolution_ < std::numeric_limits<float>::epsilon())
  {
    std::cerr << "Angular resolution is 0, if this is intentional, then the kernel factory is not checking\n";
  //  return;
  }

  parallel_axis_ = r_align.as_matrix() * canonical_parallel_axis_;

  //spherical coordinates of the parallel axis.
  float /* theta_p = std::atan2(parallerl_axis[1],parallerl_axis[0]), //azimuth, unused */
        phi_p = std::acos(parallel_axis_[2]/radius); //zenith

  //parallel axis needs to be rotated to have same polar angle as rotation axis

  //If the rotation axis is pointing to the poles, there is an exception
  if ((rotation_axis_ != vnl_vector_fixed<float, 3>(0.0f, 0.0f, 1.0f))&&
      (rotation_axis_ != vnl_vector_fixed<float, 3>(0.0f, 0.0f, -1.0f)))
  {
    float correction_phi = phi_p - phi;
    if (correction_phi <  std::numeric_limits<float>::epsilon())
      return;
    vgl_rotation_3d<float> r_correct(vnl_quaternion<float>(rotation_axis, correction_phi));
    // rotate correction_phi around new axis of rotation. This position is the 0-rotation.
    kernel_ = rotate(r_correct);
    parallel_axis_ = r_correct.as_matrix() * parallel_axis_;
    return;
  }
  else
  { //make sure parallel axis is aligned with y-axis
    for ( unsigned i = 0; i < 3; ++i ){
      if ( std::abs(parallel_axis_[i] - canonical_parallel_axis_[i]) >  std::numeric_limits<float>::epsilon() ){
        std::cerr << "Error when aligning rotation axis to the z axis\n" ;
        std::cout << "Parallel axis: "<< parallel_axis_ << std::endl
                 << "Canonical parallel axis " << canonical_parallel_axis_ << std::endl;
        return;
      }
    }
  }

  return;
}

//: Rotates "class-kernel_" around "class-rotation_axis_"  by an "angle"
bvpl_kernel_factory::kernel_type
bvpl_kernel_factory::rotate(float angle)
{
  //construct a quternion to represent the rotation
  float mag = rotation_axis_.magnitude();
  if (angle > std::numeric_limits<float>::epsilon())
  {
    if (mag > double(0))
    {
      vnl_quaternion<float> q(rotation_axis_/mag,angle);
      vgl_rotation_3d<float> r(q);
      parallel_axis_ = r.as_matrix()*parallel_axis_;
      return this->rotate(r);
    }
    else
      std::cout << "magnitude of rotation axis is zero, returning without modifying kernel\n";

    return kernel_;
  }
  else
    return kernel_;
}

//: Rotates "class-kernel_" using the given rotation matrix
bvpl_kernel_factory::kernel_type
bvpl_kernel_factory::rotate(const vgl_rotation_3d<float>& R)
{
#ifdef DEBUG
  std::cout << "Rotating kernel using the following matrix" << std::endl
           << R.as_matrix() << std::endl;
#endif

  auto kernel_it =this->kernel_.begin();
  std::vector<std::pair<vgl_point_3d<float>, bvpl_kernel_dispatch> > kernel;

  //for efficiency and accuracy, get the rotation matrix of R and use the matrix for multiplicatiom
  vnl_matrix_fixed<float,3,3> R_as_matrix = R.as_matrix() ;


  for (; kernel_it != kernel_.end(); ++kernel_it)
  {
    //Rotate, maintaining floating point values
    vnl_float_3 new_coord = R_as_matrix* vnl_float_3(float((*kernel_it).first.x()),
                                                     float((*kernel_it).first.y()),
                                                     float((*kernel_it).first.z()));

    kernel.emplace_back(vgl_point_3d<float>(new_coord[0],new_coord[1],new_coord[2]), (kernel_it->second));

    // As it is implemented now, if many points round to a single integer, then that integer is used multiple times
    // This may be a good solution, and avoids the problem of getting unequal number of symbols
  }

  return kernel;
}


vgl_vector_3d<int> bvpl_kernel_factory::dim()
{
  int x,y,z;

  x = max_point_.x()-min_point_.x() + 1;
  y = max_point_.y()-min_point_.y() + 1;
  z = max_point_.z()-min_point_.z() + 1;

  return {x,y,z};
}
