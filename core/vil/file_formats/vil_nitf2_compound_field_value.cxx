// vil_nitf2: Written by Harry Voorhees (hlv@) and Rob Radtke (rob@) of
// Stellar Science Ltd. Co. (stellarscience.com) for
// Air Force Research Laboratory, 2005.

#include <iomanip>
#include <string>
#include "vil_nitf2_compound_field_value.h"

// not used? #include <sstream>
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif

#include "vil_nitf2_typed_field_formatter.h"

//==============================================================================
// Class vil_nitf2_date_time

std::ostream &
vil_nitf2_date_time::output(std::ostream & os) const
{
  os << year << '/' << std::setw(2) << std::setfill('0') << month << '/' << std::setw(2) << std::setfill('0') << day
     << ' ' << std::setw(2) << std::setfill('0') << hour << ':' << std::setw(2) << std::setfill('0') << minute << ':';
  if (second < 10)
    os << '0';
  if (sec_precision == 0)
  {
    os << int(second);
  }
  else if (sec_precision > 0)
  {
    os << std::fixed << std::setprecision(sec_precision) << second;
  }
  return os;
}

bool
vil_nitf2_date_time::is_valid() const
{
  return year > 1900 && year <= 9999 && month > 0 && month <= 12 && day > 0 && day <= 31 && hour >= 0 && hour < 24 &&
         minute >= 0 && minute < 60 && second >= 0.0 && second < 60.0;
}

bool
vil_nitf2_date_time::write(std::ostream & output, int field_width) const
{
  output << std::setw(4) << std::noshowpos << std::internal << year << std::setw(2) << std::noshowpos << std::internal
         << std::setfill('0') << month << std::setw(2) << std::noshowpos << std::internal << std::setfill('0') << day;
  if (field_width >= 10 && !output.fail())
  {
    output << std::setw(2) << std::noshowpos << std::internal << std::setfill('0') << hour;
  }
  else
    output << "  ";
  if (field_width >= 12 && !output.fail())
  {
    output << std::setw(2) << std::noshowpos << std::internal << std::setfill('0') << minute;
  }
  else
    output << "  ";
  if (field_width < 14 && !output.fail())
  {
    // seconds not displayed
    output << "  ";
  }
  else if (field_width == 14 && !output.fail())
  {
    // display integer seconds
    output << std::setw(2) << std::noshowpos << std::internal << std::setfill('0') << (int)second;
  }
  else if (!output.fail())
  {
    // display decimal seconds
    output << std::setw(field_width - 12) << std::fixed << std::noshowpos << std::internal << std::setfill(' ')
           << std::setprecision(field_width - 15) << second;
  }
  // Return whether all output operations were successful
  return !output.fail();
}

bool
vil_nitf2_date_time::read(std::istream & input, int field_width, bool & out_blank)
{
  bool        blank;
  std::string fieldStr;
  bool        ok;
  ok = vil_nitf2_integer_formatter(4).read_vcl_stream(input, year, blank);
  out_blank = blank;
  ok &= vil_nitf2_integer_formatter(2).read_vcl_stream(input, month, blank);
  out_blank &= blank;
  ok &= vil_nitf2_integer_formatter(2).read_vcl_stream(input, day, blank);
  out_blank &= blank;
  if (field_width >= 10)
  {
    ok &= vil_nitf2_integer_formatter(2).read_vcl_stream(input, hour, blank);
    out_blank &= blank;
  }
  else
  {
    hour = 0;
  }
  if (field_width >= 12)
  {
    ok &= vil_nitf2_integer_formatter(2).read_vcl_stream(input, minute, blank);
    out_blank &= blank;
  }
  else
  {
    minute = 0;
  }
  if (field_width == 14)
  {
    // integer seconds, no decimal point
    int intSecond;
    ok &= vil_nitf2_integer_formatter(2).read_vcl_stream(input, intSecond, blank);
    out_blank &= blank;
    second = intSecond;
  }
  else if (field_width > 14)
  {
    // decimal seconds
    ok &= vil_nitf2_double_formatter(field_width - 12, field_width - 15, false).read_vcl_stream(input, second, blank);
    out_blank &= blank;
  }
  else
  {
    // no seconds
    second = 0.0;
  }
  return ok && is_valid();
}

std::ostream &
operator<<(std::ostream & os, const vil_nitf2_date_time & dateTime)
{
  return dateTime.output(os);
}

//==============================================================================
// Class vil_nitf2_location_degrees

std::ostream &
vil_nitf2_location_degrees::output(std::ostream & os) const
{
  os << '(' << std::fixed << lat_degrees << ", " << std::fixed << lon_degrees << ')';
  return os;
}

bool
vil_nitf2_location_degrees::read(std::istream & input, int field_width, bool & out_blank)
{
  int  lat_width = (field_width - 1) / 2;
  int  lon_width = (field_width + 1) / 2;
  bool ok, blank;
  ok = vil_nitf2_double_formatter(lat_width, precision, true).read_vcl_stream(input, lat_degrees, blank);
  out_blank = blank;
  ok &= vil_nitf2_double_formatter(lon_width, precision, true).read_vcl_stream(input, lon_degrees, out_blank);
  out_blank &= blank;
  return ok && is_valid();
}

