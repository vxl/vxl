// This is core/vcsl/vcsl_composition.h
#ifndef vcsl_composition_h_
#define vcsl_composition_h_
//:
// \file
// \brief Composition of transformations
// \author Francois BERTEL
//
// \verbatim
//  Modifications
//   2000/06/28 Francois BERTEL Creation. Adapted from IUE
//   2002/01/22 Peter Vanroose - return type of execute() and inverse() changed to non-ptr
//   2002/01/28 Peter Vanroose - std::vector member transformations_ changed to non-ptr
//   2004/09/17 Peter Vanroose - made composition() non-virtual - it just returns a member and should not be overloaded
// \endverbatim

#include <vector>
#include <vcsl/vcsl_spatial_transformation.h>
#include <vcsl/vcsl_composition_sptr.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Composition of transformations
// This transformation handles a composition of transformations, that is,
// at a given time, all the transformations are applied on a given point
class vcsl_composition
  :public vcsl_spatial_transformation
{
 public:
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  // Default constructor
  vcsl_composition() = default;

  // Destructor
  ~vcsl_composition() override = default;

  //***************************************************************************
  // Status report
  //***************************************************************************

  //: Is `this' invertible at time `time'?
  //  REQUIRE: valid_time(time)
  // Pure virtual function of vcsl_spatial_transformation
  bool is_invertible(double time) const override;

  //: Is `this' correctly set ?
  // Virtual function of vcsl_spatial_transformation
  bool is_valid() const override;

  //: Return the list of transformations
  std::vector<vcsl_spatial_transformation_sptr> composition() const { return transformations_; }

  //***************************************************************************
  // Status setting
  //***************************************************************************

  //: Set the list of transformations of the composition
  //  The transformations are performed in the order of the list
  void set_composition(std::vector<vcsl_spatial_transformation_sptr> const& t) { transformations_=t; }

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
  std::vector<vcsl_spatial_transformation_sptr> transformations_;
};

#endif // vcsl_composition_h_
