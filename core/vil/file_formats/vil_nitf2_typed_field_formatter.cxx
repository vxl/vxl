// vil_nitf2: Written by Harry Voorhees (hlv@) and Rob Radtke (rob@) of
// Stellar Science Ltd. Co. (stellarscience.com) for
// Air Force Research Laboratory, 2005.

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <utility>
#include <cerrno>
#include "vil_nitf2_typed_field_formatter.h"


//==============================================================================
// Class vil_nitf2_date_time_formatter

vil_nitf2_date_time_formatter::vil_nitf2_date_time_formatter(int field_width)
  : vil_nitf2_typed_field_formatter<vil_nitf2_date_time>(vil_nitf2::type_date_time, field_width)
{}

vil_nitf2_field_formatter* vil_nitf2_date_time_formatter::copy() const
{
  return new vil_nitf2_date_time_formatter(field_width);
}

bool vil_nitf2_date_time_formatter::read_vcl_stream(std::istream& input, vil_nitf2_date_time& out_value, bool& out_blank)
{
  return out_value.read(input, field_width, out_blank);
}

bool vil_nitf2_date_time_formatter::write_vcl_stream(std::ostream& output, const vil_nitf2_date_time& value)
{
  return value.write(output, field_width);
}

//==============================================================================
// Class vil_nitf2_location_formatter

vil_nitf2_location_formatter::vil_nitf2_location_formatter(int field_width)
  : vil_nitf2_typed_field_formatter<vil_nitf2_location*>(vil_nitf2::type_location, field_width)
{}

vil_nitf2_field_formatter* vil_nitf2_location_formatter::copy() const
{
  return new vil_nitf2_location_formatter(field_width);
}

bool vil_nitf2_location_formatter::read_vcl_stream(std::istream& input,
                                                   vil_nitf2_location*& out_value,
                                                   bool& out_blank)
{
  std::streampos tag_start_pos = input.tellg();
  vil_nitf2_location* location = new vil_nitf2_location_degrees(deg_precision(field_width));
  if (location->read(input, field_width, out_blank)) {
    out_value = location;
    return true;
  } else {
    delete location;
    input.seekg(tag_start_pos);
    location = new vil_nitf2_location_dmsh(sec_precision(field_width));
    if (location->read(input, field_width, out_blank)) {
      out_value = location;
      return true;
    } else {
      delete location;
      out_value = nullptr;
      return false;
    }
  }
}

bool vil_nitf2_location_formatter::write_vcl_stream(std::ostream& output, vil_nitf2_location*const& value)
{
  return value->write(output, field_width);
}

//==============================================================================
// Class vil_nitf2_integer_formatter

vil_nitf2_integer_formatter::vil_nitf2_integer_formatter(int field_width, bool show_sign )
  : vil_nitf2_typed_field_formatter<int>(vil_nitf2::type_int, field_width),
      show_sign(show_sign)
{
  //Assuming that int is 32 bits, then it can't represent a value higher than
  // (10^32)/2 ... that is 2147483648.
  //assert( field_width < 10 );
}

vil_nitf2_field_formatter* vil_nitf2_integer_formatter::copy() const
{
  return new vil_nitf2_integer_formatter(field_width, show_sign);
}

bool
vil_nitf2_integer_formatter::read_vcl_stream(std::istream& input,
                                             int& out_value, bool& out_blank)
{
  char* cstr;
  if (!read_c_str(input, field_width, cstr, out_blank)) {
    delete[] cstr;
    return false;
  }
  char* endp;
  errno = 0;
  out_value = (int)strtol(cstr, &endp, 10);
  bool sign_ok = check_sign(cstr, show_sign);
  bool retVal = (endp-cstr)==field_width // processed all chars
         && errno==0              // with no errors
         && sign_ok;              // sign shown as expected
  delete[] cstr;
  return retVal;
}

bool vil_nitf2_integer_formatter::write_vcl_stream(std::ostream& output, const int& value)
{
  output << std::setw(field_width) << std::right << std::setfill('0');
  if (show_sign) {
    output << std::showpos;
  } else {
    output << std::noshowpos;
  }
  output << value;
  return !output.fail();
}

//==============================================================================
// Class vil_nitf2_long_long_formatter

