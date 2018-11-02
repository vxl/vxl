// This is core/vul/vul_reg_exp.h
#ifndef vul_reg_exph
#define vul_reg_exph
//:
// \file
// \brief contains class for pattern matching with regular expressions
// \author Texas Instruments Incorporated.
//
// \verbatim
// Modifications
// PDA (Manchester) 21/03/2001: Tidied up the documentation
// Peter Vanroose   27/05/2001: Corrected the documentation
// Peter Vanroose   07/02/2002: brief doxygen comment placed on single line
// Peter Vanroose   13/06/2002: bug fix: crash in match() when startp==endp==0
// Ian Scott        08/06/2003: Add protect(char) function
// \endverbatim
//
// Original Copyright notice:
// Copyright (C) 1991 Texas Instruments Incorporated.
//
// Permission is granted to any individual or institution to use, copy, modify,
// and distribute this software, provided that this complete copyright and
// permission notice is maintained, intact, in all copies and supporting
// documentation.
//
// Texas Instruments Incorporated provides this software "as is" without
// express or implied warranty.

#include <string>
#include <cstddef>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

constexpr int vul_reg_exp_nsubexp = 10;

//: Pattern matching with regular expressions.
//  A regular expression allows a programmer to specify complex
//  patterns that can be searched for and matched against the
//  character string of a string object. In its simplest form, a
//  regular expression is a sequence of characters used to search for
//  exact character matches. However, many times the exact sequence to
//  be found is not known, or only a match at the beginning or end of
//  a string is desired. This regular expression class implements
//  regular expression pattern matching as is found and implemented in
//  many UNIX commands and utilities.
//
//  Example: The perl code
// \code
//     $filename =~ m"([a-z]+)\.cc";
//     print $1;
// \endcode
//  is written as follows in C++
// \code
//     vul_reg_exp re("([a-z]+)\\.cc");
//     re.find(filename);
//     std::cout << re.match(1);
// \endcode
//
//  The regular expression class provides a convenient mechanism for
//  specifying and manipulating regular expressions. The regular
//  expression object allows specification of such patterns by using
//  the following regular expression metacharacters:
//
// -  ^        Matches at beginning of a line
// -  $        Matches at end of a line
// - .         Matches any single character
// - [ ]       Matches any character(s) inside the brackets
// - [^ ]      Matches any character(s) not inside the brackets
// - [ - ]     Matches any character in range on either side of a dash
// -  *        Matches preceding pattern zero or more times
// -  +        Matches preceding pattern one or more times
// -  ?        Matches preceding pattern at most once
// - ()        Saves a matched expression and uses it in a later match
//
//  Note that more than one of these metacharacters can be used in a
//  single regular expression in order to create complex search
//  patterns. For example, the pattern [^ab1-9] says to match any
//  character sequence that does not begin with the characters "a",
//  "b", or the characters "1" through "9".
//
class vul_reg_exp
{
  //: anchor point of start position for n-th matching regular expression
  const char* startp[vul_reg_exp_nsubexp];
  //: anchor point of end position for n-th matching regular expression
  const char* endp[vul_reg_exp_nsubexp];
  //: Internal use only
  char  regstart;
  //: Internal use only
  char  reganch;
  //: Internal use only
  const char* regmust;
  //: Internal use only
  int   regmlen;
  char* program;
  int   progsize;
  const char* searchstring;
 public:
  //: Creates an empty regular expression.
  inline vul_reg_exp() : program(nullptr) { clear_bufs(); }
  //: Creates a regular expression from string s, and compiles s.
  inline vul_reg_exp(char const* s) : program(nullptr) { clear_bufs(); compile(s); }
  //: Copy constructor
  vul_reg_exp(vul_reg_exp const&);
  //: Frees space allocated for regular expression.
  inline ~vul_reg_exp() { delete[] this->program; }
  //: Compiles char* --> regexp
  void compile(char const*);
  //: true if regexp in char* arg
  bool find(char const*);
  //: true if regexp in char* arg
  bool find(std::string const&);
  //: Returns the start index of the last item found.
  inline std::ptrdiff_t start() const { return this->startp[0] - searchstring; }
  //: Returns the end index of the last item found.
  inline std::ptrdiff_t end()   const { return this->endp[0] - searchstring; }
  //: Equality operator
  bool operator==(vul_reg_exp const&) const;
  //: Inequality operator
  inline bool operator!=(vul_reg_exp const& r) const { return !operator==(r); }
  //: Same regexp and state?
  bool deep_equal(vul_reg_exp const&) const;
  //: Returns true if a valid RE is compiled and ready for pattern matching.
  inline bool is_valid() const { return this->program != nullptr; }
  //: Invalidates regular expression.
  inline void set_invalid() { delete[] this->program; this->program = nullptr; clear_bufs(); }

  //: Return start index of nth submatch.
  // start(0) is the start of the full match.
  inline std::ptrdiff_t start(long n) const { return this->startp[n] - searchstring; }
  //: Return end index of nth submatch.
  // end(0) is the end of the full match.
  inline std::ptrdiff_t end(long n)   const { return this->endp[n] - searchstring; }
  //: Return nth submatch as a string.
  std::string match(int n) const {
    return this->endp[n] == this->startp[n] ? std::string("") :
           std::string(this->startp[n], this->endp[n] - this->startp[n]);
  }
  //: Return an expression that will match precisely c
  // The returned string is owned by the function, and
  // will be overwritten in subsequent calls.
  static const char * protect(char c);

 private:
  //: private function to clear startp[] and endp[]
  void clear_bufs() { for (int n=0; n<vul_reg_exp_nsubexp; ++n) startp[n]=endp[n]=nullptr; }
};

#endif // vul_reg_exph
