// This is vxl/vcsl/vcsl_rotation.h
#ifndef vcsl_rotation_h
#define vcsl_rotation_h
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author François BERTEL
//
// \verbatim
// Modifications
// 2000/06/28 François BERTEL Creation. Adapted from IUE
// 2001/04/10 Ian Scott (Manchester) Converted perceps header to doxygen
// 2002/01/22 Peter Vanroose - return type of quaternion(), execute() and inverse() changed to non-ptr
// 2002/01/28 Peter Vanroose - vcl_vector members angle_ and axis_ changed to non-ptr
// \endverbatim

#include <vcsl/vcsl_rotation_sptr.h>

#include <vcsl/vcsl_spatial_transformation.h>
#include <vnl/vnl_quaternion.h>

//: Rotation transformation
// A rotation rotate a point around an axis passing through the origin
// For a more general rotation (affine rotation or displacement), see
// a vcsl_displacement
class vcsl_rotation
  : public vcsl_spatial_transformation
{
 public:
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  //: Default constructor. 3D rotation
  explicit vcsl_rotation(void) : mode_2d_(false) {}

  //: Destructor
  virtual ~vcsl_rotation() {}

  //***************************************************************************
  // Status report
  //***************************************************************************

  //: Is `this' invertible at time `time'?
  //  REQUIRE: valid_time(time)
  virtual bool is_invertible(double time) const { return true; }

  //: Is `this' correctly set ?
  virtual bool is_valid(void) const;

  //: Are `new_vector' a list of unit vectors ?
  virtual bool are_unit_axes(list_of_vectors const& new_axis) const;

  //: Is `this' a 2D rotation ?
  virtual bool is_2d(void) const { return mode_2d_; }

  //: Is `this' a 3D rotation ?
  virtual bool is_3d(void) const { return !mode_2d_; }

  //***************************************************************************
  // Status setting
  //***************************************************************************

  //: Set `this' as a 2D rotation
  virtual void set_2d(void) { mode_2d_=true; }

  //: Set `this' as a 3D rotation
  virtual void set_3d(void) { mode_2d_=false; }

  //***************************************************************************
  // Transformation parameters
  //***************************************************************************

  //: Set the parameters of a static 2D rotation
  virtual void set_static_2d(double new_angle);

  //: Set the parameters of a static rotation
  void set_static(double new_angle, vnl_vector<double> const& new_axis);

  //: Set the angle variation along the time in radians
  virtual void set_angle(list_of_scalars const& new_angle);

  //: Return the angle variation along the time in radians
  virtual list_of_scalars angle(void) const { return angle_; }

  //: Set the direction vector variation along the time
  //  REQUIRE: are_unit_vectors(new_vector)
  virtual void set_axis(list_of_vectors const& new_axis);

  //: Return the direction variation along the time
  virtual list_of_vectors axis(void) const { return axis_; }

  //***************************************************************************
  // Basic operations
  //***************************************************************************

  //: Image of `v' by `this'
  //  REQUIRE: is_valid()
  //  REQUIRE: (is_2d()&&v.size()==2)||(is_3d()&&v.size()==3)
  virtual vnl_vector<double> execute(const vnl_vector<double> &v,
                                     double time) const;

  //: Image of `v' by the inverse of `this'
  //  REQUIRE: is_valid()
  //  REQUIRE: is_invertible(time)
  //  REQUIRE (is_2d()&&v.size()==2)||(is_3d()&&v.size()==3)
  virtual vnl_vector<double> inverse(const vnl_vector<double> &v,
                                     double time) const;

 protected:
  //: Compute the value of the quaternion at time `time'
  vnl_quaternion<double> quaternion(double time) const;

  //: False if `this' is a 3D rotation, true if `this' is a 2D rotation
  bool mode_2d_;

  //: Angle variation along the time in radians
  list_of_scalars angle_;

  //: Direction vector variation along the time
  list_of_vectors axis_;
};

#endif // vcsl_rotation_h
