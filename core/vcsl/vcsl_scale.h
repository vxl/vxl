// This is core/vcsl/vcsl_scale.h
#ifndef vcsl_scale_h_
#define vcsl_scale_h_
//:
// \file
// \brief Scale transformation
// \author Francois BERTEL
//
// \verbatim
//  Modifications
//   2000/07/19 Francois BERTEL Creation.
//   2001/04/10 Ian Scott (Manchester) Converted perceps header to doxygen
//   2002/01/22 Peter Vanroose - return type of execute() and inverse() changed to non-ptr
//   2002/01/28 Peter Vanroose - std::vector member scale_ changed to non-ptr
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
  vcsl_scale() = default;

  // Destructor
  ~vcsl_scale() override = default;

  //***************************************************************************
  // Status report
  //***************************************************************************

  //: Is `this' invertible at time `time'?
  //  REQUIRE: valid_time(time)
  // Pure virtual function of vcsl_spatial_transformation
  bool is_invertible(double time) const override;

  //: Is `this' correctly set ?
  // Virtual function of vcsl_spatial_transformation
  bool is_valid() const override
  { return vcsl_spatial_transformation::is_valid() &&
           ((this->duration()==0&&scale_.size()==1) ||
             this->duration()==scale_.size()); }

  //***************************************************************************
  // Transformation parameters
  //***************************************************************************

  //: Set the scale value of a static scale
  void set_static(double new_scale);

  //: Set the scale variation along the time
  void set_scale(std::vector<double> const& new_scale) { scale_=new_scale; }

  //: Return the scale variation along the time
  std::vector<double> scale() const { return scale_; }

  //***************************************************************************
  // Basic operations
  //***************************************************************************

  //: Image of `v' by `this'
  //  REQUIRE: is_valid()
  // Pure virtual function of vcsl_spatial_transformation
  vnl_vector<double> execute(const vnl_vector<double> &v,
                                     double time) const override;

  //: Image of `v' by the inverse of `this'
  //  REQUIRE: is_valid()
  //  REQUIRE: is_invertible(time)
  // Pure virtual function of vcsl_spatial_transformation
  vnl_vector<double> inverse(const vnl_vector<double> &v,
                                     double time) const override;

 protected:

  //: Compute the value of the parameter at time `time'
  double scale_value(double time) const;

  //: Scale variation along the time
  std::vector<double> scale_;
};

#endif // vcsl_scale_h_