bool
vil_nitf2_location_degrees::write(std::ostream & output, int field_width)
{
  // Could someone remind me again why I didn't just use printf and scanf
  // instead?
  output << std::setw((field_width - 1) / 2) << std::fixed << std::showpos << std::internal << std::setfill('0')
         << std::setprecision(precision) << lat_degrees << std::setw((field_width + 1) / 2) << std::fixed
         << std::showpos << std::internal << std::setfill('0') << std::setprecision(precision) << lon_degrees;
  return !output.fail();
}

bool
vil_nitf2_location_degrees::is_valid() const
{
  return lat_degrees >= -90.0 && lat_degrees <= 90.0 && lon_degrees >= -180.0 && lon_degrees <= 180.0;
}


//==============================================================================
// Class vil_nitf2_location

std::ostream &
operator<<(std::ostream & os, const vil_nitf2_location & loc)
{
  return loc.output(os);
}

//==============================================================================
// Class vil_nitf2_location_dmsh

std::ostream &
vil_nitf2_location_dmsh::output(std::ostream & os) const
{
  os << '(' << lat_degrees << ':' << lat_minutes << ':' << lat_seconds << ':' << lat_hemisphere << ", " << lon_degrees
     << ':' << lon_minutes << ':' << lon_seconds << ':' << lon_hemisphere << ')';
  return os;
}

bool
vil_nitf2_location_dmsh::read(std::istream & input, int /* field_width */, bool & out_blank)
{
  bool blank;
  // Read latitude fields
  bool ok = vil_nitf2_integer_formatter(2).read_vcl_stream(input, lat_degrees, blank);
  if (out_blank)
    out_blank = blank;
  if (ok)
    ok = vil_nitf2_integer_formatter(2).read_vcl_stream(input, lat_minutes, out_blank);
  if (out_blank)
    out_blank = blank;
  if (ok)
    ok = vil_nitf2_double_formatter(3 + sec_precision, sec_precision, false)
           .read_vcl_stream(input, lat_seconds, out_blank);
  if (out_blank)
    out_blank = blank;
  if (ok)
    ok = vil_nitf2_char_formatter().read_vcl_stream(input, lat_hemisphere, out_blank);
  if (out_blank)
    out_blank = blank;
  // Read longitude fields (degrees is one digit longer than latitude)
  if (ok)
    ok = vil_nitf2_integer_formatter(3).read_vcl_stream(input, lon_degrees, out_blank);
  if (out_blank)
    out_blank = blank;
  if (ok)
    ok = vil_nitf2_integer_formatter(2).read_vcl_stream(input, lon_minutes, out_blank);
  if (out_blank)
    out_blank = blank;
  if (ok)
    ok = vil_nitf2_double_formatter(3 + sec_precision, sec_precision, false)
           .read_vcl_stream(input, lon_seconds, out_blank);
  if (out_blank)
    out_blank = blank;
  if (ok)
    ok = vil_nitf2_char_formatter().read_vcl_stream(input, lon_hemisphere, out_blank);
  if (out_blank)
    out_blank = blank;
  return ok && is_valid();
}

bool
vil_nitf2_location_dmsh::write(std::ostream & output, int /* field_width */)
{
  bool ok;
  // Write latitude fields
  ok = vil_nitf2_integer_formatter(2).write_vcl_stream(output, lat_degrees);
  ok &= vil_nitf2_integer_formatter(2).write_vcl_stream(output, lat_minutes);
  ok &= vil_nitf2_double_formatter(3 + sec_precision, sec_precision, false).write_vcl_stream(output, lat_seconds);
  ok &= vil_nitf2_char_formatter().write_vcl_stream(output, lat_hemisphere);
  // Write longitude fields (degrees is one digit longer than latitude)
  ok &= vil_nitf2_integer_formatter(3).write_vcl_stream(output, lon_degrees);
  ok &= vil_nitf2_integer_formatter(2).write_vcl_stream(output, lon_minutes);
  ok &= vil_nitf2_double_formatter(3 + sec_precision, sec_precision, false).write_vcl_stream(output, lon_seconds);
  ok &= vil_nitf2_char_formatter().write_vcl_stream(output, lon_hemisphere);
  return ok;
}

bool
vil_nitf2_location_dmsh::is_valid() const
{
  return lat_degrees >= -90 && lat_degrees <= 90 && lon_degrees >= -180 && lon_degrees <= 180 && lat_minutes >= 0 &&
         lat_minutes < 60 && lon_minutes >= 0 && lon_minutes < 60 && lat_seconds >= 0.0 && lat_seconds < 60.0 &&
         lon_seconds >= 0.0 && lon_seconds < 60.0 && std::string("NnSs").find(lat_hemisphere) != std::string::npos &&
         std::string("EeWw").find(lon_hemisphere) != std::string::npos;
}
