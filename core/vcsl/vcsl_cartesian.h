#ifndef vcsl_cartesian_h_
#define vcsl_cartesian_h_
//:
// \file
// \brief Cartesian coordinate system
// \author François BERTEL
//
// \verbatim
//  Modifications
//   2000/06/28 François BERTEL Creation. Adapted from IUE
//   2004/09/10 Peter Vanroose  Inlined all 1-line methods in class decl
// \endverbatim

#include <vcsl/vcsl_coordinate_system.h>
#include <vcsl/vcsl_cartesian_sptr.h>

//: Cartesian coordinate system
// The axes of a cartesian coordinate system are orthogonal and normally right
// handed. This is not necessarily a spatial coordinate system.
class vcsl_cartesian
  : public vcsl_coordinate_system
{
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  //: Default constructor. Sets itself right handed
  vcsl_cartesian() : right_handed_(true) {}

 public:
  // Destructor
  virtual ~vcsl_cartesian() {}

  //***************************************************************************
  // Status report
  //***************************************************************************

  //: Are the axes of `this' right handed ?
  bool is_right_handed() const { return right_handed_; }

  //***************************************************************************
  // Status setting
  //***************************************************************************

  //: Set whether the coordinate system is right handed or not
  void set_right_handed(bool val) { right_handed_ = val; }

 protected:
  //***************************************************************************
  // Implementation
  //***************************************************************************

  //:  True if the axes of `this' are right handed
  bool right_handed_;
};

#endif // vcsl_cartesian_h_
