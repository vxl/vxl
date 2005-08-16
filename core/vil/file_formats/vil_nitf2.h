// vil_nitf2: Written by Rob Radtke (rob@) and Harry Voorhees (hlv@) of
// Stellar Science Ltd. Co. (stellarscience.com) for 
// Air Force Research Laboratory, 2005.

#ifndef VIL_NITF2_STREAM_H
#define VIL_NITF2_STREAM_H

#include <vil/vil_stream.h>

// These typedefs serve to maintain the method signatures' distinction between 
// input and output streams, at least until we decide that such distinction is 
// no longer useful.
typedef vil_stream vil_nitf2_istream;
typedef vil_stream vil_nitf2_ostream;

// Wrapper class for shared enums and static variables, to avoid circular 
// dependencies among classes
//
class vil_nitf2 
{
public:
  // NITF field data types supported
  enum enum_field_type { type_undefined=0, 
                         type_int, type_long_long, type_double, 
                         type_char, type_string, type_binary,
                         type_location, type_date_time, type_tre };

  // Controls the level of detail of logging to vcl_cout. 
  // All errors are logged to vcl_cerr, irrespective of log level.
  enum enum_log_level { log_none=0, log_info, log_debug };

  // Logging level for all vil_nitf classes. This could be generalized to an
  // array, if different subsets of classes want their own logging levels.
  static enum_log_level s_log_level;
};

#define VIL_NITF2_LOG(LEVEL) \
  if (vil_nitf2::s_log_level >= vil_nitf2::LEVEL) vcl_cout

#endif // VIL_NITF2_STREAM_H