// This is oxl/oxp/oxp_parse_seqname.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

#include "oxp_parse_seqname.h"

#include <vcl_cstring.h>
#include <vcl_cstdlib.h> // atoi()
#include <vcl_iostream.h>

#include <vul/vul_reg_exp.h>
#include <vul/vul_printf.h>

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
  start_ = -1;
  step_ = -1;
  end_ = -1;

  // First match any trailing ",n[:n]:[n]" (can use ; or ,)
  filename_ = s;
  vul_reg_exp re("[,;]([0-9]+)(:[0-9]+)?:([0-9]+)?$");
  if (re.find(s)) {
    vcl_string match_start = re.match(1);
    vcl_string match_step = re.match(2);
    vcl_string match_end = re.match(3);

    int last = re.start(0);
    filename_ = filename_.substr(0,last);

    vul_printf(vcl_cerr, "oxp_parse_seqname: %s [%s:%s:%s]  -> ",
           filename_.c_str(),
           match_start.c_str(), match_step.c_str()+1, match_end.c_str());


    if (match_start.length() > 0)
      start_ = vcl_atoi(match_start.c_str());

    if (match_step.length() > 0)
      step_ = vcl_atoi(match_step.c_str()+1);

    if (match_end.length() > 0)
      end_ = vcl_atoi(match_end.c_str());

    vul_printf(vcl_cerr, "[%d:%d:%d]\n", start_, step_, end_);

  } else if (vcl_strchr(s, ',')) {
    vul_printf(vcl_cerr, "oxp_parse_seqname: Warning: \"%s\" contains a comma, but didn't match my regexp.\n", s);
  }
}
