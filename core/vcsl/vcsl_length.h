#ifndef vcsl_length_h_
#define vcsl_length_h_
//:
// \file
// \brief Length dimension
// \author François BERTEL
//
// \verbatim
//  Modifications
//   2000/06/28 François BERTEL Creation. Adapted from IUE
//   2001/04/10 Ian Scott (Manchester) Converted perceps header to doxygen
//   2004/09/17 Peter Vanroose  made "instance_" a local static variable of method instance()
//   2004/09/17 Peter Vanroose  do not pass vcsl_unit objects; use vcsl_unit_sptr instead
// \endverbatim

#include <vcsl/vcsl_dimension.h>
#include <vcsl/vcsl_length_sptr.h>
#include <vcsl/vcsl_unit_sptr.h>

//: Length dimension
class vcsl_length
  : public vcsl_dimension
{
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  // Default constructor
  vcsl_length() {}

 public:
  // Destructor
  virtual ~vcsl_length() {}

  //***************************************************************************
  // Status report
  //***************************************************************************

  //: Is `new_unit' a compatible unit for the dimension ?
  // Pure virtual function of vcsl_dimension
  virtual bool compatible_unit(vcsl_unit_sptr const& new_unit) const;

  //: Return the standard unit associated to the dimension
  // Pure virtual function of vcsl_dimension
  virtual vcsl_unit_sptr standard_unit() const;

  //***************************************************************************
  // Singleton pattern
  //***************************************************************************

  //: Return the reference to the unique vcsl_length object
  static vcsl_length_sptr instance();
};

#endif // vcsl_length_h_
