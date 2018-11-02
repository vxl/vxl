// This is core/vul/vul_string.cxx
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <algorithm>
#include <sstream>
#include <cmath>
#include "vul_string.h"
//:
// \file

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_reg_exp.h>
#include <vul/vul_sprintf.h>

#ifndef END_OF_STRING                           // If END_OF_STRING not defined
#define END_OF_STRING (0)
#endif

// Converts all alphabetical characters to uppercase.
char* vul_string_c_upcase(char* s)  // Convert entire string to upper case
{
  char* p = s;                   // Point to beginning of string
  while (*p) {                   // While there are still valid characters
    if (std::islower(*p))         // if this is lower case
      *p = (char)std::toupper(*p);// convert to uppercase
    p++;                         // Advance pointer
  }
  return s;                      // Return reference to modified string
}

// Converts all alphabetical characters to lowercase.
char* vul_string_c_downcase(char* s)  // Convert entire string to lower case
{
  char* p = s;                   // Point to beginning of string
  while (*p) {                   // While there are still valid characters
    if (std::isupper(*p))         // if this is upper case
      *p = (char)std::tolower(*p);// convert to lowercase
    p++;                         // Advance pointer
  }
  return s;                      // Return reference to modified string
}

// Capitalizes all words in a string. A word is defined as
// a sequence of characters separated by non-alphanumerics.
char* vul_string_c_capitalize(char* s)  // Capitalize each word in string
{
  char* p = s;                           // Point to beginning of string
  while (true) {                         // Infinite loop
    for (; *p && !std::isalnum(*p); p++) ;// Skip to first alphanumeric
    if (*p == END_OF_STRING)             // If end of string
      return s;                          // Return string
    *p = (char)std::toupper(*p);          // Convert character
    while (*++p && std::isalnum(*p)) ;    // Search for next word
  }
}

// Removes any occurrence of the string rem from string str,
// and returns the modified string str.
char* vul_string_c_trim(char* str, const char* rem) // Trim characters from string
{
  char* s = str;
  char* result = str;
   char c;
  while ((c=*s++) != END_OF_STRING) {
     const char* r = rem;
     char t;
    while ((t=*r++) != END_OF_STRING && t != c) ; // Scan for match
    if (t == END_OF_STRING)                       // If no match found
      *result++ = c;
  }
  *result = END_OF_STRING;                        // NULL terminate string
  return str;                                     // Return pointer to string
}

// Removes any prefix occurrence of the string rem from
// the first string str, and returns the modified string str.
char* vul_string_c_left_trim(char* str, const char* rem) // Trim prefix from string
{
  char* result = str;
  char* s;
   char c;
  for (s=str; (c=*s) != END_OF_STRING; s++) {
     const char* r = rem;
     char t;
    while ((t=*r++) != END_OF_STRING && t != c) ; // Scan for match
    if (t == END_OF_STRING)                       // If no match found
      break;
  }
  if (s != result)                                // when characters trimed
    while ((*result++ = *s++) != END_OF_STRING) ; // shift string down
  return str;                                     // Return pointer to string
}

// Removes any suffix occurrence of the string rem
// from the first string str, and returns the modified string str.
char* vul_string_c_right_trim(char* str, const char* rem) // Trim suffix from string
{
  char* s = str + std::strlen(str) - 1;            // last character of str
  for (; s >= str; s--) {
     const char* r = rem;
     char t;
     char c = *s;
    while ((t=*r++) != END_OF_STRING && t != c) ; // Scan for match
    if (t == END_OF_STRING)                       // If no match found
      break;
  }
  *(s+1) = END_OF_STRING;
  return str;                                     // Return pointer to string
}

