#ifndef vcsl_dimension_h_
#define vcsl_dimension_h_
//:
// \file
// \brief Abstract dimension
// \author François BERTEL
//
// \verbatim
//  Modifications
//   2000/06/28 François BERTEL Creation. Adapted from IUE
//   2004/09/10 Peter Vanroose  Inlined all 1-line methods in class decl
//   2004/09/17 Peter Vanroose  do not pass vcsl_unit objects; use vcsl_unit_sptr instead
// \endverbatim

#include <vbl/vbl_ref_count.h>
#include <vcsl/vcsl_dimension_sptr.h>
#include <vcsl/vcsl_unit_sptr.h>

//: Abstract dimension
// A dimension is a type of quantity to be measured
class vcsl_dimension
  : public vbl_ref_count
{
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

 protected:
  // Default constructor
  vcsl_dimension() {}

 public:
  // Copy constructor
  vcsl_dimension(vcsl_dimension const&) : vbl_ref_count() {}

  // Destructor
  virtual ~vcsl_dimension() {}

  //***************************************************************************
  // Status report
  //***************************************************************************

  //: Is `new_unit' a compatible unit for the dimension ?
  virtual bool compatible_unit(vcsl_unit_sptr const& new_unit) const=0;

  //: Return the standard unit associated to the dimension
  virtual vcsl_unit_sptr standard_unit() const=0;
};

#endif // vcsl_dimension_h_
