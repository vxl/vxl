// This is core/vcsl/vcsl_translation.h
#ifndef vcsl_translation_h_
#define vcsl_translation_h_
//:
// \file
// \brief Translation transformation
// \author François BERTEL
//
// \verbatim
//  Modifications
//   2000/06/28 François BERTEL Creation. Adapted from IUE
//   2001/04/10 Ian Scott (Manchester) Converted perceps header to doxygen
//   2002/01/22 Peter Vanroose - return type of execute() and inverse() changed to non-ptr
//   2002/01/28 Peter Vanroose - vcl_vector member vector_ changed to non-ptr
//   2004/09/17 Peter Vanroose - made vector() non-virtual - it just returns a member and should not be overloaded
// \endverbatim

#include <vcsl/vcsl_spatial_transformation.h>
#include <vcsl/vcsl_translation_sptr.h>
#include <vnl/vnl_vector.h>

//: Translation transformation
class vcsl_translation
  :public vcsl_spatial_transformation
{
 public:
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  // Default constructor
  vcsl_translation() {}

  // Destructor
  virtual ~vcsl_translation() {}

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
           ((this->duration()==0&&vector_.size()==1) ||
            this->duration()==vector_.size()); }

  //***************************************************************************
  // Transformation parameters
  //***************************************************************************

  //: Set the parameters of a static translation
  void set_static(vnl_vector<double> const& new_vector);

  //: Set the direction vector variation along the time
  void set_vector(list_of_vectors const&new_vector){vector_=new_vector;}

  //: Return the angle variation along the time
  list_of_vectors vector() const { return vector_; }

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
  vnl_vector<double> vector_value(double time) const;

  //: Direction vector variation along the time
  list_of_vectors vector_;
};

#endif // vcsl_translation_h_
