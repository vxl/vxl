// This is core/vul/vul_awk.h
#ifndef vul_awk_h_
#define vul_awk_h_
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
// Eric Moyer       15/07/2009: Modified the documentation to reflect
//                              working strip_comments and non-working
//                              backslash_continuation
// \endverbatim


#include <string>
#include <iosfwd>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: The core of awk
// vul_awk reads lines from a std::istream and breaks them into whitespace-separated
// fields.  Its primary advantage is that its name defines the semantics of
// its methods---except that this C++ version uses zero-based fields.  The
// usage is exemplified in this example, to print the second field in every
// line:
// \code
//    for (vul_awk awk=cin; awk; ++awk)
//      std::cout << awk[2] << std::endl;
// \endcode
//
// The constructor takes an integer mode-flag variable.  Right now,
// only the strip_comments flag has any effect, though the ModeFlags
// enumeration contains other potential flags.
//
// When the strip_comments flag is set then everything from the first
// '#' character to the end of the line is replaced with a single
// space.  As a special feature, lines that contain an # as the first
// character are skipped entirely by the next() routine, no attempt
// will be made to extract fields from them.  They will be counted in
// the line numbering so that error messages can easily refer to the
// correct line in the file.  To extend the above example to handle
// comments in the file, write:
// \code
//    for (vul_awk awk(cin, vul_awk::strip_comments); awk; ++awk)
//      std::cout << awk[2] << std::endl;
// \endcode
//

class vul_awk
{

 public:
  // Constructors/Destructors--------------------------------------------------
  enum ModeFlags {
    none = 0x00,
    verbose = 0x01,
    strip_comments = 0x02,
    backslash_continuations = 0x04
  };

  vul_awk(std::istream& s, ModeFlags mode = none);
  ~vul_awk();

  // Operations----------------------------------------------------------------

//: Return field i.  Counting starts at 0.
  char const* operator[] (unsigned i) const {
    if (i < fields_.size())
      return fields_[i];
    else
      return nullptr;
  }

//: Return the current "record number", i.e. line number
  int NR() const { return line_number_; }

//: Return the number of fields on this line.
  int NF() const { return int(fields_.size()); }

//: Return the entire line
  char const* line() const { return (char const*)line_.c_str(); }

//: Return the remainder of the line, starting from field_number.
// (0 is from the first non-whitespace character)
  char const* line_from(int field_number) const;

//: Return true if this line is not the last.
  explicit operator bool () const
    { return (!done_)? true : false; }

//: Return false if this line is not the last.
  bool operator!() const
    { return done_; }

//: Advance to the next line
  vul_awk& operator ++ () { next(); return *this; }

//: Display error message, line number.
// Also display optional field number and  char within field.

  void error(std::ostream&, char const* message, int field = -1,
             int char_within_field = 0);

 protected:
  // Data Members--------------------------------------------------------------
  std::istream& fd_;

  ModeFlags mode_;

  // The last input line.
  std::string line_;

  // Copy of last line with null characters at the start of every field
  char* split_line_;
  // Pointers to the fields within split_line_;
  std::vector<char *> fields_;

  // May as well keep track of it...
  int line_number_;

  // Set to true when the current line is the last one.
  bool done_;

  void next();

  vul_awk(const vul_awk& that);
  vul_awk& operator=(const vul_awk& that);
};

#endif // vul_awk_h_