vil_nitf2_long_long_formatter::
vil_nitf2_long_long_formatter(int field_width, bool show_sign)
  : vil_nitf2_typed_field_formatter<vil_nitf2_long>(vil_nitf2::type_long_long, field_width),
    show_sign(show_sign)
{}

vil_nitf2_field_formatter* vil_nitf2_long_long_formatter::copy() const
{
  return new vil_nitf2_long_long_formatter(field_width, show_sign);
}

bool vil_nitf2_long_long_formatter::
read_vcl_stream(std::istream& input, vil_nitf2_long& out_value, bool& out_blank)
{
  char* cstr;
  if (!read_c_str(input, field_width, cstr, out_blank)) {
    delete[] cstr;
    return false;
  }
  bool conversion_ok;
  char* endp;
  errno = 0;

#if VXL_HAS_INT_64

#if defined _MSC_VER
  out_value = _strtoi64(cstr, &endp, 10);
  conversion_ok = (endp-cstr)==field_width;   // processed all chars
#else
  out_value = ::strtoll(cstr, &endp, 10);     // in Standard C Library
  conversion_ok = (endp-cstr)==field_width;   // processed all chars
#endif

#else //VXL_HAS_INT_64
  out_value = strtol(cstr, &endp, 10);
  conversion_ok = (endp-cstr)==field_width;   // processed all chars
#endif //VXL_HAS_INT_64

  bool sign_ok = check_sign(cstr, show_sign);
  delete[] cstr;
  return conversion_ok
         && errno==0              // with no errors
         && sign_ok;              // sign shown as expected
}

bool vil_nitf2_long_long_formatter::write_vcl_stream(std::ostream& output, const vil_nitf2_long& value)
{
  output << std::setw(field_width) << std::right << std::setfill('0');
  if (show_sign) {
    output << std::showpos;
  } else {
    output << std::noshowpos;
  }
  output << value;
  return !output.fail();
}

//==============================================================================
// Class vil_nitf2_double_formatter

vil_nitf2_double_formatter::
vil_nitf2_double_formatter(int field_width, int precision, bool show_sign)
  : vil_nitf2_typed_field_formatter<double>(vil_nitf2::type_double, field_width),
    precision(precision),
    show_sign(show_sign)
{}

vil_nitf2_field_formatter* vil_nitf2_double_formatter::copy() const
{
  return new vil_nitf2_double_formatter(field_width, precision, show_sign);
}

bool vil_nitf2_double_formatter::read_vcl_stream(std::istream& input,
                                                 double& out_value, bool& out_blank)
{
  char* cstr;
  if (!read_c_str(input, field_width, cstr, out_blank)) {
    delete[] cstr;
    return false;
  }
  char* endp;
  errno=0;
  out_value = strtod(cstr, &endp);
  bool sign_ok = check_sign(cstr, show_sign);
  bool decimal_ok = cstr[(field_width-precision)-1]=='.';
  bool retVal =
    (endp-cstr)==field_width  // processed all chars
     && errno==0              // with no errors
     && decimal_ok            // decimal point in right place
     && sign_ok;              // sign shown as expected
  delete[] cstr;
  return retVal;
}

bool vil_nitf2_double_formatter::write_vcl_stream(std::ostream& output, const double& value)
{
  output << std::setw(field_width) << std::fixed;
  if (show_sign) {
    output << std::showpos;
  } else {
    output << std::noshowpos;
  }
  output << std::internal << std::setfill('0') << std::setprecision(precision)
         << value;
  return !output.fail();
}

//==============================================================================
// Class vil_nitf2_exponential_formatter

vil_nitf2_exponential_formatter::
vil_nitf2_exponential_formatter(int mantissa_width, int exponent_width)
  : vil_nitf2_typed_field_formatter<double>(vil_nitf2::type_double,
                                            mantissa_width + exponent_width + 5),
    mantissa_width(mantissa_width),
    exponent_width(exponent_width)
{}

vil_nitf2_field_formatter* vil_nitf2_exponential_formatter::copy() const
{
  return new vil_nitf2_exponential_formatter(mantissa_width, exponent_width);
}

