// vil_nitf2: Written by Harry Voorhees (hlv@) and Rob Radtke (rob@) of
// Stellar Science Ltd. Co. (stellarscience.com) for
// Air Force Research Laboratory, 2005.

#ifndef VIL_NITF2_TYPED_FIELD_FORMATTER_H
#define VIL_NITF2_TYPED_FIELD_FORMATTER_H

#include <iosfwd>
#include <string>
#include <sstream>
#include "vil_nitf2_field_formatter.h"
#include "vil_nitf2_tagged_record.h"

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

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
  ~vil_nitf2_typed_field_formatter() override = default;

  // Returns a std::vector field of specified dimensionality.
  vil_nitf2_array_field* create_array_field(
    int num_dimensions, vil_nitf2_field_definition* field_definition) override;

  // Returns a new field, read from stream.
  vil_nitf2_scalar_field* read_field(vil_nitf2_istream& input, bool& out_blank) override;

  // Writes scalar field to the specified stream.
  bool write_field(vil_nitf2_ostream& output, const vil_nitf2_scalar_field* field) override;

  // Attempts to read one instance of field from vil_nitf2_istream into value.
  // Returns whether value is valid. Sets out_blank to indicate whether the
  // input read consisted entirely of blanks (in which case false is the
  // return value). The default implementation converts to a std::stringstream
  // and calls read(std::istream&). Subclasses either need to overload
  // this method or read(vil_nitf2_istream&).
  virtual bool read(vil_nitf2_istream& input, T& out_value, bool& out_blank);

  // Same as above, but reads value from a std::istream (which, unlike a
  // vil_nitf2_istream, supports formatted I/O).
  virtual bool read_vcl_stream(std::istream& input, T& out_value, bool& out_blank);

  // Write value to a vil_nitf2_ostream.  Return success. The default
  // implementation calls write(std::ostream&). Subclasses need to either
  // overload this method or define write(std::ostream&).  Returns success.
  virtual bool write(vil_nitf2_ostream& output, const T& value);

  // Same as above, but writes value to a std::ostream (which, unlike
  // vil_nitf2_ostream, supports formatted I/O).
  virtual bool write_vcl_stream(std::ostream& output, const T& value);
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
    return new vil_nitf2_typed_scalar_field<T>(value, nullptr);
  }
  return nullptr;
}

template<typename T>
bool vil_nitf2_typed_field_formatter<T>::write_field(
  vil_nitf2_ostream& output, const vil_nitf2_scalar_field* field)
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
  std::string str = vil_nitf2_field_formatter::read_string(input, field_width);
  std::stringstream s(str);
  return read_vcl_stream( s, out_value, out_blank );
}

template<typename T>
bool vil_nitf2_typed_field_formatter<T>::read_vcl_stream(
  std::istream& /* input */, T& /* out_value */, bool& /* out_blank */)
{
  assert(0);
  return false;
}

template<typename T>
bool vil_nitf2_typed_field_formatter<T>::write_vcl_stream(
  std::ostream& /* output */, const T& /* value */)
{
  assert( 0 );
  return false;
}

template<typename T>
bool vil_nitf2_typed_field_formatter<T>::write(
  vil_nitf2_ostream& output, const T& value)
{
  std::stringstream strstr;
  write_vcl_stream(strstr, value);
  std::string str = strstr.str();
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

  vil_nitf2_field_formatter* copy() const override;

  // partially overridden read/write methods
  using vil_nitf2_typed_field_formatter<int>::read;
  using vil_nitf2_typed_field_formatter<int>::write;
  bool read_vcl_stream(std::istream& input, int& out_value, bool& out_blank) override;
  bool write_vcl_stream(std::ostream& output, const int& value) override;

  bool show_sign;
};

class vil_nitf2_long_long_formatter : public vil_nitf2_typed_field_formatter<vil_nitf2_long>
{
 public:
  vil_nitf2_long_long_formatter(int field_width, bool show_sign = false);

  vil_nitf2_field_formatter* copy() const override;

