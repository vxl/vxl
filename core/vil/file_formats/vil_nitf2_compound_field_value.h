// vil_nitf2: Written by Harry Voorhees (hlv@) and Rob Radtke (rob@) of
// Stellar Science Ltd. Co. (stellarscience.com) for
// Air Force Research Laboratory, 2005.

#ifndef VIL_NITF2_COMPOUND_FIELD_VALUE_H
#define VIL_NITF2_COMPOUND_FIELD_VALUE_H

#include <iostream>
#include <string>
#include <istream>
#include <ostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//-----------------------------------------------------------------------------
// NITF compound field types
//
// NITF fields can represent built-in types like integer or std::string, and
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
  virtual std::ostream& output(std::ostream&) const = 0;

  // Returns true iff all members lie within their expected ranges
  virtual bool is_valid() const = 0;

  virtual ~vil_nitf2_compound_field_value() = default;
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
  vil_nitf2_date_time(std::string format);
  ~vil_nitf2_date_time() override = default;
  bool is_valid() const override;
  bool read(std::istream& input, int field_width, bool& out_blank);
  bool write(std::ostream& output, int field_width) const;
  std::ostream& output(std::ostream& os) const override;
};

std::ostream& operator << (std::ostream& os, const vil_nitf2_date_time& dateTime);

// Base class for geodetic location field type

class vil_nitf2_location : public vil_nitf2_compound_field_value
{
  friend class vil_nitf2_location_formatter;
public:
  enum format_type { format_degrees, format_dmsh };
  format_type format;
  vil_nitf2_location(format_type format) : format(format) {}
  ~vil_nitf2_location() override = default;
  virtual bool read(std::istream& input, int field_width, bool& out_blank) = 0;
  virtual bool write(std::ostream& output, int field_width) = 0;
};

// Geodetic location represented as a pair of signed degrees, with a
// specified precision.

struct vil_nitf2_location_degrees : public vil_nitf2_location
{
public:
  vil_nitf2_location_degrees(int precision)
    : vil_nitf2_location(format_degrees), precision(precision) {}
  bool read(std::istream& input, int field_width, bool& out_blank) override;
  bool write(std::ostream& output, int field_width) override;
  double lat_degrees;
  double lon_degrees;
  int precision;
  std::ostream& output(std::ostream&) const override;
  bool is_valid() const override;
};

std::ostream& operator << (std::ostream& os, const vil_nitf2_location& loc);

// Geodetic location represented as unsigned degrees, minutes, seconds, and
// hemisphere, to a specified precision. (Currently this class assumes that
// the coarsest precision is integer seconds, although that restriction
// could easily be lifted.)

struct vil_nitf2_location_dmsh : public vil_nitf2_location
{
public:
  vil_nitf2_location_dmsh(int sec_precision)
    : vil_nitf2_location(format_dmsh), sec_precision(sec_precision) {}
  bool read(std::istream& input, int field_width, bool& out_blank) override;
  bool write(std::ostream& output, int field_width) override;
  int lat_degrees; int lat_minutes; double lat_seconds; char lat_hemisphere;
  int lon_degrees; int lon_minutes; double lon_seconds; char lon_hemisphere;
  int sec_precision; // second's significant decimal places
  std::ostream& output(std::ostream&) const override;
  bool is_valid() const override;
};

std::ostream& operator << (std::ostream& os, const vil_nitf2_location_dmsh& loc);

#endif // VIL_NITF2_COMPOUND_FIELD_VALUE_H
