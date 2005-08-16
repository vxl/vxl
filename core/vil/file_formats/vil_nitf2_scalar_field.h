// vil_nitf2: Written by Harry Voorhees (hlv@) and Rob Radtke (rob@) of
// Stellar Science Ltd. Co. (stellarscience.com) for 
// Air Force Research Laboratory, 2005.

#ifndef VIL_NITF2_SCALAR_FIELD_H
#define VIL_NITF2_SCALAR_FIELD_H

#include "vil_nitf2_field.h"

class vil_nitf2_field_definition;

// An abstract class that represents a scalar field, i.e., one that
// is not within a repeat loop. Subclasses store the type-specific
// value. 

class vil_nitf2_scalar_field : public vil_nitf2_field
{
public:
  vil_nitf2_scalar_field(vil_nitf2_field_definition* definition) 
    : vil_nitf2_field(definition) {}

  virtual ~vil_nitf2_scalar_field() {}

  virtual int num_dimensions() const { return 0; }

  // Sets output argument to the value of field. Subclasses overload the 
  // appropriate method to set out parameter and return true. These methods
  // are defined here for the convenience of my callers, so they don't
  // have to downcast to the specific field type.
  virtual bool value(long long&) const { return false; }
  virtual bool value(int&) const { return false; }
  virtual bool value(double&) const { return false; }
  virtual bool value(char&) const { return false; }
  virtual bool value(void*&) const { return false; }
  virtual bool value(vcl_string&) const { return false; }
  virtual bool value(vil_nitf2_location*&) const { return false; }
  virtual bool value(vil_nitf2_date_time&) const { return false; }

  // Attempts to read a scalar field from input stream, using specified 
  // definition. Returns field if successfully created.  May set arg
  // error to true even if field is returned; for example, if it's a 
  // required (non-blank) field that is all blank. If variable_width
  // is non-negative, it overrides the formatter's field width.
  static vil_nitf2_scalar_field* read(vil_nitf2_istream& input,
                                      vil_nitf2_field_definition* definition, 
                                       int variable_width = -1,
                                       bool* error = 0);

  // Write to NITF stream. Arg variable_width, if non-negative, overrides
  // formatter's field_width.
  bool write(vil_nitf2_ostream& output, int variable_width = -1);
};

#endif // VIL_NITF2_SCALAR_FIELD_H