  // partially overridden read/write methods
  using vil_nitf2_typed_field_formatter<vil_nitf2_long>::read;
  using vil_nitf2_typed_field_formatter<vil_nitf2_long>::write;
  bool read_vcl_stream(std::istream& input, vil_nitf2_long& out_value, bool& out_blank) override;
  bool write_vcl_stream(std::ostream& output, const vil_nitf2_long& value) override;

  bool show_sign;
};

// Reads and writes a fixed point field, with or without sign, and with
// specified precision.
//
class vil_nitf2_double_formatter : public vil_nitf2_typed_field_formatter<double>
{
 public:
  vil_nitf2_double_formatter(int field_width, int precision, bool show_sign);

  vil_nitf2_field_formatter* copy() const override;

  // partially overridden read/write methods
  using vil_nitf2_typed_field_formatter<double>::read;
  using vil_nitf2_typed_field_formatter<double>::write;
  bool read_vcl_stream(std::istream& input, double& out_value, bool& out_blank) override;
  bool write_vcl_stream(std::ostream& output, const double& value) override;

  int precision;
  bool show_sign;
};

// Reads and writes a floating point field in exponential format: sign, digit,
// decimal point, mantissa digits, 'E', sign, exponent digits;
// e.g., "+3.1416E+00", which has mantissa width 4 and exponent width 2.
//
class vil_nitf2_exponential_formatter : public vil_nitf2_typed_field_formatter<double>
{
 public:
  vil_nitf2_exponential_formatter(int mantissa_width, int exponent_width);

  vil_nitf2_field_formatter* copy() const override;

  // partially overridden read/write methods
  using vil_nitf2_typed_field_formatter<double>::read;
  using vil_nitf2_typed_field_formatter<double>::write;
  bool read_vcl_stream(std::istream& input, double& out_value, bool& out_blank) override;
  bool write_vcl_stream(std::ostream& output, const double& value) override;

  int mantissa_width;
  int exponent_width;
};

// Reads and writes a character field. (I know this seems like overkill,
// but it's part of an inheritance hierarchy.)
//
class vil_nitf2_char_formatter : public vil_nitf2_typed_field_formatter<char>
{
 public:
  vil_nitf2_char_formatter();

  vil_nitf2_field_formatter* copy() const override;

  // partially overridden read/write methods
  using vil_nitf2_typed_field_formatter<char>::read;
  using vil_nitf2_typed_field_formatter<char>::write;
  bool read_vcl_stream(std::istream& input, char& out_value, bool& out_blank) override;
  bool write_vcl_stream(std::ostream& output, const char& value) override;
};

// Reads and writes a binary data field
//
class vil_nitf2_binary_formatter : public vil_nitf2_typed_field_formatter<void*>
{
 public:
  vil_nitf2_binary_formatter(int width_bytes);

  vil_nitf2_field_formatter* copy() const override;

  // partially overridden read/write methods
  using vil_nitf2_typed_field_formatter<void*>::read_vcl_stream;
  using vil_nitf2_typed_field_formatter<void*>::write_vcl_stream;

  // Overload read() instead of read_vcl_stream() to read binary data without
  // converting to string, because zero data would prematurely null-terminate
  // the stringstream.
  bool read( vil_nitf2_istream& input, void*& out_value, bool& out_blank ) override;

  /// Overload to write() instead of write_vcl_stream() to write binary data
  // (see preceding comment).
  bool write(vil_nitf2_ostream& output, void*const& value) override;
};

// Reads and writes a std::string field.
//
class vil_nitf2_string_formatter : public vil_nitf2_typed_field_formatter<std::string>
{
 public:
  // Character sets
  enum enum_char_set { ECS, ECSA, BCS, BCSA };

  // Constructor
  vil_nitf2_string_formatter(int field_width, enum_char_set char_set = ECS);

  vil_nitf2_field_formatter* copy() const override;

  // Destructor
  ~vil_nitf2_string_formatter() override = default;