bool vil_nitf2_exponential_formatter::read_vcl_stream(std::istream& input,
                                                      double& out_value, bool& out_blank)
{
  char* cstr;
  if (!read_c_str(input, field_width, cstr, out_blank)) {
    delete[] cstr;
    return false;
  }
  char* endp;
  errno=0;
  out_value = strtod(cstr, &endp);
  // Check locations of non-digits in format: +d.dddddE+dd
  const char base_sign = cstr[0];
  const bool base_sign_ok = base_sign=='+' || base_sign=='-';
  const bool decimal_ok = cstr[2]=='.';
  const char e_ok = cstr[3+mantissa_width]=='E';
  const char exp_sign = cstr[4+mantissa_width];
  const bool exp_sign_ok = exp_sign=='+' || exp_sign=='-';
  bool retVal =
    (endp-cstr)==field_width   // processed all chars
     && errno==0               // read a number with no errors
     && base_sign_ok           // base sign in right place
     && decimal_ok             // decimal point in right place
     && e_ok                   // 'E' in right place
     && exp_sign_ok;           // exponent sign in right place
  delete[] cstr;
  return retVal;
}

bool vil_nitf2_exponential_formatter::write_vcl_stream(std::ostream& output,
                                                       const double& value)
{
  // Can't control the width of exponent (it's 3) so write it to a buffer first
  std::ostringstream buffer;
  buffer << std::setw(field_width) << std::scientific
         << std::showpos << std::uppercase
         << std::internal << std::setfill('0') << std::setprecision(mantissa_width)
         << value;
  std::string buffer_string = buffer.str();
  auto length = (unsigned int)(buffer_string.length());
  // Write everything up to the exponent sign
  output << buffer_string.substr(0,length-3);
  // Write exponent digits, padding or unpadding them to desired width
  output << std::setw(exponent_width) << std::setfill('0')
         << buffer_string.substr(length-std::min(3,exponent_width), std::min(3,exponent_width));
  return !output.fail();
}

//==============================================================================
// Class vil_nitf2_binary_formatter

vil_nitf2_binary_formatter::vil_nitf2_binary_formatter(int width_bytes)
  : vil_nitf2_typed_field_formatter<void*>(vil_nitf2::type_binary, width_bytes)
{}

vil_nitf2_field_formatter* vil_nitf2_binary_formatter::copy() const
{
  return new vil_nitf2_binary_formatter(field_width);
}

bool vil_nitf2_binary_formatter::read(vil_stream& input, void*& out_value,
                                      bool& out_blank)
{
  out_value = (void*)(new char[field_width]);
  out_blank = false; //no such thing as 'blank' binary data
  return input.read( out_value, field_width ) == field_width;
}

bool vil_nitf2_binary_formatter::write(vil_nitf2_ostream& output, void*const& value)
{
  return output.write( value, field_width ) == field_width;
}

//==============================================================================
// Class vil_nitf2_char_formatter

vil_nitf2_char_formatter::vil_nitf2_char_formatter()
  : vil_nitf2_typed_field_formatter<char>(vil_nitf2::type_char, 1)
{}

vil_nitf2_field_formatter* vil_nitf2_char_formatter::copy() const
{
  return new vil_nitf2_char_formatter();
}

bool vil_nitf2_char_formatter::read_vcl_stream(std::istream& input, char& out_value, bool& out_blank)
{
  input.get(out_value);
  //int numRead = input.read(&out_value, 1);
  out_blank = (out_value == ' ');
  return !input.fail();
  //return numRead == 1;
}

bool vil_nitf2_char_formatter::write_vcl_stream(std::ostream& output, const char& value)
{
  output << value;
  return !output.fail();
}

//==============================================================================
// Class vil_nitf2_string_formatter

vil_nitf2_string_formatter::
vil_nitf2_string_formatter(int field_width, enum_char_set char_set)
  : vil_nitf2_typed_field_formatter<std::string>(vil_nitf2::type_string, field_width),
    char_set(char_set)
{}

vil_nitf2_field_formatter* vil_nitf2_string_formatter::copy() const
{
  return new vil_nitf2_string_formatter(field_width, char_set);
}

