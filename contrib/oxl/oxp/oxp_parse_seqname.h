//-*- c++ -*-------------------------------------------------------------------
#ifndef oxp_parse_seqname_h_
#define oxp_parse_seqname_h_
#ifdef __GNUC__
#pragma interface
#endif
// Author: awf@robots.ox.ac.uk
// Created: 22 Jul 00

#include <vcl/vcl_string.h>

//: Match optional ,1:2:40 at end of filename and strip it.
// Fields are -1 if not matched.
class oxp_parse_seqname {
public:
  vcl_string filename;
  int start;
  int step;
  int end;
  
  oxp_parse_seqname(char const* s);
  oxp_parse_seqname(vcl_string const& s);
  
  void parse(char const* s);
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS oxp_parse_seqname.

