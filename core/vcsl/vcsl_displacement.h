// This is vxl/vcsl/vcsl_displacement.h
#ifndef vcsl_displacement_h
#define vcsl_displacement_h
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
// 2002/01/22 Peter Vanroose - return type of execute() and inverse() changed to non-ptr
// 2002/01/28 Peter Vanroose - vcl_vector member point_ changed to non-ptr
// \endverbatim


#include <vcsl/vcsl_displacement_sptr.h>
#include <vcsl/vcsl_rotation.h>

//: Affine rotation
// A displacement rotates a point with a given angle around an axis defined by
// a point and a unit vector
class vcsl_displacement
  :public vcsl_rotation
{
 public:
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  //: Default constructor
  explicit vcsl_displacement(void) {}

  //: Destructor
  virtual ~vcsl_displacement() {}

  //***************************************************************************
  // Status report
  //***************************************************************************

  //: Is `this' correctly set ?
  virtual bool is_valid(void) const;

  //***************************************************************************
  // Transformation parameters
  //***************************************************************************

  //: Set the point for a static displacement
  virtual void set_static_point(vnl_vector<double> const& new_point);

  //: Set the variation of the point of the axis along the time
  virtual void set_point(list_of_vectors const& new_point);

  //: Return the variation of the point of the axis along the time
  virtual list_of_vectors point(void) const { return point_; }

  //***************************************************************************
  // Basic operations
  //***************************************************************************

  //: Image of `v' by `this'
  //  REQUIRE: is_valid()
  virtual vnl_vector<double> execute(const vnl_vector<double> &v,
                                     double time) const;

  //: Image of `v' by the inverse of `this'
  //  REQUIRE: is_valid()
  //  REQUIRE: is_invertible(time)
  virtual vnl_vector<double> inverse(const vnl_vector<double> &v,
                                     double time) const;
 protected:

  //: Compute the value of the vector at time `time'
  virtual vnl_vector<double> vector_value(double time) const;

  //: Variation of the point of the axis along the time
  list_of_vectors point_;
};

#endif // vcsl_displacement_h
