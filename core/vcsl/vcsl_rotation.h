#ifndef VCSL_ROTATION_H
#define VCSL_ROTATION_H

//:
// \file 
// \author François BERTEL
// Modifications
// \verbatim
// 2000/06/28 François BERTEL Creation. Adapted from IUE
// 10/4/2001 Ian Scott (Manchester) Coverted perceps header to doxygen
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
  explicit vcsl_rotation(void);

  //: Destructor
  virtual ~vcsl_rotation();

  //***************************************************************************
  // Status report
  //***************************************************************************

  //: Is `this' invertible at time `time'?
  //: REQUIRE: valid_time(time)
  virtual bool is_invertible(const double time) const;

  //: Is `this' correctly set ?
  virtual bool is_valid(void) const;

  //: Are `new_vector' a list of unit vectors ?
  virtual bool are_unit_axes(list_of_vectors &new_axis) const;

  //: Is `this' a 2D rotation ?
  virtual bool is_2d(void) const;

  //: Is `this' a 3D rotation ?
  virtual bool is_3d(void) const;

  //***************************************************************************
  // Status setting
  //***************************************************************************

  //: Set `this' as a 2D rotation
  virtual void set_2d(void);

  //: Set `this' as a 3D rotation
  virtual void set_3d(void);

  //***************************************************************************
  // Transformation parameters
  //***************************************************************************

  //: Set the parameters of a static 2D rotation
  virtual void set_static_2d(const double new_angle);

  //: Set the parameters of a static rotation
  virtual void set_static(const double new_angle,
                          vnl_vector<double> &new_axis);

  //: Set the angle variation along the time in radians
  virtual void set_angle(list_of_scalars &new_angle);

  //: Return the angle variation along the time in radians
  virtual list_of_scalars *angle(void) const;

  //: Set the direction vector variation along the time
  //: REQUIRE: are_unit_vectors(new_vector)
  virtual void set_axis(list_of_vectors &new_axis);

  //: Return the direction variation along the time
  virtual list_of_vectors *axis(void) const;
  
  //***************************************************************************
  // Basic operations
  //***************************************************************************
  
  //: Image of `v' by `this'
  //: REQUIRE: is_valid()
  //: REQUIRE: (is_2d()&&v.size()==2)||(is_3d()&&v.size()==3)
  virtual vnl_vector<double> *execute(const vnl_vector<double> &v,
                                      const double time) const;
  
  //: Image of `v' by the inverse of `this'
  //: REQUIRE: is_valid()
  //: REQUIRE: is_invertible(time)
  //: REQUIRE (is_2d()&&v.size()==2)||(is_3d()&&v.size()==3)
  virtual vnl_vector<double> *inverse(const vnl_vector<double> &v,
                                      const double time) const;

protected:
  //: Compute the value of the quaternion at time `time'
  vnl_quaternion<double> *quaternion(const double time) const;

  //: False if `this' is a 3D rotation, true if `this' is a 2D rotation
  bool _mode_2d;

  //: Angle variation along the time in radians
  list_of_scalars *angle_;

  //: Direction vector variation along the time
  list_of_vectors *axis_;
};

#endif // #ifndef VCSL_ROTATION_H
