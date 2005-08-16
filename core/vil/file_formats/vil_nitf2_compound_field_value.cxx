// vil_nitf2: Written by Harry Voorhees (hlv@) and Rob Radtke (rob@) of
// Stellar Science Ltd. Co. (stellarscience.com) for 
// Air Force Research Laboratory, 2005.

#include "vil_nitf2_compound_field_value.h"

#include <vcl_sstream.h>
#include <vcl_iomanip.h>
#include <vcl_string.h>

#include "vil_nitf2_typed_field_formatter.h"

//==============================================================================
// Class vil_nitf2_date_time

vcl_ostream& vil_nitf2_date_time::output(vcl_ostream& os) const
{
  os << year << "/"
     << vcl_setw(2) << vcl_setfill('0') << month << "/"
     << vcl_setw(2) << vcl_setfill('0') << day << " "
     << vcl_setw(2) << vcl_setfill('0') << hour << ":"
     << vcl_setw(2) << vcl_setfill('0') << minute << ":";
  if (second < 10) os << '0';
  if (sec_precision==0) {
    os << int(second);
  } else if (sec_precision>0) {
    os << vcl_fixed << vcl_setprecision(sec_precision) << second;
  }
  return os;
}

bool vil_nitf2_date_time::is_valid() const
{
  return year > 1900 && year <= 9999 &&
         month > 0 && month <= 12 &&
         day > 0 && day <= 31 &&
         hour >= 0 && hour < 24 &&
         minute >=0 && minute < 60 &&
         second >= 0.0 && second < 60.0;
}

bool vil_nitf2_date_time::write(vcl_ostream& output, int field_width) const
{
  output << vcl_setw(4) << vcl_noshowpos << vcl_internal << year
         << vcl_setw(2) << vcl_noshowpos << vcl_internal << vcl_setfill('0') << month
         << vcl_setw(2) << vcl_noshowpos << vcl_internal << vcl_setfill('0') << day;
  if (field_width >= 10 && !output.fail()) {
    output << vcl_setw(2) << vcl_noshowpos << vcl_internal << vcl_setfill('0') << hour;
  } else output << "  ";
  if (field_width >= 12 && !output.fail()) {
    output << vcl_setw(2) << vcl_noshowpos << vcl_internal << vcl_setfill('0') << minute;
  } else output << "  ";
  if (field_width < 14 && !output.fail()) {
    // seconds not displayed
    output << "  ";
  } else if (field_width == 14 && !output.fail()) {
    // display integer seconds
    output << vcl_setw(2) << vcl_noshowpos << vcl_internal << vcl_setfill('0') << (int)second;
  } else if (!output.fail()) {
    // display decimal seconds
    output << vcl_setw(field_width - 12) << vcl_fixed << vcl_noshowpos << vcl_internal
      << vcl_setfill(' ') << vcl_setprecision(field_width - 15) << second;
  }
  // Return whether all output operations were successful
  return !output.fail();
}

bool vil_nitf2_date_time::read(vcl_istream& input, int field_width, bool& out_blank)
{
  bool blank;
  vcl_string fieldStr;
  bool ok;
  ok = vil_nitf2_integer_formatter(4).read(input, year, blank); out_blank = blank;
  ok &= vil_nitf2_integer_formatter(2).read(input, month, blank); out_blank &= blank;
  ok &= vil_nitf2_integer_formatter(2).read(input, day, blank); out_blank &= blank;
  if (field_width >= 10) {
    ok &= vil_nitf2_integer_formatter(2).read(input, hour, blank); out_blank &= blank;
  } else { 
    hour = 0; 
  }
  if (field_width >= 12) {
    ok &= vil_nitf2_integer_formatter(2).read(input, minute, blank); out_blank &= blank;
  } else {
    minute = 0;
  }
  if (field_width == 14) {
    // integer seconds, no decimal point
    int intSecond;
    ok &= vil_nitf2_integer_formatter(2).read(input, intSecond, blank); out_blank &= blank;
    second = intSecond;
  } else if (field_width > 14) {
    // decimal seconds
    ok &= vil_nitf2_double_formatter(field_width-12, field_width-15, false)
      .read(input, second, blank); out_blank &= blank;
  } else {
    // no seconds
    second = 0.0;
  }
  return ok && is_valid();
}

vcl_ostream& operator << (vcl_ostream& os, const vil_nitf2_date_time& dateTime)
{
  return dateTime.output(os);
}

//==============================================================================
// Class vil_nitf2_location_degrees

vcl_ostream& vil_nitf2_location_degrees::output(vcl_ostream& os) const
{
  os << "(" 
     << vcl_fixed << lat_degrees << ", " 
     << vcl_fixed << lon_degrees << ")";
  return os;
}

