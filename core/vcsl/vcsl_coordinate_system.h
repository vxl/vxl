// This is core/vcsl/vcsl_coordinate_system.h
#ifndef vcsl_coordinate_system_h_
#define vcsl_coordinate_system_h_
//:
// \file
// \brief Abstract coordinate system
// \author François BERTEL
//
// \verbatim
//  Modifications
//   2000/06/28 François BERTEL Creation. Adapted from IUE
//   2002/01/22 Peter Vanroose - return type of from_cs_to_standard_units() and from_standard_units_to_cs() changed non-ptr
//   2004/09/10 Peter Vanroose - Added explicit copy constructor (ref_count !)
//   2004/09/17 Peter Vanroose - made dimensionality() non-virtual - it just returns a member and should not be overloaded
// \endverbatim

#include <vbl/vbl_ref_count.h>
#include <vcsl/vcsl_coordinate_system_sptr.h>
#include <vcsl/vcsl_axis_sptr.h>
#include <vcl_vector.h>
#include <vnl/vnl_vector.h>
class vcsl_spatial;

//: Abstract coordinate system
class vcsl_coordinate_system
  : public vbl_ref_count
{
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

 protected:
  // Default constructor
  vcsl_coordinate_system() {}

 public:
  // Copy constructor
  vcsl_coordinate_system(vcsl_coordinate_system const& c)
    : vbl_ref_count(), axes_(c.axes_) {}

  // Destructor
  virtual ~vcsl_coordinate_system() {}

  //***************************************************************************
  // Status report
  //***************************************************************************

  //: Number of axes
  int dimensionality() const { return axes_.size(); }

  //: Is `i' an index on an axis ?
  bool valid_axis(unsigned int i) const { return i < axes_.size(); }

  //: Return the axis `i'
  //  REQUIRE: valid_axis(i)
  vcsl_axis_sptr axis(int i) const;

  //***************************************************************************
  // Because VXL does not necessarily use dynamic_cast<>
  //***************************************************************************
  virtual const vcsl_spatial *cast_to_spatial() const { return 0; }

  //***************************************************************************
  // Conversion
  //***************************************************************************

  //: Convert `v', expressed with cs units, to standard units
  //  REQUIRE: v.size()==dimensionality()
  vnl_vector<double>
  from_cs_to_standard_units(const vnl_vector<double> &v) const;

  //: Convert `v', expressed with standard units, to cs units
  //  REQUIRE: v.size()==dimensionality()
  vnl_vector<double>
  from_standard_units_to_cs(const vnl_vector<double> &v) const;

 protected:
  //***************************************************************************
  // Implementation
  //***************************************************************************

  //: List of axes
  vcl_vector<vcsl_axis_sptr> axes_;
};

#endif // vcsl_coordinate_system_h_
