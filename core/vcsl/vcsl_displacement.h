#ifndef VCSL_DISPLACEMENT_H
#define VCSL_DISPLACEMENT_H
//*****************************************************************************
//
// .NAME vcsl_displacement - Affine rotation
// .LIBRARY vcsl
// .HEADER  vxl Package
// .INCLUDE vcsl/vcsl_displacement.h
// .FILE    vcsl/vcsl_displacement.cxx
//
// .SECTION Description
// A displacement rotates a point with a given angle around an axis defined by
// a point and a unit vector
//
// .SECTION Author
// François BERTEL
//
// .SECTION Modifications
// 2000/08/06 François BERTEL Creation.
//*****************************************************************************

#include <vcsl/vcsl_displacement_ref.h>
#include <vcsl/vcsl_rotation.h>

#include <vnl/vnl_quaternion.h>

class vcsl_displacement
  :public vcsl_rotation
{
public:
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  //: Default constructor
  explicit vcsl_displacement(void);

  //: Destructor
  virtual ~vcsl_displacement();

  //***************************************************************************
  // Status report
  //***************************************************************************

  //: Is `this' correctly set ?
  virtual bool is_valid(void) const;

  //***************************************************************************
  // Transformation parameters
  //***************************************************************************

  //: Set the point for a static displacement
  virtual void set_static_point(vnl_vector<double> &new_point);

  //: Set the variation of the point of the axis along the time
  virtual void set_point(list_of_vectors &new_point);

  //: Return the variation of the point of the axis along the time
  virtual list_of_vectors *point(void) const;
  
  //***************************************************************************
  // Basic operations
  //***************************************************************************

  //: Image of `v' by `this'
  //: REQUIRE: is_valid()
  virtual vnl_vector<double> *execute(const vnl_vector<double> &v,
                                      const double time) const;

  //: Image of `v' by the inverse of `this'
  //: REQUIRE: is_valid()
  //: REQUIRE: is_invertible(time)
  virtual vnl_vector<double> *inverse(const vnl_vector<double> &v,
                                      const double time) const;
protected:

  //: Compute the value of the vector at time `time'
  virtual vnl_vector<double> *vector_value(const double time) const;

  //: Variation of the point of the axis along the time
  list_of_vectors *_point;
};

#endif // #ifndef VCSL_DISPLACEMENT_H
