#ifndef VCSL_TRANSLATION_H
#define VCSL_TRANSLATION_H

//:
// \file 
// \author François BERTEL
// Modifications
// \verbatim
// 2000/06/28 François BERTEL Creation. Adapted from IUE
// 10/4/2001 Ian Scott (Manchester) Coverted perceps header to doxygen
// \endverbatim


#include <vcsl/vcsl_translation_sptr.h>

#include <vcsl/vcsl_spatial_transformation.h>

#include <vcl_vector.h>
#include <vnl/vnl_vector.h>
//: Translation transformation
class vcsl_translation
  :public vcsl_spatial_transformation
{
public:
  //: Direction vector variation along the time
  //  vcl_vector<vnl_vector<double> *> *_vector;

  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  //: Default constructor
  explicit vcsl_translation(void);

  //: Destructor
  virtual ~vcsl_translation();

  //***************************************************************************
  // Status report
  //***************************************************************************

  //: Is `this' invertible at time `time'?
  //: REQUIRE: valid_time(time)
  virtual bool is_invertible(const double time) const;

  //: Is `this' correctly set ?
  virtual bool is_valid(void) const;

  //***************************************************************************
  // Transformation parameters
  //***************************************************************************

  //: Set the parameters of a static translation
  virtual void set_static(vnl_vector<double> &new_vector);
  
  //: Set the direction vector variation along the time
  virtual void set_vector(list_of_vectors &new_vector);
  
  //: Return the angle variation along the time
  virtual list_of_vectors *vector(void) const;
  
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
  //: Compute the value of the parameter at time `time'
  virtual vnl_vector<double> *vector_value(const double time) const;

  //: Direction vector variation along the time
  list_of_vectors *vector_;
};

#endif // VCSL_TRANSLATION_H
