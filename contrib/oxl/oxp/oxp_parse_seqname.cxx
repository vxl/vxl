//-*- c++ -*-------------------------------------------------------------------
#ifdef __GNUC__
#pragma implementation "oxp_parse_seqname.h"
#endif
//
// Class: oxp_parse_seqname
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 22 Jul 00
// Modifications:
//   000722 AWF Initial version.
//
//-----------------------------------------------------------------------------

#include "oxp_parse_seqname.h"

#include <vcl/vcl_cstdlib.h> // atoi()
#include <vcl/vcl_iostream.h>

#include <vbl/vbl_reg_exp.h>
#include <vbl/vbl_printf.h>

// Default ctor
oxp_parse_seqname::oxp_parse_seqname(char const* s)
{
  parse(s);
}

// Copy ctor
oxp_parse_seqname::oxp_parse_seqname(vcl_string const& s)
{
  parse(s.c_str());
}

// Assignment
void oxp_parse_seqname::parse(char const* s)
{
  start = -1;
  step = -1;
  end = -1;

  // First match any trailing ",n[:n]:[n]" (can use ; or ,)
  filename = s;
  vbl_reg_exp re("[,;]([0-9]+)(:[0-9]+)?:([0-9]+)?$");
  if (re.find(s)) {
    vcl_string match_start = re.match(1);
    vcl_string match_step = re.match(2);
    vcl_string match_end = re.match(3);
    
    int last = re.start(0);
    filename = filename.substr(0,last);

    vbl_printf(cerr, "oxp_parse_seqname: %s [%s:%s:%s]  -> ",
	   filename.c_str(),
	   match_start.c_str(), match_step.c_str()+1, match_end.c_str());


    if (match_start.length() > 0)
      start = atoi(match_start.c_str());

    if (match_step.length() > 0)
      step = atoi(match_step.c_str()+1);
    
    if (match_end.length() > 0)
      end = atoi(match_end.c_str());

    vbl_printf(cerr, "[%d:%d:%d]\n", start, step, end);
    
  } else if (strchr(s, ',')) {
    vbl_printf(cerr, "oxp_parse_seqname: Warning: \"%s\" contains a comma, but didn't match my regexp.\n", s);
  }
  
}
