#ifndef vbl_get_timestamp_h_
#define vbl_get_timestamp_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  fsm@robots.ox.ac.uk
*/

//: purpose: obtain time elapsed since 1 Jan 1970, in seconds and milliseconds.
void vbl_get_timestamp(int &secs, int &msecs);

#endif
