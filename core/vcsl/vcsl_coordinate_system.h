#ifndef VCSL_COORDINATE_SYSTEM_H
#define VCSL_COORDINATE_SYSTEM_H
//*****************************************************************************
//
// .NAME vcsl_coordinate_system - Abstract coordinate system
// .LIBRARY vcsl
// .HEADER  vcsl/vcsl_coordinate_system.h
// .INCLUDE vcsl/vcsl_coordinate_system_ref.h
// .INCLUDE vbl/vbl_ref_count.h
// .INCLUDE vcsl/vcsl_axis_ref.h
// .INCLUDE vcl/vcl_list.h
// .FILE    vcsl/vcsl_coordinate_system.cxx
//
// .SECTION Description
// A coordinate system is just a list of axes. Each concrete coordinate system
// has to inherit from this class
//
// .SECTION Author
// François BERTEL
//
// .SECTION Modifications
// 2000/06/28 François BERTEL Creation. Adapted from IUE
//*****************************************************************************

#include <vcsl/vcsl_coordinate_system_ref.h>

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vbl/vbl_ref_count.h>

#include <vcsl/vcsl_axis_ref.h>
#include <vcl/vcl_vector.h>
#include <vnl/vnl_vector.h>

// Because VXL does not use dynamic_cast<> :-(
class vcsl_spatial;

class vcsl_coordinate_system
  : public vbl_ref_count
{
public:
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  //: Default constructor
  explicit vcsl_coordinate_system(void);

  //: Destructor
  virtual ~vcsl_coordinate_system();

  //***************************************************************************
  // Status report
  //***************************************************************************

  //: Number of axes
  virtual int dimensionnality(void) const;


  //: Is `i' an index on an axis ?
  virtual bool valid_axis(const int i) const;

  //: Return the axis `i'
  //: REQUIRE: valid_axis(i)
  virtual vcsl_axis_ref axis(const int i) const;

  //***************************************************************************
  // Because VXL does not use dynamic_cast<> :-(
  //***************************************************************************

  virtual const vcsl_spatial *cast_to_spatial(void) const;

  //***************************************************************************
  // Conversion
  //***************************************************************************

  //: Convert `v', exprimed with cs units, to standard units
  //: REQUIRE: v.size()==dimensionnality()
  vnl_vector<double> *
  from_cs_to_standard_units(const vnl_vector<double> &v) const;

  //: Convert `v', exprimed with standard units, to cs units
  //: REQUIRE: v.size()==dimensionnality()
  vnl_vector<double> *
  from_standard_units_to_cs(const vnl_vector<double> &v) const;

protected:
  //***************************************************************************
  // Implementation
  //***************************************************************************

  //: List of axes
  vcl_vector<vcsl_axis_ref> _axes;
};

#endif // #ifndef VCSL_COORDINATE_SYSTEM_H
