// vil_nitf2: Written by Harry Voorhees (hlv@) and Rob Radtke (rob@) of
// Stellar Science Ltd. Co. (stellarscience.com) for 
// Air Force Research Laboratory, 2005.

#ifndef VIL_NITF2_COMPOUND_FIELD_VALUE_H
#define VIL_NITF2_COMPOUND_FIELD_VALUE_H

#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_istream.h>
#include <vcl_ostream.h>

//-----------------------------------------------------------------------------
// NITF compound field types
//
// NITF fields can represent built-in types like integer or vcl_string, and
// compound types like date_time. Subclasses of vil_nitf2_compound_field_value, along with
// built-in types, represent all the types of values that are passed back to 
// clients. These types implement operator << (ostream&) to output themselves 
// in human-readable form. Some compound fields keep track of their precision.
// By contrast, NITF file i/o is handled by corresponding Formatter classes, 
// which clients should not need to know about.

// Base class for NITF compound field types.
// Each class is expected to define operator << which should call output().
//
class vil_nitf2_compound_field_value
{
public:
  // To be called by operator << to pretty-print field to stream
  virtual vcl_ostream& output(vcl_ostream&) const = 0;

  // Returns true iff all members lie within their expected ranges
  virtual bool is_valid() const = 0;
};

// A date and time, down to the second or decimal fraction thereof.
//
class vil_nitf2_date_time : public vil_nitf2_compound_field_value
{
  friend class vil_nitf2_date_time_formatter;
public:
  int year;
  int month;        // 1-12
  int day;          // 1-31
  int hour;         // 0-23
  int minute;       // 0-59
  double second;    // 0-59.999...
  int sec_precision; // second's significant decimal places
  vil_nitf2_date_time() : year(0), month(0), day(0), hour(0), minute(0), second(0), sec_precision(0) {}
  vil_nitf2_date_time(vcl_string format);
  virtual ~vil_nitf2_date_time() {}
  bool is_valid() const;
  bool read(vcl_istream& input, int field_width, bool& out_blank);
  bool write(vcl_ostream& output, int field_width) const;
  vcl_ostream& output(vcl_ostream& os) const;
};

vcl_ostream& operator << (vcl_ostream& os, const vil_nitf2_date_time& dateTime);

// Base class for geodetic location field type

class vil_nitf2_location : public vil_nitf2_compound_field_value
{
  friend class vil_nitf2_location_formatter;
public:
  enum format_type { format_degrees, format_dmsh };
  format_type format;
  vil_nitf2_location(format_type format) : format(format) {}
  virtual ~vil_nitf2_location() {}
  virtual bool read(vcl_istream& input, int field_width, bool& out_blank) = 0;
  virtual bool write(vcl_ostream& output, int field_width) = 0;
};

// Geodetic location represented as a pair of signed degrees, with a 
// specified precision.

struct vil_nitf2_location_degrees : public vil_nitf2_location
{
public:
  vil_nitf2_location_degrees(int precision) 
    : vil_nitf2_location(format_degrees), precision(precision) {}
  bool read(vcl_istream& input, int field_width, bool& out_blank);
  bool write(vcl_ostream& output, int field_width);
  double lat_degrees; 
  double lon_degrees;
  int precision;
  vcl_ostream& output(vcl_ostream&) const;
  bool is_valid() const;
};

vcl_ostream& operator << (vcl_ostream& os, const vil_nitf2_location& loc);

// Geodetic location represented as unsigned degrees, minutes, seconds, and
// hemisphere, to a specified precision. (Currently this class assumes that
// the coarsest precision is integer seconds, although that restriction
// could easily be lifted.)

struct vil_nitf2_location_dmsh : public vil_nitf2_location
{
public:
  vil_nitf2_location_dmsh(int sec_precision)
    : vil_nitf2_location(format_dmsh), sec_precision(sec_precision) {}
  bool read(vcl_istream& input, int field_width, bool& out_blank);
  bool write(vcl_ostream& output, int field_width);
  int lat_degrees; int lat_minutes; double lat_seconds; char lat_hemisphere;
  int lon_degrees; int lon_minutes; double lon_seconds; char lon_hemisphere;
  int sec_precision; // second's significant decimal places
  vcl_ostream& output(vcl_ostream&) const;
  bool is_valid() const;
};

vcl_ostream& operator << (vcl_ostream& os, const vil_nitf2_location_dmsh& loc);

#endif // VIL_NITF2_COMPOUND_FIELD_VALUE_H