// Reverses the order of the characters in char*.
char* vul_string_c_reverse(char* c)     // Reverse the order of characters
{
  int length = (int)std::strlen(c);      // Number of characters in string
  char temp;

  for (int i = 0, j = length-1;         // Counting from front and rear
       i < j; ++i, --j)                 // until we reach the middle
  {
    temp = c[i];                        // Save front character
    c[i] = c[j];                        // Switch with rear character
    c[j] = temp;                        // Copy new rear character
  }
  return c;
}

// Reverses the order of the characters in string
std::string& vul_string_reverse(std::string& s)
{
  for (int i=0, j=(int)std::strlen(s.c_str())-1; i<j; ++i,--j)
  {
    char c = s[i]; s[i] = s[j]; s[j] = c;
  }
  return s;
}

// In some implementations of <cctype>, toupper and tolower are macros
// instead of functions.  In that case, they cannot be passed as 4th argument
// to std::transform.  Hence it's easier to "inline" std::transform here,
// instead of using it explicitly. - PVr.

// Converts all alphabetical characters in string s to uppercase.
std::string& vul_string_upcase(std::string& s)
{
  for (char & i : s)
    i = (char)std::toupper(i);
  return s;
}

// Converts all alphabetical characters in string s to lowercase.
std::string& vul_string_downcase(std::string& s)
{
  for (char & i : s)
    i = (char)std::tolower(i);
  return s;
}

// Capitalizes all words in string s.
std::string& vul_string_capitalize(std::string& s)
{
  // Word beginnings are defined as the transition from
  // non-alphanumeric to alphanumeric, and word endings as the reverse
  // transition.
  std::string::iterator si;
  bool in_word = false;
  for ( si = s.begin(); si != s.end(); ++si ) {
    if ( !in_word && std::isalnum( *si ) ) {
      *si = (char)std::toupper( *si );
      in_word = true;
    }
    else if ( in_word && !std::isalnum( *si ) ) {
      in_word = false;
    }
  }
  return s;
}

// Removes any occurrence of the character string rem
// from the string sr, and returns the modified string sr.
std::string& vul_string_trim(std::string& sr, const char* rem)
{
  int l = (int)std::strlen(rem);
  for (;;) {
    std::string::size_type loc = sr.find(rem);
    if (loc == std::string::npos)
      break;
    sr.erase(loc, l);
  }
  return sr;
}

// Removes any prefix occurrence of the character string rem
// from the string sr, and returns the modified string sr.
std::string& vul_string_left_trim(std::string& sr, const char* rem)
{
  int l = (int)std::strlen(rem);
  if (std::strncmp(sr.c_str(), rem, l) == 0)
    sr.erase(0, l);
  return sr;
}

// Removes any suffix occurrence of the character string rem
// from the string sr, and returns the modified string sr.
std::string& vul_string_right_trim(std::string& sr, const char* rem)
{
  int l = (int)std::strlen(rem);
  int lsr = int(sr.length());
  if (std::strncmp(sr.c_str() + lsr - l, rem, l) == 0)
    sr.erase(lsr - l, l);
  return sr;
}

int vul_string_atoi(std::string const& s)
{
  return std::atoi(s.c_str());
}

double vul_string_atof(std::string const& s)
{
  return std::atof(s.c_str());
}


//: Reads a double from a string, with k, kb, M, etc suffix.
// No space is allowed between the number and the suffix.
// k=10^3, kb=2^10, M=10^6, Mb=2^20, G=10^9, Gb=2^30, T=10^12, Tb=2^40
// If parse fails, return 0.0;
double vul_string_atof_withsuffix(std::string const& s)
{
  std::istringstream ss(s);
  double d;
  ss >> d;
  if (!ss) return 0.0;
  if (ss.eof()) return d;

  char c='A';
  ss >> c;
  if (ss.eof()) return d;

  double e=0;
  switch (c)
  {
    case 'k': e=1; break;
    case 'M': e=2; break;
    case 'G': e=3; break;
    case 'T': e=4; break;
    default: return 0.0;
  }
  if (ss.eof()) return d*std::pow(10.0,3.0*e);

  c='A';
  ss >> c;
  if (ss.eof()) return d*std::pow(10.0,3.0*e);
  if (!ss || c!='i') return 0.0;

  ss >> c;
  if (!ss.eof()) return 0.0;

  return d*std::pow(2.0,10.0*e);
}