bool vil_nitf2_string_formatter::read_vcl_stream(std::istream& input,
                                                 std::string& out_value,
                                                 bool& out_blank)
{
  char* cstr;
  if (!read_c_str(input, field_width, cstr, out_blank)) {
    delete[] cstr;
    return false;
  }
  std::string str = std::string(cstr);
  delete[] cstr;
  std::string::size_type end_pos = str.find_last_not_of(' ')+1;
  if (end_pos == std::string::npos) {
    out_value = str;
  } else {
    out_value = str.substr(0, end_pos);
  }
  return is_valid(out_value);
}

bool vil_nitf2_string_formatter::write_vcl_stream(std::ostream& output, const std::string& value)
{
  output << std::setw(field_width) << std::left << std::setfill(' ') << value;
  return !output.fail();
}

bool vil_nitf2_string_formatter::is_valid(std::string /*value*/) const
{
  // to do: check char set
  return true;
}

//==============================================================================
// Class vil_nitf2_enum_string_formatter

vil_nitf2_enum_string_formatter::
vil_nitf2_enum_string_formatter(int field_width, vil_nitf2_enum_values  value_map)
  : vil_nitf2_string_formatter(field_width), value_map(std::move(value_map))
{
  //field_type = Nitf::Enum;
  validate_value_map();
}

vil_nitf2_field_formatter* vil_nitf2_enum_string_formatter::copy() const
{
  return new vil_nitf2_enum_string_formatter(field_width, value_map);
}

void vil_nitf2_enum_string_formatter::validate_value_map()
{
  for (auto & entry : value_map)
  {
    std::string token = entry.first;
#if 0  // disable the err message
    if (int(token.length()) > field_width) {
      //std::cerr << "vil_nitf2_enum_values: WARNING: Ignoring token "
      //         << token << "; length exceeds declared field width.\n";
      // Should probably remove it so that is_valid() doesn't match it.
      // On the other hand, this class will never read a token of this
      // length, so don't worry about it.
    }
#endif
  }
}

bool vil_nitf2_enum_string_formatter::is_valid_value(const std::string& token) const
{
  return value_map.find(token) != value_map.end();
}

//==============================================================================
// Class vil_nitf2_enum_values

vil_nitf2_enum_values& vil_nitf2_enum_values::value(std::string token, std::string pretty_name)
{
  if (!insert(std::make_pair(token, pretty_name)).second) {
    std::cerr << "vil_nitf2_enum_values: WARNING: Ignoring definition "
             << token << "; token already defined for this enumeration.\n";
  }
  return *this;
}

//==============================================================================
// Class vil_nitf2_tre_sequence_formatter

#include "vil_nitf2_tagged_record.h"

vil_nitf2_tagged_record_sequence_formatter::vil_nitf2_tagged_record_sequence_formatter()
  : vil_nitf2_typed_field_formatter<vil_nitf2_tagged_record_sequence>(
      vil_nitf2::type_tagged_record_sequence, 1 /* ignored */)
{}

vil_nitf2_field_formatter*
vil_nitf2_tagged_record_sequence_formatter::copy() const
{
  return new vil_nitf2_tagged_record_sequence_formatter();
}

bool vil_nitf2_tagged_record_sequence_formatter::
read( vil_nitf2_istream& input,
      vil_nitf2_tagged_record_sequence& out_value, bool& out_blank )
{
  if (field_width <= 0) return false;
  vil_streampos current = input.tell();
  vil_streampos end = current + field_width;
  bool error_reading_tre = false;
  out_value.clear();
  while (input.tell() < end && !error_reading_tre) {
    vil_nitf2_tagged_record* record = vil_nitf2_tagged_record::create(input);
    if (record) {
      out_value.push_back(record); // out_value assumes ownership of record
    }
    error_reading_tre &= !record;
  }
  if (input.tell() != end) { // TO DO: what does end equal at EOF?
    VIL_NITF2_LOG(log_info) << "\nSeeking to end of TRE sequence field.\n";
    input.seek(end);
    if (input.tell() != end) {
      std::cerr << "\nSeek to end of TRE sequence field failed.\n";
      error_reading_tre = true;
    }
  }
  out_blank = false;
  return !error_reading_tre;
}

bool vil_nitf2_tagged_record_sequence_formatter::
write(vil_nitf2_ostream& /*output*/, const vil_nitf2_tagged_record_sequence& /*value*/ )
{
  return false;
}
