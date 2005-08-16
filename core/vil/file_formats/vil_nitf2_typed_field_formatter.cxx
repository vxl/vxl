// vil_nitf2: Written by Harry Voorhees (hlv@) and Rob Radtke (rob@) of
// Stellar Science Ltd. Co. (stellarscience.com) for 
// Air Force Research Laboratory, 2005.

#include "vil_nitf2_typed_field_formatter.h"

#include <vcl_iomanip.h>
#include <vcl_iostream.h>
#include <vcl_utility.h>
//==============================================================================
// Class vil_nitf2_date_time_formatter

vil_nitf2_date_time_formatter::vil_nitf2_date_time_formatter(int field_width) 
  : vil_nitf2_typed_field_formatter<vil_nitf2_date_time>(vil_nitf2::type_date_time, field_width) 
{};

bool vil_nitf2_date_time_formatter::read(vcl_istream& input, vil_nitf2_date_time& out_value, bool& out_blank)
{
  return out_value.read(input, field_width, out_blank);
}

bool vil_nitf2_date_time_formatter::write(vcl_ostream& output, const vil_nitf2_date_time& value)
{
  return value.write(output, field_width);
}

//==============================================================================
// Class vil_nitf2_location_formatter

vil_nitf2_location_formatter::vil_nitf2_location_formatter(int field_width) 
  : vil_nitf2_typed_field_formatter<vil_nitf2_location*>(vil_nitf2::type_location, field_width) 
{}

bool vil_nitf2_location_formatter::read(vcl_istream& input, 
                                 vil_nitf2_location*& out_value, bool& out_blank)
{
  vcl_streampos tag_start_pos = input.tellg();
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
      out_value = 0;
      return false;
    }
  }
}

bool vil_nitf2_location_formatter::write(vcl_ostream& output, vil_nitf2_location*const& value)
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

bool
vil_nitf2_integer_formatter::read(vcl_istream& input, 
                           int& out_value, bool& out_blank)
{
  const char* cstr;
  if (!read_c_str(input, field_width, cstr, out_blank)) {
    return false;
  }
  char* endp;
  errno = 0;
  out_value = (int)strtol(cstr, &endp, 10);
  bool sign_ok = check_sign(cstr, show_sign);
  delete[] cstr;
  return ((endp-cstr)==field_width  // processed all chars
          && errno==0              // with no errors
          && sign_ok);              // sign shown as expected
}