  // partially overridden read/write methods
  using vil_nitf2_typed_field_formatter<std::string>::read;
  using vil_nitf2_typed_field_formatter<std::string>::write;
  bool read_vcl_stream(std::istream& input, std::string& out_value, bool& out_blank ) override;
  bool write_vcl_stream(std::ostream& output, const std::string& value) override;

  virtual bool is_valid(std::string value) const;

  // Member variable
  enum_char_set char_set;
};

// Helper class for vil_nitf2_enum_string_formatter. Represents a std::string enumeration.
//
class vil_nitf2_enum_values : public std::map<std::string, std::string>
{
 public:
  vil_nitf2_enum_values& value(std::string token, std::string pretty_name = "");
};

// Reads and writes an enumerated std::string field.
// To Do: Reimplement this to represent its value using a new class
// NitfEnum_value, that outputs its pretty_name by default.
//
class vil_nitf2_enum_string_formatter : public vil_nitf2_string_formatter
{
 public:
  // Constructor
  vil_nitf2_enum_string_formatter(int field_width, vil_nitf2_enum_values );

  vil_nitf2_field_formatter* copy() const override;

  // Is specified value valid?
  bool is_valid_value(const std::string& value) const;

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

  vil_nitf2_field_formatter* copy() const override;

  // partially overridden read/write methods
  using vil_nitf2_typed_field_formatter<vil_nitf2_date_time>::read;
  using vil_nitf2_typed_field_formatter<vil_nitf2_date_time>::write;
  bool read_vcl_stream(std::istream& input, vil_nitf2_date_time& out_value, bool& out_blank) override;
  bool write_vcl_stream(std::ostream& output, const vil_nitf2_date_time& value) override;
};

// Reads and writes a location field.
//
class vil_nitf2_location_formatter : public vil_nitf2_typed_field_formatter<vil_nitf2_location*>
{
 public:
  // Constructor
  vil_nitf2_location_formatter(int field_width);

  vil_nitf2_field_formatter* copy() const override;

  // partially overridden read/write methods
  using vil_nitf2_typed_field_formatter<vil_nitf2_location*>::read;
  using vil_nitf2_typed_field_formatter<vil_nitf2_location*>::write;
  bool read_vcl_stream(std::istream& input, vil_nitf2_location*& out_value, bool& out_blank) override;
  bool write_vcl_stream(std::ostream& output, vil_nitf2_location*const& value) override;

  // Converts overall field width to seconds precision for DMSH formatted
  // field, ddmmss.sssHdddmmss.sssH
  static int sec_precision(int field_width) { return (field_width-17)/2; }

  // Converts overall field width to degrees precision for degrees formatted
  // field, +dd.ddd+ddd.ddd
  static int deg_precision(int field_width) { return (field_width-9)/2; }
};

// Reads and writes a TRE sequence field
//
class vil_nitf2_tagged_record_sequence_formatter :
  public vil_nitf2_typed_field_formatter<vil_nitf2_tagged_record_sequence>
{
 public:
  // Constructor
  vil_nitf2_tagged_record_sequence_formatter();

  vil_nitf2_field_formatter* copy() const override;

  // partially overridden read/write methods
  using vil_nitf2_typed_field_formatter<vil_nitf2_tagged_record_sequence>::read_vcl_stream;
  using vil_nitf2_typed_field_formatter<vil_nitf2_tagged_record_sequence>::write_vcl_stream;

  // Overload read() instead of read_vcl_stream() to read binary data without
  // converting to string, because zero data would prematurely null-terminate
  // the stringstream.
  bool read( vil_nitf2_istream& input, vil_nitf2_tagged_record_sequence& out_value, bool& out_blank ) override;

  /// Overload to write() instead of write_vcl_stream() to write binary data
  // (see preceding comment).
  bool write(vil_nitf2_ostream& output, const vil_nitf2_tagged_record_sequence& value) override;
};

#endif // VIL_NITF2_TYPED_FIELD_FORMATTER_H