static bool NotSpace(char a)
{
  return !std::isspace(a);
}

template <class IT>
static bool myequals(IT b1, IT e1,
                     const char * b2, const char * e2)
{
  for (;b1 != e1 && b2 != e2; ++b1, ++b2)
    if (std::toupper(*b1) != *b2) return false;
  return b1 == e1
      && b2 == e2;
}

bool vul_string_to_bool(const std::string &str)
{
  std::string::const_iterator begin = std::find_if(str.begin(), str.end(), NotSpace);
  const std::string::const_reverse_iterator rend(begin);
  std::string::const_iterator end = std::find_if(str.rbegin(), rend, NotSpace).base();
  const char *syes = "YES";
  const char *strue = "TRUE";
  const char *s1 = "1";
  const char *son = "ON";
  return myequals(begin, end, syes, syes+3)
     ||  myequals(begin, end, strue, strue+4)
     ||  myequals(begin, end, s1, s1+1)
     ||  myequals(begin, end, son, son+2);
}


//: Convert a string to a list of ints, using the matlab index format.
// e.g. "0,1,10:14,20:-2:10" results in 0,1,10,11,12,13,14,20,18,16,14,12,10
// No spaces are allowed.
// \return empty on error.
std::vector<int> vul_string_to_int_list(std::string str)
{
  std::vector<int> rv;


#define REGEXP_INTEGER "\\-?[0123456789]+"

  vul_reg_exp range_regexp("(" REGEXP_INTEGER ")"      // int
                           "([:-]" REGEXP_INTEGER ")?" // :int [optional]
                           "([:-]" REGEXP_INTEGER ")?" // :int [optional]
                          );


  while (str.length() > 0 && range_regexp.find(str)) {
    // the start/end positions (ref from 0) of the
    //    current ',' separated token.
    std::ptrdiff_t start= range_regexp.start(0);
    std::ptrdiff_t endp = range_regexp.end(0);
    if (start != 0)
    {
      rv.clear();
      return rv;
    }


    std::string match1 = range_regexp.match(1);
    std::string match2 = range_regexp.match(2);
    std::string match3 = range_regexp.match(3);


    // Remove this match from the front of string.
    str.erase(0, endp);
    if (str.size() > 1 && str[0] == ',' ) str.erase(0, 1);

    bool matched2 = range_regexp.match(2).size() > 0;
    bool matched3 = range_regexp.match(3).size() > 0;

    int s = vul_string_atoi(match1);
    int d = 1;
    int e = s;
    if (matched3) {
      // "1:2:10"
      d = vul_string_atoi(match2.substr(1));
      e = vul_string_atoi(match3.substr(1));
    }
    else if (matched2)
      e = vul_string_atoi(match2.substr(1));

    if (d==0)
    {
      rv.clear();
      return rv;
    }

    if (e >= s)
    {
      if (d < 0) d = -d;
      for (int i = s; i <= e; i += d)
        rv.push_back(i);
    }
    else
    {
      if (d > 0) d = -d;
      for (int i = s; i >= e; i += d)
        rv.push_back(i);
    }
  }

  if (!str.empty())
    rv.clear();

  return rv;
}


//Leave verbatim in to avoid $->LaTeX munging.

