// This is core/vul/vul_awk.h
#ifndef vul_awk_h_
#define vul_awk_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Andrew W. Fitzgibbon, Oxford RRG
// \date   17 May 97
//
// \verbatim
// Modifications
// 970517 AWF Initial version.
// PDA (Manchester) 21/03/2001: Tidied up the documentation
// Peter Vanroose   27/05/2001: Corrected the documentation
// \endverbatim


#include <vcl_string.h>
#include <vcl_iosfwd.h>
#include <vcl_vector.h>

//: The core of awk
// vul_awk reads lines from a vcl_istream and breaks them into whitespace-separated
// fields.  Its primary advantage is that its name defines the semantics of
// its methods---except that this C++ version uses zero-based fields.  The
// usage is exemplified in this example, to print the second field in every
// line:
// \code
//    for (vul_awk awk=cin; awk; ++awk)
//      vcl_cout << awk[2] << vcl_endl;
// \endcode

class vul_awk
{
  //: Helper types for safe boolean conversion.
  struct safe_bool_dummy { void dummy() {} };
  typedef void (safe_bool_dummy::* safe_bool)();
 public:
  // Constructors/Destructors--------------------------------------------------
  enum ModeFlags {
    none = 0x00,
    verbose = 0x01,
    strip_comments = 0x02,
    backslash_continuations = 0x04
  };

  vul_awk(vcl_istream& s, ModeFlags mode = none);
  ~vul_awk();

  // Operations----------------------------------------------------------------

//: Return field i.  Counting starts at 0.
  char const* operator[] (unsigned i) const {
    if (i < fields_.size())
      return fields_[i];
    else
      return 0;
  }

//: Return the current "record number", i.e. line number
  int NR() const { return line_number_; }

//: Return the number of fields on this line.
  int NF() const { return fields_.size(); }

//: Return the entire line
  char const* line() const { return (char const*)line_.c_str(); }

//: Return the remainder of the line, starting from field_number.
// (0 is from the first non-whitespace character)
  char const* line_from(int field_number) const;

//: Return true if this line is not the last.
  operator safe_bool () const
    { return (!done_)? &safe_bool_dummy::dummy : 0; }

//: Advance to the next line
  vul_awk& operator ++ () { next(); return *this; }

//: Display error message, line number.
// Also display optional field number and  char within field.

  void error(vcl_ostream&, char const* message, int field = -1,
             int char_within_field = 0);

 protected:
  // Data Members--------------------------------------------------------------
  vcl_istream& fd_;

  ModeFlags mode_;

  // The last input line.
  vcl_string line_;

  // Copy of last line with null characters at the start of every field
  char* split_line_;
  // Pointers to the fields within split_line_;
  vcl_vector<char *> fields_;

  // May as well keep track of it...
  int line_number_;

  // Set to true when the current line is the last one.
  bool done_;

  void next();

  vul_awk(const vul_awk& that);
  vul_awk& operator=(const vul_awk& that);
};

#endif // vul_awk_h_
