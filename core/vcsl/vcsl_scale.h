// This is core/vcsl/vcsl_scale.h
#ifndef vcsl_scale_h_
#define vcsl_scale_h_
//:
// \file
// \brief Scale transformation
// \author François BERTEL
//
// \verbatim
//  Modifications
//   2000/07/19 François BERTEL Creation.
//   2001/04/10 Ian Scott (Manchester) Converted perceps header to doxygen
//   2002/01/22 Peter Vanroose - return type of execute() and inverse() changed to non-ptr
//   2002/01/28 Peter Vanroose - vcl_vector member scale_ changed to non-ptr
//   2004/09/17 Peter Vanroose - made scale() non-virtual - it just returns a member and should not be overloaded
// \endverbatim

#include <vcsl/vcsl_spatial_transformation.h>
#include <vcsl/vcsl_scale_sptr.h>

//: Scale transformation
class vcsl_scale
  : public vcsl_spatial_transformation
{
 public:
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  // Default constructor
  vcsl_scale() {}

  // Destructor
  virtual ~vcsl_scale() {}

  //***************************************************************************
  // Status report
  //***************************************************************************

  //: Is `this' invertible at time `time'?
  //  REQUIRE: valid_time(time)
  // Pure virtual function of vcsl_spatial_transformation
  virtual bool is_invertible(double time) const;

  //: Is `this' correctly set ?
  // Virtual function of vcsl_spatial_transformation
  virtual bool is_valid() const
  { return vcsl_spatial_transformation::is_valid() &&
           ((this->duration()==0&&scale_.size()==1) ||
             this->duration()==scale_.size()); }

  //***************************************************************************
  // Transformation parameters
  //***************************************************************************

  //: Set the scale value of a static scale
  void set_static(double new_scale);

  //: Set the scale variation along the time
  void set_scale(vcl_vector<double> const& new_scale) { scale_=new_scale; }

  //: Return the scale variation along the time
  vcl_vector<double> scale() const { return scale_; }

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

  //: Compute the value of the parameter at time `time'
  double scale_value(double time) const;

  //: Scale variation along the time
  vcl_vector<double> scale_;
};

#endif // vcsl_scale_h_
