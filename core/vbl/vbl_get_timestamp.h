#ifndef vbl_get_timestamp_h_
#define vbl_get_timestamp_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// .NAME vbl_get_timestamp
// .HEADER vxl package
// .LIBRARY vbl
// .INCLUDE vbl/vbl_get_timestamp.h
// .FILE vbl_get_timestamp.cxx
// .SECTION Author
//   fsm@robots.ox.ac.uk
//

//: purpose: obtain time elapsed since 1 Jan 1970, in seconds and milliseconds.
void vbl_get_timestamp(int &secs, int &msecs);

#endif // vbl_get_timestamp_h_
