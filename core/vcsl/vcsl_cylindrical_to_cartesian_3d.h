// This is core/vcsl/vcsl_cylindrical_to_cartesian_3d.h
#ifndef vcsl_cylindrical_to_cartesian_3d_h_
#define vcsl_cylindrical_to_cartesian_3d_h_
//:
// \file
// \brief Convert 3D cartesian coordinates to cylindrical coordinates
// \author François BERTEL
//
// \verbatim
//  Modifications
//   2000/06/28 François BERTEL Creation. Adapted from IUE
//   2002/01/22 Peter Vanroose - return type of execute() and inverse() changed to non-ptr
//   2004/09/17 Peter Vanroose - made "instance_" a local static variable of method instance()
// \endverbatim

#include <vcsl/vcsl_spatial_transformation.h>
#include <vcsl/vcsl_cylindrical_to_cartesian_3d_sptr.h>

//: Convert 3D cartesian coordinates to cylindrical coordinates
class vcsl_cylindrical_to_cartesian_3d
  : public vcsl_spatial_transformation
{
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  // Default constructor
  vcsl_cylindrical_to_cartesian_3d() {}

 public:
  // Destructor
  virtual ~vcsl_cylindrical_to_cartesian_3d() {}

  //***************************************************************************
  // Status report
  //***************************************************************************

  //: Is `this' invertible at time `time'?
  //  REQUIRE: valid_time(time)
  // Pure virtual function of vcsl_spatial_transformation
  virtual bool is_invertible(double time) const;

  //: Is `this' correctly set ?
  //  Always true.
  // Virtual function of vcsl_spatial_transformation
  virtual bool is_valid() const { return true; }

  //***************************************************************************
  // Basic operations
  //***************************************************************************

  //: Image of `v' by `this'
  //  REQUIRE: is_valid()
  //  REQUIRE: v.size()==3
  // Pure virtual function of vcsl_spatial_transformation
  virtual vnl_vector<double> execute(const vnl_vector<double> &v,
                                     double time) const;

  //: Image of `v' by the inverse of `this'
  //  REQUIRE: is_valid()
  //  REQUIRE: is_invertible(time)
  //  REQUIRE: v.size()==3
  // Pure virtual function of vcsl_spatial_transformation
  virtual vnl_vector<double> inverse(const vnl_vector<double> &v,
                                     double time) const;

  //***************************************************************************
  // Singleton pattern
  //***************************************************************************

  //: Return the reference to the unique vcsl_length object
  static vcsl_cylindrical_to_cartesian_3d_sptr instance();
};

#endif // vcsl_cylindrical_to_cartesian_3d_h_
