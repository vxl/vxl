// This is vxl/vbl/vbl_reg_exp.h
#ifndef vbl_reg_exph
#define vbl_reg_exph

//:
// \file
// \brief contains class for pattern matching with regular expressions
// \author Texas Instruments Incorporated.
//
// \verbatim
// Modifications
// PDA (Manchester) 21/03/2001: Tidied up the documentation
// Peter Vanroose   27/05/2001: Corrected the documentation
// \endverbatim

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

#include <vcl_string.h>

const int NSUBEXP = 10;

//: Pattern matching with regular expressions.
//  A regular expression allows a programmer to specify  complex
//  patterns  that  can  be searched for and matched against the
//  character string of a string object. In its simplest form, a
//  regular  expression  is  a  sequence  of  characters used to
//  search for exact character matches. However, many times  the
//  exact  sequence to be found is not known, or only a match at
//  the beginning or end of a string is desired. The vbl  regu-
//  lar  expression  class implements regular expression pattern
//  matching as is found and implemented in many  UNIX  commands
//  and utilities.
//
//  Example: The perl code
// \verbatim
//     $filename =~ m"([a-z]+)\.cc";
//     print $1;
// \endverbatim
//  is written as follows in C++
// \verbatim
//     vbl_reg_exp re("([a-z]+)\\.cc");
//     re.find(filename);
//     cerr << re.match(1);
// \endverbatim
//
//  The regular expression class provides a convenient mechanism
//  for  specifying  and  manipulating  regular expressions. The
//  regular expression object allows specification of such  pat-
//  terns  by using the following regular expression metacharac-
//  ters:
//
// \verbatim
//   ^        Matches at beginning of a line
//   $        Matches at end of a line
//  .         Matches any single character
//  [ ]       Matches any character(s) inside the brackets
//  [^ ]      Matches any character(s) not inside the brackets
//   -        Matches any character in range on either side of a dash
//   *        Matches preceding pattern zero or more times
//   +        Matches preceding pattern one or more times
//   ?        Matches preceding pattern zero or once only
//  ()        Saves a matched expression and uses it in a  later match
// \endverbatim
//
//  Note that more than one of these metacharacters can be  used
//  in  a  single  regular expression in order to create complex
//  search patterns. For example, the pattern [^ab1-9]  says  to
//  match  any  character  sequence that does not begin with the
//  characters "ab"  followed  by  numbers  in  the  series  one
//  through nine.
//
class vbl_reg_exp {
public:
    //: Creates an empty regular expression.
  inline vbl_reg_exp () { this->program = NULL; } 
    //: Creates a regular expression from string s, and compiles s.
  inline vbl_reg_exp (char const* s) { this->program = NULL; compile(s); }
    //: Copy constructor
  vbl_reg_exp (vbl_reg_exp const&);
    //: Frees space allocated for regular expression.
  inline ~vbl_reg_exp() { delete [] this->program; }
    //: Compiles char* --> regexp
  void compile (char const*);
    //: true if regexp in char* arg
  bool find (char const*);
    //: true if regexp in char* arg
  bool find (vcl_string const&);
    //: Returns the start index of the last item found.
  inline long start() const { return(this->startp[0] - searchstring); }
    //: Returns the end index of the last item found.
  inline long end()   const { return(this->endp[0] - searchstring); }
    //: Equality operator
  bool operator== (vbl_reg_exp const&) const;
    //: Inequality operator
  inline bool operator!= (vbl_reg_exp const& r) const { return !(*this == r); }
    //: Same regexp and state?
  bool deep_equal (vbl_reg_exp const&) const;
    //: Returns true if a valid RE is compiled and ready for pattern matching.
  inline bool is_valid() const { return this->program != NULL; }
    //: Invalidates regular expression.
  inline void set_invalid() { delete [] this->program; this->program = NULL; }

    //: Return start index of nth submatch.
    // start(0) is the start of the full match.
  int start(int n) const { return this->startp[n] - searchstring; }
    //: Return end index of nth submatch
    // end(0) is the end of the full match.
  int end(int n)   const { return this->endp[n] - searchstring; }
    //: Return nth submatch as a string.
  vcl_string match(int n) const { return vcl_string(this->startp[n], this->endp[n] - this->startp[n]); }

private:
  const char* startp[NSUBEXP];
  const char* endp[NSUBEXP];
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
};

#endif // vbl_reg_exph
