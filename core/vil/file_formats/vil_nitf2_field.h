// vil_nitf2: Written by Harry Voorhees (hlv@) and Rob Radtke (rob@) of
// Stellar Science Ltd. Co. (stellarscience.com) for 
// Air Force Research Laboratory, 2005.

#ifndef VIL_NITF2_FIELD_H
#define VIL_NITF2_FIELD_H

#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vcl_string.h>

class vil_nitf2_date_time;
class vil_nitf2_location;
class vil_nitf2_field_definition;
class vil_nitf2_field_formatter;
class vil_nitf2_index_vector;
class vil_nitf2_scalar_field;
class vil_nitf2_array_field;

#include "vil_nitf2.h"

//-----------------------------------------------------------------------------
// vil_nitf2_field is an instance of a single NITF field (or sequence of repeating
// fields), including its definition and value, as read from (or to be written 
// to) a NITF file. 

// Abstract base class for scalar and array-valued fields, as read from
// or to be written to a NITF file. This class includes its definition
// information, value information is stored in subclasses.
//
class vil_nitf2_field
{
public:
  // Return my identifier
  vcl_string tag() const;

  // Return my descriptive name
  vcl_string pretty_name() const;

  // Return my description
  vcl_string description() const;

  // Return number of dimensions: 0 for scalar fields, positive for array fields.
  virtual int num_dimensions() const = 0;

  // Downcast methods
  vil_nitf2_scalar_field* scalar_field();
  vil_nitf2_array_field* array_field();

  // Destructor
  virtual ~vil_nitf2_field() { };

  // Output to stream (required overload as a reminder to implement operator <<)
  virtual vcl_ostream& output(vcl_ostream& os) const = 0;

  // Return my element data type
  vil_nitf2::enum_field_type type() const; 

protected:
  // Default constructor
  vil_nitf2_field(vil_nitf2_field_definition* definition) : m_definition(definition) {}

  // Members
  vil_nitf2_field_definition* m_definition;

};

// Output operator
vcl_ostream& operator << (vcl_ostream& os, const vil_nitf2_field& field);

#endif // VIL_NITF2_FIELD_H