bool vil_nitf2_integer_formatter::write(vcl_ostream& output, const int& value)
{
  output << vcl_setw(field_width) << vcl_right << vcl_setfill('0'); 
  if (show_sign) {
    output << vcl_showpos;
  } else {
    output << vcl_noshowpos;
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
{};

bool vil_nitf2_long_long_formatter::
read(vcl_istream& input, vil_nitf2_long& out_value, bool& out_blank)
{
  const char* cstr;
  if (!read_c_str(input, field_width, cstr, out_blank)) {
    return false;
  }
  char* endp;
  errno = 0;

#if VXL_HAS_INT_64
#if defined(WIN32)
  out_value = _strtoi64(cstr, &endp, 10);
#else
  out_value = ::strtoll(cstr, &endp, 10);  // in Standard C Library
#endif
#else //VXL_HAS_INT_64
  out_value = strtoi(cstr, &endp, 10);
#endif //VXL_HAS_INT_64
  
  bool sign_ok = check_sign(cstr, show_sign);
  delete[] cstr;
  return ((endp-cstr)==field_width  // processed all chars
          && errno==0              // with no errors
          && sign_ok);              // sign shown as expected
}

bool vil_nitf2_long_long_formatter::write(vcl_ostream& output, const vil_nitf2_long& value)
{
  output << vcl_setw(field_width) << vcl_right << vcl_setfill('0'); 
  if (show_sign) {
    output << vcl_showpos;
  } else {
    output << vcl_noshowpos;
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
{};

bool vil_nitf2_double_formatter::read(vcl_istream& input, 
                               double& out_value, bool& out_blank)
{
  const char* cstr;
  if (!read_c_str(input, field_width, cstr, out_blank)) {
    return false;
  }
  char* endp;
  errno=0;
  out_value = strtod(cstr, &endp);
  bool sign_ok = check_sign(cstr, show_sign);
  bool decimal_ok = cstr[(field_width-precision)-1]=='.';
  return 
    (endp-cstr)==field_width  // processed all chars 
     && errno==0              // with no errors
     && decimal_ok            // decimal point in right place
     && sign_ok;              // sign shown as expected
}

bool vil_nitf2_double_formatter::write(vcl_ostream& output, const double& value)
{
  output << vcl_setw(field_width) << vcl_fixed;
  if (show_sign) {
    output << vcl_showpos;
  } else {
    output << vcl_noshowpos;
  }
  output << vcl_internal << vcl_setfill('0') << vcl_setprecision(precision);
  output << value;
  return !output.fail();
}

//==============================================================================
// Class vil_nitf2_binary_formatter

vil_nitf2_binary_formatter::vil_nitf2_binary_formatter(int widthBytes) 
  : vil_nitf2_typed_field_formatter<void*>(vil_nitf2::type_binary, widthBytes) 
{};

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
{};

bool vil_nitf2_char_formatter::read(vcl_istream& input, char& out_value, bool& out_blank)
{
  input.get(out_value);
  //int numRead = input.read(&out_value, 1);
  out_blank = (out_value == ' ');
  return !input.fail();
  //return numRead == 1;
}

bool vil_nitf2_char_formatter::write(vcl_ostream& output, const char& value)
{
  output << value;
  return !output.fail();
}

//==============================================================================
// Class vil_nitf2_string_formatter

vil_nitf2_string_formatter::
vil_nitf2_string_formatter(int field_width, enum_char_set char_set) 
  : vil_nitf2_typed_field_formatter<vcl_string>(vil_nitf2::type_string, field_width), 
    char_set(char_set) 
{};

bool vil_nitf2_string_formatter::read(vcl_istream& input, 
                               vcl_string& out_value, bool& out_blank)
{
  const char* cstr;
  if (!read_c_str(input, field_width, cstr, out_blank)) {
    return false;
  }
  vcl_string str = vcl_string(cstr);
  vcl_string::size_type end_pos = str.find_last_not_of(" ")+1;
  if (end_pos == vcl_string::npos) {
    out_value = str;
  } else {
    out_value = str.substr(0, end_pos);
  }
  return is_valid(out_value);
}

bool vil_nitf2_string_formatter::write(vcl_ostream& output, const vcl_string& value)
{
  output << vcl_setw(field_width) << vcl_left << vcl_setfill(' ') << value;
  return !output.fail();
}

bool vil_nitf2_string_formatter::is_valid(vcl_string /*value*/) const
{ 
  // to do: check char set
  return true;
}

vil_nitf2_enum_string_formatter::
vil_nitf2_enum_string_formatter(int field_width, const vil_nitf2_enum_values& value_map)
  : vil_nitf2_string_formatter(field_width), value_map(value_map)
{
  //field_type = Nitf::Enum;
  validate_value_map();
}

//==============================================================================
// Class vil_nitf2_enum_string_formatter

void vil_nitf2_enum_string_formatter::validate_value_map()
{
  for (vil_nitf2_enum_values::iterator entry = value_map.begin();
    entry != value_map.end(); ++entry) 
  {
    vcl_string token = entry->first;
    if (int(token.length()) > field_width) {
      vcl_cerr << "vil_nitf2_enum_values: WARNING: Ignoring token " 
        << token << "; length exceeds declared field width." << vcl_endl;
      // Should probably remove it so that is_valid() doesn't match it.
      // On the other hand, this class will never read a token of this
      // length, so don't worry about it.
    }
  }
}

bool vil_nitf2_enum_string_formatter::is_valid_value(vcl_string token) const
{
  return value_map.find(token) != value_map.end();
}

//==============================================================================
// Class vil_nitf2_enum_values

vil_nitf2_enum_values& vil_nitf2_enum_values::value(vcl_string token, vcl_string pretty_name)
{
  if (!insert(vcl_make_pair(token, pretty_name)).second) {
    vcl_cerr << "vil_nitf2_enum_values: WARNING: Ignoring definition " 
      << token << "; token already defined for this enumeration." << vcl_endl;
  } 
  return *this; 
}
