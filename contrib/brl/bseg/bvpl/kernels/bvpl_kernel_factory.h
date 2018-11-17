// This is brl/bseg/bvpl/kernels/bvpl_kernel_factory.h
#ifndef bvpl_kernel_base_h
#define bvpl_kernel_base_h
//:
// \file
// \brief  A base class for 3D kernel. The kernel is fully described by a rotation axis and an angle
//  The children of this classes must create a kernel on a "canonical form" i.e. with a default
//  axis of rotation and a complementary vector here called canonical_paralle_axis_.
//
//  If this two vector are seen as points on the unit sphere, then they lie on the same
//  parallel (constant polar angle). These vectors determine the zero rotation position as follows:
//  *  The rotation that transforms the canonical_rotation_axis to a new specified rotation axis,
//     is applied to the kernel (Lets call such rotation R).
//  *  The kernel is rotate around its new rotation such that R*canonical_parallel_axis achieves
//     the same polar angle as the new rotation axis
//
//  Usage:
//  - The user can create a kernel in its canonical form through children constructors
//  - Then, the user call set_rotation_axis(), set_angle() and create() and if one wishes to use
//    another angle with the same axis, it suffices to call set_angle(angle) and create().
//    This will save computation time since the rotation_axis has already been set.
//  - Alternatively, the user can call create(axis, angle). This transforms the canonical kernel according
//    to new parameters
//
// \author Isabel Restrepo mir@lems.brown.edu
// \date  May 29, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#include <map>
#include <utility>
#include "bvpl_kernel.h"
#include "bvpl_kernel_iterator.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_float_3.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vbl/vbl_ref_count.h>

class bvpl_kernel_factory;
typedef vbl_smart_ptr<bvpl_kernel_factory> bvpl_kernel_factory_sptr;


//: A factory of bvpl_kernels
class bvpl_kernel_factory: public vbl_ref_count
{
 public:
  //Default constructor. Initialize constant member variables
  bvpl_kernel_factory(): canonical_rotation_axis_(vnl_float_3(1,0,0)), canonical_parallel_axis_(vnl_float_3(0,1,0)){}

  ~bvpl_kernel_factory() override = default;

  //: Returns a kernel described by class variables rotation_axis_, \p angle_
  //  The user can modified the axis and angle using set_rotation_axis() and set_angle()
  bvpl_kernel create();

  //: Returns a kernel described by inputs; rotation_axis and angle
  bvpl_kernel create(vnl_float_3 rotation_axis, float angle);

  //: Sets the rotation axis of this kernel
  void set_rotation_axis(vnl_float_3 rotation_axis);

  //: Sets rotation angle of this kernel
  void set_angle(float angle) { angle_ =angle; }

  //: Returns the current alignment axis
  vnl_float_3 axis() const { return rotation_axis_; }

  //: Return current rotation angle around axis()
  float angle() const { return angle_; }

  //: Returns angular resolution around rotation axis
  float angular_resolution() const { return angular_resolution_; }

  //: Return x-y-z maximum coordinate values
  vgl_point_3d<int> max_point() const { return max_point_; }

  //: Return x-y-z minimum coordinate values
  vgl_point_3d<int> min_point() const { return min_point_; }

  //: returns the rectangular dimensions around the kernel center
  vgl_vector_3d<int> dim();

  //: Creates a vector of kernels as specified by func
  template <class F>
  bvpl_kernel_vector_sptr create_kernel_vector(F func);

  //: Return an xml element
  virtual bxml_data_sptr xml_element()
  {
    std::cout << "Calling xml write in parent class, xml data will be NULL" << std::endl;
    return nullptr;
  }

  virtual bvpl_kernel_factory_sptr self(){return this;}

 protected:

  typedef std::vector<std::pair<vgl_point_3d<float>, bvpl_kernel_dispatch> > kernel_type;

  //: The map of 3d positions and their symbols. This kernel has an axis of rotation, but it is always on zero-rotation position
  std::vector<std::pair<vgl_point_3d<float>, bvpl_kernel_dispatch> > kernel_;

  //: The map of 3d positions and their symbols in their canonical form (As specified by children)
  std::vector<std::pair<vgl_point_3d<float>, bvpl_kernel_dispatch> > canonical_kernel_;

  //: The rotation axis for canonical edge
  const vnl_float_3 canonical_rotation_axis_;//(1,0,0);

  //: Rotation axis of kernel_
  vnl_float_3 rotation_axis_;

  vnl_float_3 parallel_axis_;

  // parallel_axis_ define a coordinate system for each kernel.
  // The vector (parallel_axis_ - rotation_axis_) defines the direction of the minor axis of the kernel
  // The zero-rotation for any rotation axis is that for which parallel_axis_ and rotation_axis_ have constant polar angle
  const vnl_float_3 canonical_parallel_axis_;//(0,1,0);

  //: Amounts rotation around rotation_axis_
  float angle_;

  //: Dimensions of the 3D grid
  vgl_point_3d<int> max_point_;

  //: Dimensions of the 3D grid
  vgl_point_3d<int> min_point_;

  //: Angular resolutions
  float angular_resolution_;

  //: Length of a voxel in global coordinates
  double voxel_length_;

  //: Name that identifies the kernel e.g "edge3d", "gauss"
  std::string factory_name_;

  //: Creates canonical(default) kernel.
  //  It is described by a canonical axis of rotation and a canonical parallel axis
  //  This is the main function implemented by the children.
  virtual void create_canonical()=0;

  //: Rounds coordinates of kernel to the nearest integer
  bvpl_kernel_iterator interpolate(kernel_type const& kernel);

  //: Rotates "class-kernel_" around "class-rotation_axis_"  by an "angle"
  kernel_type rotate(float angle);

  //: Rotates "class-kernel_" using the given rotation matrix
  kernel_type rotate(const vgl_rotation_3d<float>& R);
};


template <class F>
bvpl_kernel_vector_sptr bvpl_kernel_factory::create_kernel_vector(F func)
{
  std::vector<vnl_float_3> axes=func.get_axes();
  std::vector<float> angles = func.get_angles();

  bvpl_kernel_vector_sptr vec_kernel=new bvpl_kernel_vector();

  for (unsigned i=0;i<axes.size();i++)
  {
    this->set_rotation_axis(axes[i]);
    this->set_angle(angles[i]);
    vec_kernel->kernels_.push_back(new bvpl_kernel(this->create()));
  }
  return vec_kernel;
}

#endif
