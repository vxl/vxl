#ifndef VCSL_AXIS_H
#define VCSL_AXIS_H
//:
// \file 
// \brief Descibes vcsl_axis - Axis descriptor: a dimension, a unit, a label
// \author François BERTEL
// Modifications
// \verbatim
// 2000/06/28 François BERTEL Creation. Adapted from IUE
// \endverbatim

#include <vcl_string.h>
#include <vcsl/vcsl_axis_sptr.h>

//*****************************************************************************
// External declarations
//*****************************************************************************
#include <vbl/vbl_ref_count.h>
#include <vcsl/vcsl_dimension_sptr.h>
#include <vcsl/vcsl_unit_sptr.h>

//: Axis descriptor: a dimension, a unit, a label
class vcsl_axis
  : public vbl_ref_count
{
public:
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  //: Default constructor. Axis with length in meters and an empty label
  explicit vcsl_axis(void);

  //: Constructor from dimension. Unit is the standard one. Label is empty
  explicit vcsl_axis(vcsl_dimension &new_dimension);

  //: Constructor from dimension and unit. Label is empty
  //: REQUIRE: new_dimension.compatible_unit(new_unit)
  explicit vcsl_axis(vcsl_dimension &new_dimension,
                     vcsl_unit &new_unit);

  //: Constructor from dimension, unit and label
  explicit vcsl_axis(vcsl_dimension &new_dimension,
                     vcsl_unit &new_unit,
                     vcl_string &new_label);
  
  //: Copy constructor
  vcsl_axis(const vcsl_axis &other);

  //: Destructor
  virtual ~vcsl_axis();

  //***************************************************************************
  // Status report
  //***************************************************************************

  //: Return the dimension
  virtual const vcsl_dimension_sptr dimension(void) const;

  //: Return the unit of the dimension
  virtual const vcsl_unit_sptr unit(void) const;

  //: Return the label of the axis
  virtual const vcl_string label(void) const;

  //***************************************************************************
  // Status change
  //***************************************************************************

  //: Set the dimension. The unit is set with the standard unit
  virtual void set_dimension(vcsl_dimension &new_dimension);

  //: Set the dimension and the unit
  //: REQUIRE: new_dimension.compatible_unit(new_unit)
  virtual void set_dimension_and_unit(vcsl_dimension &new_dimension,
                                      vcsl_unit &new_unit);

  //: Set the unit of the dimension
  //: REQUIRE dimension()->compatible_unit(new_unit)
  virtual void set_unit(vcsl_unit &new_unit);

  //: Set the label
  virtual void set_label(const vcl_string &new_label);

protected:
  //***************************************************************************
  // Implementation
  //***************************************************************************

  //: Dimension
  vcsl_dimension_sptr dimension_;

  //: Unit of the dimension
  vcsl_unit_sptr unit_;

  //: Label of the axis
  vcl_string label_;
};

#endif // #ifndef VCSL_AXIS_H
