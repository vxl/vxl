// This is core/vcsl/vcsl_perspective.h
#ifndef vcsl_perspective_h_
#define vcsl_perspective_h_
//:
// \file
// \brief Perspective projection transformation
// \author Francois BERTEL
//
// \verbatim
//  Modifications
//   2000/08/23 Francois BERTEL Creation.
//   2001/04/10 Ian Scott (Manchester) Converted perceps header to doxygen
//   2002/01/22 Peter Vanroose - return type of execute() and inverse() changed to non-ptr
//   2002/01/28 Peter Vanroose - std::vector member focal_ changed to non-ptr
//   2004/09/17 Peter Vanroose - made focal() non-virtual - it just returns a member and should not be overloaded
// \endverbatim

#include <vcsl/vcsl_spatial_transformation.h>
#include <vcsl/vcsl_perspective_sptr.h>

//: Perspective projection transformation
// This is a perspective projection from a 3D cartesian coordinate system to a
// 2D cartesian coordinate system, parametrized by the focal expressed in
// meters. The projection is along the z axis. The center of the projection is
// the origin of the 3D frame. The projection plane is in the half-space z<0.
class vcsl_perspective
  :public vcsl_spatial_transformation
{
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  // Default constructor
  vcsl_perspective() = default;

 public:
  // Destructor
  ~vcsl_perspective() override = default;

  //***************************************************************************
  // Status report
  //***************************************************************************

  //: Is `this' invertible at time `time'? Never !
  //  REQUIRE: valid_time(time)
  // Pure virtual function of vcsl_spatial_transformation
  bool is_invertible(double time) const override;

  //: Is `this' correctly set ?
  // Virtual function of vcsl_spatial_transformation
  bool is_valid() const override
  { return vcsl_spatial_transformation::is_valid() &&
           ((this->duration()==0&&focal_.size()==1) ||
            this->duration()==focal_.size()); }

  //***************************************************************************
  // Transformation parameters
  //***************************************************************************

  //: Set the focal in meters of a static perspective projection
  void set_static(double new_focal);

  //: Set the focal variation along the time in meters
  void set_focal(list_of_scalars const& new_focal) { focal_=new_focal; }

  //: Return the focal variation along the time in meters
  list_of_scalars focal() const { return focal_; }

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
  //  REQUIRE: is_invertible(time) and v.size()==2
  //  The first pre-condition is never true. You can not use this method
  // Pure virtual function of vcsl_spatial_transformation
  vnl_vector<double> inverse(const vnl_vector<double> &v,
                                     double time) const override;

 protected:

  //: Compute the parameter at time `time'
  double focal_value(double time) const;

  //: Angle variation along the time
  list_of_scalars focal_;
};

#endif // vcsl_perspective_h_
