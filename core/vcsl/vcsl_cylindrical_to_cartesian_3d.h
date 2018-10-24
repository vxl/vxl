// This is core/vcsl/vcsl_cylindrical_to_cartesian_3d.h
#ifndef vcsl_cylindrical_to_cartesian_3d_h_
#define vcsl_cylindrical_to_cartesian_3d_h_
//:
// \file
// \brief Convert 3D cartesian coordinates to cylindrical coordinates
// \author Francois BERTEL
//
// \verbatim
//  Modifications
//   2000/06/28 Francois BERTEL Creation. Adapted from IUE
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
  vcsl_cylindrical_to_cartesian_3d() = default;

 public:
  // Destructor
  ~vcsl_cylindrical_to_cartesian_3d() override = default;

  //***************************************************************************
  // Status report
  //***************************************************************************

  //: Is `this' invertible at time `time'?
  //  REQUIRE: valid_time(time)
  // Pure virtual function of vcsl_spatial_transformation
  bool is_invertible(double time) const override;

  //: Is `this' correctly set ?
  //  Always true.
  // Virtual function of vcsl_spatial_transformation
  bool is_valid() const override { return true; }

  //***************************************************************************
  // Basic operations
  //***************************************************************************

  //: Image of `v' by `this'
  //  REQUIRE: is_valid()
  //  REQUIRE: v.size()==3
  // Pure virtual function of vcsl_spatial_transformation
  vnl_vector<double> execute(const vnl_vector<double> &v,
                                     double time) const override;

  //: Image of `v' by the inverse of `this'
  //  REQUIRE: is_valid()
  //  REQUIRE: is_invertible(time)
  //  REQUIRE: v.size()==3
  // Pure virtual function of vcsl_spatial_transformation
  vnl_vector<double> inverse(const vnl_vector<double> &v,
                                     double time) const override;

  //***************************************************************************
  // Singleton pattern
  //***************************************************************************

  //: Return the reference to the unique vcsl_length object
  static vcsl_cylindrical_to_cartesian_3d_sptr instance();
};

#endif // vcsl_cylindrical_to_cartesian_3d_h_