//: Expand any environment variables in the string.
// Expands "foo$VARfoo" to "foobarfoo" when $VAR=bar. If both $VAR and $VARfoo
// exist, an arbitrary choice will be made of which variable to use.
// This problem can be avoided by using the syntax "foo${VAR}foo." "$(VAR)"
// and "$[VAR]" can also be used.
// There are no inbuilt variables like in shell scripting, and variable names
// cannot contain whitespace or "$"s.
// "$$" can be used to insert a literal "$" into the output.
// \returns false if a matching variable could not be found.
bool vul_string_expand_var(std::string &str)
{
  std::string::size_type i = 0; // index to current char.
  const std::string::size_type npos = std::string::npos;

  // If there is a problem, carry on trying to convert rest
  bool success=true; //  of string, but remember failure.

  enum {not_in_var, start_var, in_var, in_bracket_var} state = not_in_var;
  std::string::size_type var_begin = 0;

  std::string::size_type bracket_type = npos; //index into open_brackets.
  const std::string  open_brackets("{([");
  const std::string close_brackets("})]");

  while (i<str.size())
  {
    switch (state)
    {
     case not_in_var: // not currently in a variable
      if (str[i] == '$')
      {
        state = start_var;
        var_begin = i;
      }
      break;
     case start_var: // just started a variable
      if (str[i] == '$')
      {
        str.erase(i,1);
        state=not_in_var;
        continue;
      }
      else if ((bracket_type = open_brackets.find_first_of(str[i])) != npos)
      {
        state=in_bracket_var;
        break;
      }
      else // or this is the first letter of the variable, in which case go through
        state=in_var;
     case in_var:  // in a non-bracketed variable
      assert(var_begin+1 < str.size());
      assert(i > var_begin);
      if (str[i] == '$')
      { // no dollars allowed - assume we missed last variable and this is a new one.
        success=false;
        state = start_var;
        var_begin = i;
        break;
      }
      else
      {
        const char * value= std::getenv(str.substr(var_begin+1, i-var_begin).c_str());
        if (value)
        {
          str.replace(var_begin, i+1-var_begin, value);
          i = var_begin + std::strlen(value);
          state=not_in_var;
          continue;
        }
      }
      break;
     case in_bracket_var:  // in a bracketed variable
      if (str[i] == close_brackets[bracket_type])
      {
        assert(var_begin+2 < str.size());
        assert(i > var_begin+1);
        state=not_in_var;
        if (i==var_begin+2) // empty variable name
        {
          success=false;
          break;
        }
        else
        {
          const char * value= std::getenv(str.substr(var_begin+2, i-var_begin-2).c_str());
          if (value)
          {
            str.replace(var_begin, i+1-var_begin, value);
            i = var_begin + std::strlen(value);
            continue;
          }
          else
            success=false;
        }
      }
      break;
     default: // do nothing (silently ignore invalid state)
      break;
    }
    ++i;
  }
  return success;
}

//: replaces instances "find_str" in "full_str" with "replace_str" a given "num_times".
//  \returns true iff at least one replacement took place.
bool vul_string_replace(std::string& full_str,
                        const std::string& find_str,
                        const std::string& replace_str,
                        int num_times)
{
  bool rep=false;
  for (int i = 0; i<num_times; i++)
  {
    int loc = int(full_str.find( find_str,0));
    if (loc >= 0)
    {
      full_str.replace( loc, find_str.length(), replace_str );
      rep=true;
    }
    else
    {
      return rep;
    }
  }
  return rep;
}


//: Replace control chars with escaped representations.
// Space and "\n" are preserved, but tabs, CR, etc are escaped.
// This is not aimed and is not suitable for any particular input-validation
// security problem, such as sql-injection.
std::string vul_string_escape_ctrl_chars(const std::string &in)
{
  std::string out;

  const static std::string special("\t\v\b\r\f\a\\");
  const static std::string special_tr("tvbrfa\\");

  for (char it : in)
  {
    if (!std::iscntrl(it) || it=='\n')
      out+=it;
    else
    {
      std::string::size_type i=special.find(it);
      if (i==std::string::npos)
        out+=vul_sprintf("\\x%02x",static_cast<int>(it));
      else
      {
        out+='\\';
        out+=special_tr[i];
      }
    }
  }
  return out;
}
