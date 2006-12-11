// This is core/vul/vul_get_timestamp.h
#ifndef vul_get_timestamp_h_
#define vul_get_timestamp_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif

//:
// \file
// \brief Obtains time elapsed since 1 Jan 1970, in seconds and milliseconds
// \author fsm
//
// \verbatim
// Modifications
// PDA (Manchester) 21/03/2001: Tidied up the documentation
// \endverbatim


#include <vcl_string.h>


//: purpose: obtain time elapsed since 1 Jan 1970, in seconds and milliseconds.
void vul_get_timestamp(int &secs, int &msecs);


//: Enum to specify style option for vul_get_time_as_string()
enum vul_time_style
{
  vul_asc,         //!< format used by std:asctime() e.g. "Fri Dec 8 14:54:17 2006"
  vul_numeric_msf  //!< space-separated numbers, most significant first "e.g. 2006 12 08 14 54 17"
};

//: Get the present time and date as a string, e.g. "Fri Dec 8 14:54:17 2006"
vcl_string vul_get_time_as_string(vul_time_style style=vul_asc);


#endif // vul_get_timestamp_h_
