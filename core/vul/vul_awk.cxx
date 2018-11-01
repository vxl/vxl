// This is core/vul/vul_awk.cxx
//:
// \file
// \author Andrew W. Fitzgibbon, Oxford RRG
// \date   17 May 1997
// \verbatim
//  Modifications
//   AWF (Oxford)     17 May 1997: Initial version.
//   Eric Moyer       15 Jul 2009: Added strip comment functionality
// \endverbatim
//
//-----------------------------------------------------------------------------

#include <cctype>
#include <cstring>
#include <iostream>
#include <cstdio>
#include "vul_awk.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Construct from input stream
vul_awk::vul_awk(std::istream& s, ModeFlags mode):
  fd_(s),
  mode_(mode)
{
  done_ = false;
  line_number_ = 0;
  split_line_ = nullptr;

  next();
}

vul_awk::~vul_awk()
{
  delete [] split_line_;
}

void vul_awk::next()
{
  bool do_strip_comments = ( ((int)mode_) & ((int)strip_comments) ) != 0;
#if 0
  bool do_backslash_continuations = (int(mode_) & int(backslash_continuations)) != 0;
#endif
  bool discard_current_line = true;
  while (discard_current_line)
  {
    bool extract_fields = true;
    discard_current_line = false;

    // Read line -- should be quite fast after the first one.
    line_.erase();

    while (true) {
      int c = fd_.get();
      if (c == EOF  ||  fd_.eof()) {
        done_ = true;
        break;
      }
      if (c == '\n')
        break;
      line_ += char(c);
    }

    char const* linep = line_.c_str();

    // copy string
    delete [] split_line_;
    split_line_ = new char[line_.size() + 1];
    std::strcpy(split_line_, linep);

    //strip comments
    if (do_strip_comments) {
      //find the first # character
      char* comment_char = split_line_;
      while (*comment_char != '#' && *comment_char != '\0') ++comment_char;
      //replace the # with a single space and terminate the string.  I
      //use a single space since that will help the backslash
      //continuation if it is ever implemented
      if (*comment_char == '#') {
        //Replace with a space
        *comment_char = ' '; ++comment_char;
        //Terminate the string
        if (*comment_char != '\0') { *comment_char = '\0'; }
        if (comment_char - split_line_ == 1) {
          //The line was only a comment -- don't try to extract
          //records, just discard the current line and go to the next
          extract_fields = false;
          discard_current_line = true;
        }
      }
    }

    if (extract_fields) {
      // Chop line up into fields
      fields_.clear();
      char* cp = split_line_;

      while (true) {
        // Eat white
        while (*cp && std::isspace(*cp))
          ++cp;
        if (!*cp) break;

        // Push
        fields_.push_back(cp);

        // Find nonwhite
        while (*cp && !std::isspace(*cp))
          ++cp;
        if (!*cp) break;

        // Zap space
        *cp++ = '\0';
      }
    }
    // Increment line number
    ++line_number_;
  }
}

char const* vul_awk::line_from(int field_number) const
{
  char const *p = line_.c_str();
  if (field_number >= NF())
    field_number = NF() - 1;
  if (field_number < 0) {
    std::cerr << "vul_awk::line_from("<< field_number <<") -- ZOIKS\n";
    return line();
  }

  return p + (fields_[field_number] - split_line_);
}

void testvul_awk()
{
  std::cout << "Start\n";
  for (vul_awk awk(std::cin); awk; ++awk) {
    std::cout << awk.NF() << ':' << awk[2] << std::endl;
  }
}
