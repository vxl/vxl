// This is core/vcsl/vcsl_rotation.h
#ifndef vcsl_rotation_h_
#define vcsl_rotation_h_
//:
// \file
// \brief Rotation transformation (either 2D or 3D)
// \author François BERTEL
//
// \verbatim
//  Modifications
//   2000/06/28 François BERTEL Creation. Adapted from IUE
//   2001/04/10 Ian Scott (Manchester) Converted perceps header to doxygen
//   2002/01/22 Peter Vanroose - return type of quaternion(), execute() and inverse() changed to non-ptr
//   2002/01/28 Peter Vanroose - vcl_vector members angle_ and axis_ changed to non-ptr
//   2004/09/17 Peter Vanroose - made angle() non-virtual - it just returns a member and should not be overloaded
// \endverbatim

#include <vcsl/vcsl_spatial_transformation.h>
#include <vcsl/vcsl_rotation_sptr.h>
#include <vnl/vnl_quaternion.h>

//: Rotation transformation (either 2D or 3D).
// A rotation rotates a point around an axis passing through the origin.
// For a more general rotation (affine rotation or displacement), see
// the derived class \b vcsl_displacement
class vcsl_rotation
  : public vcsl_spatial_transformation
{
 public:
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  //: Default constructor. Sets 3D rotation mode
  vcsl_rotation() : mode_2d_(false) {}

  // Destructor
  virtual ~vcsl_rotation() {}

  //***************************************************************************
  // Status report
  //***************************************************************************

  //: Is `this' invertible at time `time'?
  //  REQUIRE: valid_time(time)
  // Pure virtual function of vcsl_spatial_transformation
  virtual bool is_invertible(double /*time*/) const { return true; }

  //: Is `this' correctly set ?
  // Virtual function of vcsl_spatial_transformation
  virtual bool is_valid() const
  { return vcsl_spatial_transformation::is_valid() &&
           this->duration()==axis_.size()  &&
           this->duration()==angle_.size(); }

  //: Are `new_vector' a list of unit vectors ?
  bool are_unit_axes(list_of_vectors const& new_axis) const;

  //: Is `this' a 2D rotation ?
  bool is_2d() const { return mode_2d_; }

  //: Is `this' a 3D rotation ?
  bool is_3d() const { return !mode_2d_; }

  //***************************************************************************
  // Status setting
  //***************************************************************************

  //: Set `this' as a 2D rotation
  void set_2d() { mode_2d_=true; }

  //: Set `this' as a 3D rotation
  void set_3d() { mode_2d_=false; }

  //***************************************************************************
  // Transformation parameters
  //***************************************************************************

  //: Set the parameters of a static 2D rotation
  void set_static_2d(double new_angle);

  //: Set the parameters of a static rotation
  void set_static(double new_angle, vnl_vector<double> const& new_axis);

  //: Set the angle variation along the time in radians
  void set_angle(list_of_scalars const& new_angle) { angle_=new_angle; }

  //: Return the angle variation along the time in radians
  list_of_scalars angle() const { return angle_; }

  //: Set the direction vector variation along the time
  //  REQUIRE: are_unit_vectors(new_vector)
  void set_axis(list_of_vectors const& new_axis);

  //: Return the direction variation along the time
  list_of_vectors axis() const { return axis_; }

  //***************************************************************************
  // Basic operations
  //***************************************************************************

  //: Image of `v' by `this'
  //  REQUIRE: is_valid()
  //  REQUIRE: (is_2d()&&v.size()==2)||(is_3d()&&v.size()==3)
  // Pure virtual function of vcsl_spatial_transformation
  virtual vnl_vector<double> execute(const vnl_vector<double> &v,
                                     double time) const;

  //: Image of `v' by the inverse of `this'
  //  REQUIRE: is_valid()
  //  REQUIRE: is_invertible(time)
  //  REQUIRE (is_2d()&&v.size()==2)||(is_3d()&&v.size()==3)
  // Pure virtual function of vcsl_spatial_transformation
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

#endif // vcsl_rotation_h_
