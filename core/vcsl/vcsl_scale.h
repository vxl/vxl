#ifndef vcsl_scale_h
#define vcsl_scale_h

//:
// \file
// \author François BERTEL
// Modifications
// \verbatim
// 2000/07/19 François BERTEL Creation.
// 10/4/2001 Ian Scott (Manchester) Coverted perceps header to doxygen
// \endverbatim


#include <vcsl/vcsl_scale_sptr.h>

#include <vcsl/vcsl_spatial_transformation.h>
//: Scale transformation
class vcsl_scale
  : public vcsl_spatial_transformation
{
public:
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  //: Default constructor
  explicit vcsl_scale(void);

  //: Destructor
  virtual ~vcsl_scale();

  //***************************************************************************
  // Status report
  //***************************************************************************

  //: Is `this' invertible at time `time'?
  //  REQUIRE: valid_time(time)
  virtual bool is_invertible(const double time) const;

  //: Is `this' correctly set ?
  virtual bool is_valid(void) const;

  //***************************************************************************
  // Transformation parameters
  //***************************************************************************

  //: Set the scale value of a static scale
  virtual void set_static(const double new_scale);

  //: Set the scale variation along the time
  virtual void set_scale(vcl_vector<double> &new_scale);

  //: Return the scale variation along the time
  virtual vcl_vector<double> *scale(void) const;

  //***************************************************************************
  // Basic operations
  //***************************************************************************

  //: Image of `v' by `this'
  //  REQUIRE: is_valid()
  virtual vnl_vector<double> *execute(const vnl_vector<double> &v,
                                      const double time) const;

  //: Image of `v' by the inverse of `this'
  //  REQUIRE: is_valid()
  //  REQUIRE: is_invertible(time)
  virtual vnl_vector<double> *inverse(const vnl_vector<double> &v,
                                      const double time) const;

protected:

  //: Compute the value of the parameter at time `time'
  virtual double scale_value(const double time) const;

  //: Scale variation along the time
  vcl_vector<double> *scale_;
};

#endif // vcsl_scale_h
