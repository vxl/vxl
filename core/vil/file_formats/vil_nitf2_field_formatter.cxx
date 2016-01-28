// vil_nitf2: Written by Harry Voorhees (hlv@) and Rob Radtke (rob@) of
// Stellar Science Ltd. Co. (stellarscience.com) for
// Air Force Research Laboratory, 2005.

#include "vil_nitf2_field_formatter.h"
#include "vil_nitf2_field.h"

// not used? #include <vcl_iomanip.h>
#include <vcl_iostream.h>
#include <vcl_cstring.h> // for std::strlen()

// ==============================================================================
// Class vil_nitf2_field_formatter

char * vil_nitf2_field_formatter::read_char_array(vcl_istream& input, int size)
{
  char* char_array = new char[size + 1];

  input.read(char_array, size);
  char_array[input.gcount()] = '\0';
  return char_array;
}

vcl_string vil_nitf2_field_formatter::read_string(vcl_istream& input, int size)
{
  char*      cstr = read_char_array(input, size);
  vcl_string str = vcl_string(cstr);

  delete[] cstr;
  return str;
}

vcl_string vil_nitf2_field_formatter::read_string(vil_stream& input, int size)
{
  char*         char_array = new char[size + 1];
  vil_streampos pos = input.read(char_array, size);

  char_array[pos] = '\0';
  vcl_string retVal(char_array);
  delete[] char_array;
  return retVal;
}

bool vil_nitf2_field_formatter::read_c_str(vcl_istream& input, int length,
                                           char *& out_cstr, bool& all_blank)
{
  out_cstr = read_char_array(input, length);
  all_blank = is_all_blank(out_cstr);
  return int(vcl_strlen(out_cstr) ) == length;
}

bool vil_nitf2_field_formatter::write_blank(vcl_ostream& output)
{
  vcl_string str(field_width, ' ');

  output << str;
  return !output.fail();
}

bool vil_nitf2_field_formatter::write_blank(vil_nitf2_ostream& output)
{
  vcl_string str(field_width, ' ');

  output.write(str.c_str(), field_width);
  return output.ok();
}

bool vil_nitf2_field_formatter::is_all_blank(const char* cstr)
{
  while( *cstr != '\0' )
    {
    if( *cstr != ' ' ) {return false; }
    ++cstr;
    }

  return true;
}

bool vil_nitf2_field_formatter::check_sign(const char* cstr, bool show_sign)
{
  return cstr &&
         ( (!show_sign && cstr[0] != '+' && cstr[0] != '-') ||
           (show_sign && (cstr[0] == '+' || cstr[0] == '-') )  );
}
