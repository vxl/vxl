// vil_nitf2: Written by Harry Voorhees (hlv@) and Rob Radtke (rob@) of
// Stellar Science Ltd. Co. (stellarscience.com) for 
// Air Force Research Laboratory, 2005.

#ifndef VIL_NITF2_TYPED_FIELD_FORMATTER_H
#define VIL_NITF2_TYPED_FIELD_FORMATTER_H

#include "vil_nitf2_field_formatter.h"

// All subclasses of NITF field formatter derive from this template
// The template handles both scalars and vectors of type T.
//
template<typename T>
class vil_nitf2_typed_field_formatter : public vil_nitf2_field_formatter
{
public:
  // Constructor
  vil_nitf2_typed_field_formatter(vil_nitf2::enum_field_type field_type, int field_width) 
    : vil_nitf2_field_formatter(field_type, field_width) {}
 
  // Destructor
  virtual ~vil_nitf2_typed_field_formatter() {};

  // Returns a vcl_vector field of specified dimensionality.
  vil_nitf2_array_field* create_array_field(
    int num_dimensions, vil_nitf2_field_definition* field_definition);
 
  // Returns a new field, read from stream.
  virtual vil_nitf2_scalar_field* read_field(vil_nitf2_istream& input, bool& out_blank);

  // Writes scalar field to the specified stream.
  virtual bool write_field(vil_nitf2_ostream& output, vil_nitf2_scalar_field* field);
  
  // Attempts to read one instance of field from vil_nitf2_istream into value.
  // Returns whether value is valid. Sets out_blank to indicate whether the
  // input read consisted entirely of blanks (in which case false is the
  // return value). The default implementation converts to a vcl_stringstream 
  // and calls read(vcl_istream&). Subclasses either need to overload
  // this method or read(vil_nitf2_istream&).
  virtual bool read(vil_nitf2_istream& input, T& out_value, bool& out_blank);

  // Same as above method, for caller that does not care about blanked fields.
  virtual bool read(vil_nitf2_istream& input, T& out_value);

  // Same as above, but reads value from a vcl_istream (which, unlike a
  // vil_nitf2_istream, supports formatted I/O).
  virtual bool read(vcl_istream& input, T& out_value, bool& out_blank);

  // Write value to a vil_nitf2_ostream.  Return success. The default 
  // implementation calls write(vcl_ostream&). Subclasses need to either
  // overload this method or define write(vcl_ostream&).  Returns success.
  virtual bool write(vil_nitf2_ostream& output, const T& value);

  // Same as above, but writes value to a vcl_ostream (which, unlike
  // vil_nitf2_ostream, supports formatted I/O).
  virtual bool write(vcl_ostream& output, const T& value);
};

//=============================================================================
// vil_nitf2_typed_field_formatter implementation
//=============================================================================

#include "vil_nitf2_typed_scalar_field.h"
#include "vil_nitf2_typed_array_field.h"
#include "vil_nitf2_index_vector.h"

template<typename T>
vil_nitf2_array_field* vil_nitf2_typed_field_formatter<T>::create_array_field(
  int num_dimensions, vil_nitf2_field_definition* field_definition)
{
  return new vil_nitf2_typed_array_field<T>(num_dimensions, field_definition);
}

template<typename T>
vil_nitf2_scalar_field* vil_nitf2_typed_field_formatter<T>::read_field(
  vil_nitf2_istream& input, bool& out_blank)
{
  T value;
  if (read( input, value, out_blank )) {
    return new vil_nitf2_typed_scalar_field<T>(value, 0);
  }
  return 0;
}

template<typename T>
bool vil_nitf2_typed_field_formatter<T>::write_field(
  vil_nitf2_ostream& output, vil_nitf2_scalar_field* field)
{
  if (field) {
    T val;
    if (field->value(val)) {
      return write(output, val);
    }
  }
  return false;
}

template<typename T>
bool vil_nitf2_typed_field_formatter<T>::read(
  vil_nitf2_istream& input, T& out_value, bool& out_blank)
{
  //hackery for non-binary data (convert to string and create a stringstream from it)
  vcl_string str = vil_nitf2_field_formatter::read_string(input, field_width);
  vcl_stringstream s(str);
  return read( s, out_value, out_blank );
}

template<typename T>
bool vil_nitf2_typed_field_formatter<T>::read(
  vil_nitf2_istream& input, T& out_value)
{ 
  bool b; 
  return read( input, out_value, b ); 
}

template<typename T>
bool vil_nitf2_typed_field_formatter<T>::read(
  vcl_istream& /* input */, T& /* out_value */, bool& /* out_blank */)
{ 
  assert(0); 
  return false;
}

template<typename T>
bool vil_nitf2_typed_field_formatter<T>::write(
  vcl_ostream& /* output */, const T& /* value */)
{ 
  assert( 0 ); 
  return false; 
}

template<typename T>
bool vil_nitf2_typed_field_formatter<T>::write(
  vil_nitf2_ostream& output, const T& value)
{
  vcl_stringstream strstr;
  write(strstr, value);
  vcl_string str = strstr.str();
  output.write(str.c_str(), str.length());
  return output.ok();
}

