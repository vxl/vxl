// vil_nitf2: Written by Harry Voorhees (hlv@) and Rob Radtke (rob@) of
// Stellar Science Ltd. Co. (stellarscience.com) for 
// Air Force Research Laboratory, 2005.

#ifndef VIL_NITF2_FIELD_FORMATTER_H
#define VIL_NITF2_FIELD_FORMATTER_H

//#include "vil_nitf2_field.h"
class vil_nitf2_field;

#include <vcl_map.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vcl_sstream.h>
#include <vcl_istream.h>
#include <vcl_ostream.h>
#include <vcl_cassert.h>

#include "vil_nitf2.h"

class vil_nitf2_field;
class vil_nitf2_field_definition;
class vil_nitf2_date_time;
class vil_nitf2_location;
class vil_nitf2_scalar_field;
class vil_nitf2_array_field;

//-----------------------------------------------------------------------------
// A subclass of NITF field formatter exists for each NITF field data type. 
// These are used to define record extensions (as opposed to representing 
// values); they are used to read and write individual fields of the NITF 
// stream. 

// All formatters definitions include a field width, typically a vcl_fixed width,
// unless a length functor is specified which is evaluated when the file is 
// read.
//
// Some fields include "precision" (the length of mantissa). For some numeric
// fields, NITF allows blanks in lieu of insignificant decimal values.
// For this reason, the precision stored in a field may (once implemented)
// be less than the Formatter's precision.

// Base class for NITF field formatters
//
class vil_nitf2_field_formatter
{ 
public:
  // Field types currently supported

  vil_nitf2::enum_field_type field_type;
  int field_width;

  vil_nitf2_field_formatter(vil_nitf2::enum_field_type field_type, int field_width) 
    : field_type(field_type), field_width(field_width) {}

  // Destructor
  virtual ~vil_nitf2_field_formatter() {}

  // Attempts to read scalar field from stream. Returns a new instance of field if 
  // success and 0 otherwise. Sets out_blank to whether input was entirely blank 
  // (in which case 0 is returned).
  virtual vil_nitf2_scalar_field* read_field(
    vil_nitf2_istream& input, bool& out_blank) = 0;

  // Initializes a vector field of specified dimensionality
  virtual vil_nitf2_array_field* create_array_field(
    int num_dimensions, vil_nitf2_field_definition*) = 0;

  virtual bool write_field(vil_nitf2_ostream& output, vil_nitf2_scalar_field* field) = 0;

  // Writes a blank instance of field value to output stream. Returns 
  // true iff successfully written. No need to overload this method.
  bool write_blank(vcl_ostream& output);

  // Same as above, but writes to a vil_stream.
  bool write_blank(vil_stream& output);

  // Helper function which reads the specified number characters into 
  // a string (if possible), which it returns as a null-terminated C string,
  // which the caller owns. The length of the C string reflects the number 
  // of characters read.
  static const char* read_char_array(vcl_istream& input, int length);
  static bool read_c_str(vcl_istream& input, int length, 
                       const char*& out_cstr, bool& all_blank);
  // Same as above, but returns a vcl_string.
  static vcl_string read_string(vcl_istream& input, int length);
  // Same as above, but takes a vil_stream as input
  static vcl_string read_string(vil_stream& input, int length);

  // Helper function to test presence of number sign. Returns true
  // iff first character is consistent with flag show_sign.
  static bool check_sign(const char* cstr, bool show_sign);

  // Helper function to test whether the null-terminated C vcl_string contains 
  // all blanks
  static bool is_all_blank(const char* cstr);
};

#endif // VIL_NITF2_FIELD_FORMATTER_H
