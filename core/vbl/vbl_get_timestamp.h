#ifndef vbl_get_timestamp_h_
#define vbl_get_timestamp_h_
#ifdef __GNUC__
#pragma interface
#endif
// This is vxl/vbl/vbl_get_timestamp.h

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
void vbl_get_timestamp(int &secs, int &msecs);

#endif // vbl_get_timestamp_h_
