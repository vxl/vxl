// This is core/vul/vul_awk.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
//
// \author Andrew W. Fitzgibbon, Oxford RRG
// \date   17 May 97
//
//-----------------------------------------------------------------------------

#include "vul_awk.h"

#include <vcl_cctype.h>
#include <vcl_cstring.h>
#include <vcl_iostream.h>

//: Construct from input stream
vul_awk::vul_awk(vcl_istream& s, ModeFlags mode):
  fd_(s),
  mode_(mode)
{
  done_ = false;
  line_number_ = 0;
  split_line_ = 0;

  next();
}

vul_awk::~vul_awk()
{
  delete [] split_line_;
}

void vul_awk::next()
{
  // Read line -- should be quite fast after the first one.
  line_.erase();
  //bool do_backslash_continuations = (int(mode_) & int(backslash_continuations)) != 0;
  //  bool do_strip_comments = (int(mode_) & int(strip_comments)) != 0;

  while (true) {
    int c = fd_.get();
    if (c == EOF  ||  fd_.eof()) {
      done_ = true;
      break;
    }
    if (c == '\n')
      break;
    line_ += c;
  }

  char const* linep = line_.c_str();

  // copy string
  delete [] split_line_;
  split_line_ = new char[line_.size() + 1];
  vcl_strcpy(split_line_, linep);

  // Chop line up into fields
  fields_.clear();
  char* cp = split_line_;

  while (true) {
    // Eat white
    while (*cp && vcl_isspace(*cp))
      ++cp;
    if (!*cp) break;

    // Push
    fields_.push_back(cp);

    // Find nonwhite
    while (*cp && !vcl_isspace(*cp))
      ++cp;
    if (!*cp) break;

    // Zap space
    *cp++ = '\0';
  }

  // Increment line number
  ++line_number_;
}

char const* vul_awk::line_from(int field_number) const
{
  char const *p = line_.c_str();
  if (field_number >= NF())
    field_number = NF() - 1;
  if (field_number < 0) {
    vcl_cerr << "vul_awk::line_from("<< field_number <<") -- ZOIKS\n";
    return line();
  }

  return p + (fields_[field_number] - split_line_);
}

void testvul_awk()
{
  vcl_cout << "Start\n";
  for (vul_awk awk(vcl_cin); awk; ++awk) {
    vcl_cout << awk.NF() << ':' << awk[2] << vcl_endl;
  }
}
