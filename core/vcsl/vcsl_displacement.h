// This is core/vcsl/vcsl_displacement.h
#ifndef vcsl_displacement_h_
#define vcsl_displacement_h_
//:
// \file
// \brief Affine rotation
// \author François BERTEL
//
// \verbatim
//  Modifications
//   2000/06/28 François BERTEL Creation. Adapted from IUE
//   2002/01/22 Peter Vanroose - return type of execute() and inverse() changed to non-ptr
//   2002/01/28 Peter Vanroose - vcl_vector member point_ changed to non-ptr
//   2004/09/17 Peter Vanroose - made point() non-virtual - it just returns a member and should not be overloaded
// \endverbatim

#include <vcsl/vcsl_rotation.h>
#include <vcsl/vcsl_displacement_sptr.h>

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

  // Default constructor
  vcsl_displacement() {}

  // Destructor
  virtual ~vcsl_displacement() {}

  //***************************************************************************
  // Status report
  //***************************************************************************

  //: Is `this' correctly set ?
  // Virtual function of vcsl_spatial_transformation
  virtual bool is_valid() const
  { return vcsl_rotation::is_valid() && this->duration()==point_.size(); }

  //***************************************************************************
  // Transformation parameters
  //***************************************************************************

  //: Set the point for a static displacement
  void set_static_point(vnl_vector<double> const& new_point);

  //: Set the variation of the point of the axis along the time
  void set_point(list_of_vectors const& new_point) { point_=new_point; }

  //: Return the variation of the point of the axis along the time
  list_of_vectors point() const { return point_; }

  //***************************************************************************
  // Basic operations
  //***************************************************************************

  //: Image of `v' by `this'
  //  REQUIRE: is_valid()
  // Pure virtual function of vcsl_spatial_transformation
  virtual vnl_vector<double> execute(const vnl_vector<double> &v,
                                     double time) const;

  //: Image of `v' by the inverse of `this'
  //  REQUIRE: is_valid()
  //  REQUIRE: is_invertible(time)
  // Pure virtual function of vcsl_spatial_transformation
  virtual vnl_vector<double> inverse(const vnl_vector<double> &v,
                                     double time) const;
 protected:

  //: Compute the value of the vector at time `time'
  vnl_vector<double> vector_value(double time) const;

  //: Variation of the point of the axis along the time
  list_of_vectors point_;
};

#endif // vcsl_displacement_h_
