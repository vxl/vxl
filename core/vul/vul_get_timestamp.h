// This is vxl/vul/vul_get_timestamp.h
#ifndef vul_get_timestamp_h_
#define vul_get_timestamp_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Obtains time elapsed since 1 Jan 1970, in seconds and milliseconds
// \author fsm@robots.ox.ac.uk
//
// \verbatim
// Modifications
// PDA (Manchester) 21/03/2001: Tidied up the documentation
// \endverbatim

//: purpose: obtain time elapsed since 1 Jan 1970, in seconds and milliseconds.
void vul_get_timestamp(int &secs, int &msecs);

#endif // vul_get_timestamp_h_
