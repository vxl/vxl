#ifndef vcsl_cartesian_2d_h_
#define vcsl_cartesian_2d_h_
//:
// \file
// \brief 2D Cartesian coordinate system
// \author Francois BERTEL
//
// \verbatim
//  Modifications
//   2000/06/28 Francois BERTEL Creation. Adapted from IUE
//   2004/09/10 Peter Vanroose  Inlined all 1-line methods in class decl
// \endverbatim

#include "vcsl_spatial.h"
#include "vcsl_cartesian_2d_sptr.h"

//: 2D Cartesian coordinate system
class vcsl_cartesian_2d
  : public vcsl_spatial
{
 public:
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  // Default constructor
  vcsl_cartesian_2d();

  // Destructor
  ~vcsl_cartesian_2d() override = default;

  //***************************************************************************
  // Because VXL does not necessarily use dynamic_cast<>
  //***************************************************************************

  const vcsl_cartesian_2d *cast_to_cartesian_2d() const override { return this; }

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

#endif // vcsl_cartesian_2d_h_
