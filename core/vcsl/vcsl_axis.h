#ifndef vcsl_axis_h_
#define vcsl_axis_h_
//:
// \file
// \brief Axis descriptor: a dimension, a unit, a label
// \author François BERTEL
//
// \verbatim
//  Modifications
//   2000/06/28 François BERTEL Creation. Adapted from IUE
//   2004/09/17 Peter Vanroose  do not pass vcsl_unit objects; use vcsl_unit_sptr instead
// \endverbatim

#include <vbl/vbl_ref_count.h>
#include <vcsl/vcsl_axis_sptr.h>
#include <vcsl/vcsl_unit_sptr.h>
#include <vcsl/vcsl_dimension_sptr.h>
#include <vcl_string.h>

//: Axis descriptor: a dimension, a unit, a label
class vcsl_axis
  : public vbl_ref_count
{
 public:
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  //: Default constructor. Axis with length in meters and an empty label
  vcsl_axis();

  //: Constructor from dimension. Unit is the standard one. Label is empty
  explicit vcsl_axis(vcsl_dimension_sptr const& new_dimension);

  //: Constructor from dimension and unit. Label is empty
  //  REQUIRE: new_dimension.compatible_unit(new_unit)
  vcsl_axis(vcsl_dimension_sptr const& new_dimension,
            vcsl_unit_sptr const& new_unit);

  //: Constructor from dimension, unit and label
  vcsl_axis(vcsl_dimension_sptr const& new_dimension,
            vcsl_unit_sptr const& new_unit,
            vcl_string const& new_label)
    : dimension_(new_dimension), unit_(new_unit), label_(new_label) {}

  // Copy constructor
  vcsl_axis(const vcsl_axis &a)
    : vbl_ref_count(),dimension_(a.dimension_),unit_(a.unit_),label_(a.label_){}

  // Destructor
  ~vcsl_axis() {}

  //***************************************************************************
  // Status report
  //***************************************************************************

  //: Return the dimension
  vcsl_dimension_sptr dimension() const { return dimension_; }

  //: Return the unit of the dimension
  vcsl_unit_sptr unit() const { return unit_; }

  //: Return the label of the axis
  vcl_string label() const { return label_; }

  //***************************************************************************
  // Status change
  //***************************************************************************

  //: Set the dimension. The unit is set with the standard unit
  void set_dimension(vcsl_dimension_sptr const& new_dimension);

  //: Set the dimension and the unit
  //  REQUIRE: new_dimension.compatible_unit(new_unit)
  void set_dimension_and_unit(vcsl_dimension_sptr const& new_dimension,
                              vcsl_unit_sptr const& new_unit);

  //: Set the unit of the dimension
  //  REQUIRE dimension()->compatible_unit(new_unit)
  void set_unit(vcsl_unit_sptr const& new_unit);

  //: Set the label
  void set_label(vcl_string const& new_label) { label_=new_label; }

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

#endif // vcsl_axis_h_