//=============================================================================
// vil_nitf2_type_field_formatter subclasses
//=============================================================================

// Reads and writes an integer field, with or without sign.
//
class vil_nitf2_integer_formatter : public vil_nitf2_typed_field_formatter<int>
{
public:
  vil_nitf2_integer_formatter(int field_width, bool show_sign = false);
  bool read(vcl_istream& input, int& out_value, bool& out_blank);
  bool write(vcl_ostream& output, const int& value);
  bool show_sign;
};

class vil_nitf2_long_long_formatter : public vil_nitf2_typed_field_formatter<vil_nitf2_long>
{
public:
  vil_nitf2_long_long_formatter(int field_width, bool show_sign = false);
  bool read(vcl_istream& input, vil_nitf2_long& out_value, bool& out_blank);
  bool write(vcl_ostream& output, const vil_nitf2_long& value);
  bool show_sign;
};

// Reads and writes a vcl_fixed point field, with or without sign, and with
// specified precision.
// 
class vil_nitf2_double_formatter : public vil_nitf2_typed_field_formatter<double>
{
public:
  vil_nitf2_double_formatter(int field_width, int precision, bool show_sign);
  bool read(vcl_istream& input, double& out_value, bool& out_blank);
  bool write(vcl_ostream& output, const double& value);
  int precision; 
  bool show_sign;
};

// Reads and writes a character field. (I know this seems like overkill,
// but it's part of an inheritance hierarchy.)
//
class vil_nitf2_char_formatter : public vil_nitf2_typed_field_formatter<char>
{
public:
  vil_nitf2_char_formatter();
  bool read(vcl_istream& input, char& out_value, bool& out_blank);
  bool write(vcl_ostream& output, const char& value);
};

// Reads and writes a binary data field
//
class vil_nitf2_binary_formatter : public vil_nitf2_typed_field_formatter<void*>
{
public:
  vil_nitf2_binary_formatter(int widthBytes);

  // Overloaded read(vil_nitf2_istream&) instead of vil(vcl_stream&) to read 
  // binary data, without converting to to string (zero data would 
  // prematurely null-terminate the stringstream).
  virtual bool read( vil_nitf2_istream& input, void*& out_value, bool& out_blank );

  /// Overload to write(vil_nitf2_ostream) directly, to avoid string conversion.
  bool write(vil_nitf2_ostream& output, void*const& value);
};

// Reads and writes a vcl_string field.
//
class vil_nitf2_string_formatter : public vil_nitf2_typed_field_formatter<vcl_string>
{
public:
  // Character sets
  enum enum_char_set { ECS, ECSA, BCS, BCSA };

  vil_nitf2_string_formatter(int field_width, enum_char_set char_set = ECS);
  virtual ~vil_nitf2_string_formatter() {};
  bool read(vcl_istream& input, vcl_string& out_value, bool& out_blank );
  bool write(vcl_ostream& output, const vcl_string& value);
  virtual bool is_valid(vcl_string value) const;
  // Member variable
  enum_char_set char_set;
};

// Helper class for vil_nitf2_enum_string_formatter. Represents a vcl_string enumeration.
//
class vil_nitf2_enum_values : public vcl_map<vcl_string, vcl_string>
{
public:
  vil_nitf2_enum_values& value(vcl_string token, vcl_string pretty_name = "");
};

// Reads and writes an enumerated vcl_string field.
// To Do: Reimplement this to represent its value using a new class
// NitfEnum_value, that outputs its pretty_name by default.
//
class vil_nitf2_enum_string_formatter : public vil_nitf2_string_formatter
{
public:
  // Constructor
  vil_nitf2_enum_string_formatter(int field_width, const vil_nitf2_enum_values&);
  
  // Is specified value valid?
  bool is_valid_value(vcl_string value) const;
private:
  void validate_value_map();
  vil_nitf2_enum_values value_map;
};

// Reads and write a date/time field.
//
class vil_nitf2_date_time_formatter : public vil_nitf2_typed_field_formatter<vil_nitf2_date_time>
{
public:
  // Constructor
  vil_nitf2_date_time_formatter(int field_width); 

  bool read(vcl_istream& input, vil_nitf2_date_time& out_value, bool& out_blank);
  
  bool write(vcl_ostream& output, const vil_nitf2_date_time& value);
};

// Reads and writes a location field.
//
class vil_nitf2_location_formatter : public vil_nitf2_typed_field_formatter<vil_nitf2_location*>
{
public:
  // Constructor
  vil_nitf2_location_formatter(int field_width);

  bool read(vcl_istream& input, vil_nitf2_location*& out_value, bool& out_blank);

  bool write(vcl_ostream& output, vil_nitf2_location*const& value);

  // Converts overall field width to seconds precision for DMSH formatted 
  // field, ddmmss.sssHdddmmss.sssH
  static int sec_precision(int field_width) { return (field_width-17)/2; }

  // Converts overall field width to degrees precision for degrees formatted 
  // field, +dd.ddd+ddd.ddd
  static int deg_precision(int field_width) { return (field_width-9)/2; }
};

#endif // VIL_NITF2_TYPED_FIELD_FORMATTER_H