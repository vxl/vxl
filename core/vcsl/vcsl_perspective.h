#ifndef VCSL_PERSPECTIVE_H
#define VCSL_PERSPECTIVE_H
//*****************************************************************************
//
// .NAME vcsl_perspective - Perspective projection transformation
// .LIBRARY vcsl
// .HEADER  vxl Package
// .INCLUDE vcsl/vcsl_perspective.h
// .FILE    vcsl/vcsl_perspective.cxx
//
// .SECTION Description
// This is a perspective projection from a 3D cartesian coordinate system to a
// 2D cartesian coordinate system, parametrized by the focal exprimed in
// meters. The projection is along the z axis. The center of the projection is
// the origin of the 3D frame. The projection plane is in the half-space z<0.
//
// .SECTION Author
// François BERTEL
//
// .SECTION Modifications
// 2000/08/23 François BERTEL Creation.
//*****************************************************************************

#include <vcsl/vcsl_perspective_ref.h>

#include <vcsl/vcsl_spatial_transformation.h>

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
  //: REQUIRE: valid_time(time)
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
  //: REQUIRE: is_valid()
  //: REQUIRE: v.size()==3
  virtual vnl_vector<double> *execute(const vnl_vector<double> &v,
                                      const double time) const;

  //: Image of `v' by the inverse of `this'
  //: REQUIRE: is_valid()
  //: REQUIRE: is_invertible(time) and v.size()==2
  //: The first pre-condition is never true. You can not use this method
  virtual vnl_vector<double> *inverse(const vnl_vector<double> &v,
                                      const double time) const;

protected:

  //: Compute the parameter at time `time'
  virtual double focal_value(const double time) const;

  //: Angle variation along the time
  list_of_scalars *_focal;
};

#endif // #ifndef VCSL_PERSPECTIVE_H