bool vil_nitf2_location_degrees::read(vcl_istream& input, int field_width, bool& out_blank)
{
  int lat_width = (field_width-1)/2;
  int lon_width = (field_width+1)/2;
  bool ok, blank;
  ok = vil_nitf2_double_formatter(lat_width, precision, true).read(input, lat_degrees, blank);
  out_blank = blank;
  ok &= vil_nitf2_double_formatter(lon_width, precision, true).read(input, lon_degrees, out_blank);
  out_blank &= blank;
  return ok && is_valid();
}

bool vil_nitf2_location_degrees::write(vcl_ostream& output, int field_width)
{
  // Could someone remind me again why I didn't just use printf and scanf 
  // instead?
  output << vcl_setw((field_width-1)/2) << vcl_fixed << vcl_showpos << vcl_internal 
         << vcl_setfill('0') <<  vcl_setprecision(precision) << lat_degrees
         << vcl_setw((field_width+1)/2) << vcl_fixed << vcl_showpos << vcl_internal
         << vcl_setfill('0') << vcl_setprecision(precision) << lon_degrees;
  return !output.fail();
}

bool vil_nitf2_location_degrees::is_valid() const
{
  return lat_degrees >= -90.0 && lat_degrees <= 90.0 &&
         lon_degrees >= -180.0 && lon_degrees <= 180.0;
}


//==============================================================================
// Class vil_nitf2_location

vcl_ostream& operator << (vcl_ostream& os, const vil_nitf2_location& loc)
{
  return loc.output(os);
}

//==============================================================================
// Class vil_nitf2_location_dmsh

vcl_ostream& vil_nitf2_location_dmsh::output(vcl_ostream& os) const
{
  os << "(" 
     << lat_degrees << ':' << lat_minutes    << ":" 
     << lat_seconds << ':' << lat_hemisphere << ", "
     << lon_degrees << ':' << lon_minutes    << ":" 
     << lon_seconds << ':' << lon_hemisphere << ")";
  return os;
}

bool vil_nitf2_location_dmsh::read(vcl_istream& input, int /* field_width */, bool& out_blank)
{
  bool ok, blank;
  // Read latitude fields
  ok = vil_nitf2_integer_formatter(2).read(input, lat_degrees, blank); 
       out_blank &= blank;
  ok &= vil_nitf2_integer_formatter(2).read(input, lat_minutes, out_blank); 
        out_blank &= blank;
  ok &= vil_nitf2_double_formatter(3+sec_precision, sec_precision, false)
         .read(input, lat_seconds, out_blank);
        out_blank &= blank;
  ok &= vil_nitf2_char_formatter().read(input, lat_hemisphere, out_blank);
        out_blank &= blank;
  // Read longitude fields (degrees is one digit longer than latitude)
  ok &= vil_nitf2_integer_formatter(3).read(input, lon_degrees, out_blank);
        out_blank &= blank;
  ok &= vil_nitf2_integer_formatter(2).read(input, lon_minutes, out_blank);
        out_blank &= blank;
  ok &= vil_nitf2_double_formatter(3+sec_precision, sec_precision, false)
         .read(input, lon_seconds, out_blank);
        out_blank &= blank;
  ok = vil_nitf2_char_formatter().read(input,lon_hemisphere, out_blank);
       out_blank &= blank;
  return ok && is_valid();
}

bool vil_nitf2_location_dmsh::write(vcl_ostream& output, int /* field_width */)
{
  bool ok;
  // Write latitude fields
  ok = vil_nitf2_integer_formatter(2).write(output, lat_degrees);
  ok &= vil_nitf2_integer_formatter(2).write(output, lat_minutes);
  ok &= vil_nitf2_double_formatter(3+sec_precision, sec_precision, false).write(output, lat_seconds);
  ok &= vil_nitf2_char_formatter().write(output, lat_hemisphere);
  // Write longitude fields (degrees is one digit longer than latitude)
  ok = vil_nitf2_integer_formatter(3).write(output, lon_degrees);
  ok &= vil_nitf2_integer_formatter(2).write(output, lon_minutes);
  ok &= vil_nitf2_double_formatter(3+sec_precision, sec_precision, false).write(output, lon_seconds);
  ok &= vil_nitf2_char_formatter().write(output, lon_hemisphere);
  return ok;
}

bool vil_nitf2_location_dmsh::is_valid() const
{
  return lat_degrees >= -90 && lat_degrees <= 90 &&
         lon_degrees >= -180 && lon_degrees <= 180 &&
         lat_minutes >= 0 && lat_minutes < 60 &&
         lon_minutes >= 0 && lon_minutes < 60 &&
         lat_seconds >= 0.0 && lat_seconds < 60.0 &&
         lon_seconds >= 0.0 && lon_seconds < 60.0 &&
         vcl_string("NnSs").find(lat_hemisphere) != vcl_string::npos &&
         vcl_string("EeWw").find(lon_hemisphere) != vcl_string::npos;
}
