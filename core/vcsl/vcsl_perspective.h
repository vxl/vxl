#ifndef vcsl_perspective_h
#define vcsl_perspective_h

//:
// \file
// \author François BERTEL
// Modifications
// \verbatim
// 2000/08/23 François BERTEL Creation.
// 2001/04/10 Ian Scott (Manchester) Converted perceps header to doxygen
// 2002/01/22 Peter Vanroose - return type of execute() and inverse() changed to non-ptr
// \endverbatim


#include <vcsl/vcsl_perspective_sptr.h>

#include <vcsl/vcsl_spatial_transformation.h>
//: Perspective projection transformation
// This is a perspective projection from a 3D cartesian coordinate system to a
// 2D cartesian coordinate system, parametrized by the focal exprimed in
// meters. The projection is along the z axis. The center of the projection is
// the origin of the 3D frame. The projection plane is in the half-space z<0.
class vcsl_perspective
  :public vcsl_spatial_transformation
{
public:
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  //: Default constructor.
  explicit vcsl_perspective(void);

  //: Destructor
  virtual ~vcsl_perspective();

  //***************************************************************************
  // Status report
  //***************************************************************************

  //: Is `this' invertible at time `time'? Never !
  //  REQUIRE: valid_time(time)
  virtual bool is_invertible(const double time) const;

  //: Is `this' correctly set ?
  virtual bool is_valid(void) const;

  //***************************************************************************
  // Transformation parameters
  //***************************************************************************

  //: Set the focal in meters of a static perspective projection
  virtual void set_static(const double new_focal);

  //: Set the focal variation along the time in meters
  virtual void set_focal(list_of_scalars &new_focal);

  //: Return the focal variation along the time in meters
  virtual list_of_scalars *focal(void) const;

  //***************************************************************************
  // Basic operations
  //***************************************************************************

  //: Image of `v' by `this'
  //  REQUIRE: is_valid()
  //  REQUIRE: v.size()==3
  virtual vnl_vector<double> execute(const vnl_vector<double> &v,
                                     const double time) const;

  //: Image of `v' by the inverse of `this'
  //  REQUIRE: is_valid()
  //  REQUIRE: is_invertible(time) and v.size()==2
  //  The first pre-condition is never true. You can not use this method
  virtual vnl_vector<double> inverse(const vnl_vector<double> &v,
                                     const double time) const;

protected:

  //: Compute the parameter at time `time'
  virtual double focal_value(const double time) const;

  //: Angle variation along the time
  list_of_scalars *focal_;
};

#endif // vcsl_perspective_h
