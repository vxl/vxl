// This is core/vul/vul_string.cxx

#include "vul_string.h"

#include <vcl_cassert.h>
#include <vcl_cstdlib.h>
#include <vcl_cstring.h>
#include <vcl_cctype.h>
#include <vcl_algorithm.h>

#define TO_LOWER vcl_tolower                    // use ANSI functions
#define TO_UPPER vcl_toupper

#ifndef END_OF_STRING                           // If END_OF_STRING not defined
#define END_OF_STRING (0)
#endif

// Converts all alphabetical characters to uppercase.
char* vul_string_c_upcase(char* s)  // Convert entire string to upper case
{
  char* p = s;                  // Point to beginning of string
  while (*p) {                  // While there are still valid characters
    if (vcl_islower(*p))        // if this is lower case
      *p = TO_UPPER(*p);        // convert to uppercase
    p++;                        // Advance pointer
  }
  return s;                     // Return reference to modified string
}

// Converts all alphabetical characters to lowercase.
char* vul_string_c_downcase(char* s)  // Convert entire string to lower case
{
  char* p = s;                  // Point to beginning of string
  while (*p) {                  // While there are still valid characters
    if (vcl_isupper(*p))        // if this is upper case
      *p = TO_LOWER(*p);        // convert to lowercase
    p++;                        // Advance pointer
  }
  return s;                     // Return reference to modified string
}

// Capitalizes all words in a string. A word is defined as
// a sequence of characters separated by non-alphanumerics.
char* vul_string_c_capitalize(char* s)  // Capitalize each word in string
{
  char* p = s;                          // Point to beginning of string
  while (true) {                        // Infinite loop
    for (; *p && !vcl_isalnum(*p); p++);// Skip to first alphanumeric
    if (*p == END_OF_STRING)            // If end of string
      return s;                         // Return string
    *p = TO_UPPER(*p);                  // Convert character
    while (*++p && vcl_isalnum(*p));    // Search for next word
  }
}

// Removes any occurrence of the string rem from string str,
// and returns the modified string str.
char* vul_string_c_trim(char* str, const char* rem) // Trim characters from string
{
  char* s = str;
  char* result = str;
  register char c;
  while ((c=*s++) != END_OF_STRING) {
    register const char* r = rem;
    register char t;
    while ((t=*r++) != END_OF_STRING && t != c); // Scan for match
    if (t == END_OF_STRING)                      // If no match found
      *result++ = c;
  }
  *result = END_OF_STRING;                      // NULL terminate string
  return str;                                   // Return pointer to string
}

// Removes any prefix occurrence of the string rem from
// the first string str, and returns the modified string str.
char* vul_string_c_left_trim(char* str, const char* rem) // Trim prefix from string
{
  char* result = str;
  char* s;
  register char c;
  for (s=str; (c=*s) != END_OF_STRING; s++) {
    register const char* r = rem;
    register char t;
    while ((t=*r++) != END_OF_STRING && t != c); // Scan for match
    if (t == END_OF_STRING)                      // If no match found
      break;
  }
  if (s != result)                               // when characters trimed
    while ((*result++ = *s++) != END_OF_STRING); // shift string down
  return str;                                    // Return pointer to string
}

// Removes any suffix occurrence of the string rem
// from the first string str, and returns the modified string str.
char* vul_string_c_right_trim(char* str, const char* rem) // Trim suffix from string
{
  char* s = str + vcl_strlen(str) - 1;           // last character of str
  for (; s >= str; s--) {
    register const char* r = rem;
    register char t;
    register char c = *s;
    while ((t=*r++) != END_OF_STRING && t != c); // Scan for match
    if (t == END_OF_STRING)                      // If no match found
      break;
  }
  *(s+1) = END_OF_STRING;
  return str;                                     // Return pointer to string
}

// Reverses the order of the characters in char*.
char* vul_string_c_reverse(char* c)     // Reverse the order of characters
{
  int length = vcl_strlen(c);           // Number of characters in string
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
vcl_string& vul_string_reverse(vcl_string& s)
{
  for (int i=0, j=vcl_strlen(s.c_str())-1; i<j; ++i,--j)
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
vcl_string& vul_string_upcase(vcl_string& s)
{
  for (vcl_string::iterator i=s.begin(); i != s.end(); ++i)
    *i = vcl_toupper(*i);
  return s;
}

// Converts all alphabetical characters in string s to lowercase.
vcl_string& vul_string_downcase(vcl_string& s)
{
  for (vcl_string::iterator i=s.begin(); i != s.end(); ++i)
    *i = vcl_tolower(*i);
  return s;
}

// Capitalizes all words in string s.
vcl_string& vul_string_capitalize(vcl_string& s)
{
  // Word beginnings are defined as the transition from
  // non-alphanumeric to alphanumeric, and word endings as the reverse
  // transition.
  vcl_string::iterator si;
  bool in_word = false;
  for ( si = s.begin(); si != s.end(); ++si ) {
    if ( !in_word && vcl_isalnum( *si ) ) {
      *si = vcl_toupper( *si );
      in_word = true;
    } else if ( in_word && !vcl_isalnum( *si ) ) {
      in_word = false;
    }
  }
  return s;
}

// Removes any occurrence of the character string rem
// from the string sr, and returns the modified string sr.
vcl_string& vul_string_trim(vcl_string& sr, const char* rem)
{
  int l = vcl_strlen(rem);
  for (;;) {
    vcl_string::size_type loc = sr.find(rem);
    if (loc == vcl_string::npos)
      break;
    sr.erase(loc, l);
  }
  return sr;
}

// Removes any prefix occurrence of the character string rem
// from the string sr, and returns the modified string sr.
vcl_string& vul_string_left_trim(vcl_string& sr, const char* rem)
{
  int l = vcl_strlen(rem);
  if (vcl_strncmp(sr.c_str(), rem, l) == 0)
    sr.erase(0, l);
  return sr;
}

// Removes any suffix occurrence of the character string rem
// from the string sr, and returns the modified string sr.
vcl_string& vul_string_right_trim(vcl_string& sr, const char* rem)
{
  int l = vcl_strlen(rem);
  int lsr = sr.length();
  if (vcl_strncmp(sr.c_str() + lsr - l, rem, l) == 0)
    sr.erase(lsr - l, l);
  return sr;
}

int vul_string_atoi(vcl_string const& s)
{
  return vcl_atoi(s.c_str());
}

double vul_string_atof(vcl_string const& s)
{
  return vcl_atof(s.c_str());
}

static bool NotSpace(char a)
{
  return !vcl_isspace(a);
}

template <class IT>
static bool myequals(IT b1, IT e1,
                     const char * b2, const char * e2)
{
  for (;b1 != e1 && b2 != e2; ++b1, ++b2)
    if (vcl_toupper(*b1) != *b2) return false;
  if (b1 == e1 && b2 == e2) return true;
  return false;
}

bool vul_string_to_bool(const vcl_string &str)
{
  vcl_string::const_iterator begin = vcl_find_if(str.begin(), str.end(), NotSpace);
  const vcl_string::const_reverse_iterator rend(begin);
  vcl_string::const_iterator end = vcl_find_if(str.rbegin(), rend, NotSpace).base();
  const char *syes = "YES";
  const char *strue = "TRUE";
  const char *s1 = "1";
  const char *son = "ON";
  return myequals(begin, end, syes, syes+3)
     ||  myequals(begin, end, strue, strue+4)
     ||  myequals(begin, end, s1, s1+1)
     ||  myequals(begin, end, son, son+2);
}

//: Expand any environment variables in the string.
// Expands "foo$VARfoo" to "foobarfoo" when $VAR=bar. If
// both $VAR and $VARfoo exists, an arbitrary choice will
// be made of which variable to use. This problem can
// be avoided by using the syntax "foo${VAR}foo." There
// are no inbuilt variables like in shell scripting, and
// variable names cannot contain whitespace or "$"s. "$$"
// can be used to insert a literal "$" in to the output.
// \returns false if a matching variable could not be found.
bool vul_string_expand_var(vcl_string &str)
{
  vcl_string::size_type i = 0; // index to current char.
  
  // If there is a problem, carry on trying to convert rest
  bool success=true; //  of string, but remember failure.

  enum {not, start_var, in_var, in_bracket_var} state = not;
  vcl_string::size_type var_begin;

  while (i<str.size())
  {
    switch (state)
    {
    case not: // not currently in a variable
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
        state=not;
        continue;
      }
      else if (str[i] == '{')
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
        const char * value= vcl_getenv(str.substr(var_begin+1, i-var_begin).c_str());
        if (value)
        {
          str.replace(var_begin, i+1-var_begin, value);
          i = var_begin + vcl_strlen(value);
          state=not;
          continue;
        }
      }
      break;
    case in_bracket_var:  // in a bracketed variable
      if (str[i] == '}')
      {
        assert(var_begin+2 < str.size());
        assert(i > var_begin+1);
        state=not;
        if (i==var_begin+2) // empty variable name
        {
          success=false;
          break;
        }
        else
        {
          const char * value= vcl_getenv(str.substr(var_begin+2, i-var_begin-2).c_str());
          if (value)
          {
            str.replace(var_begin, i+1-var_begin, value);
            i = var_begin + vcl_strlen(value);
            continue;
          }
          else
            success=false;
        }
      }
      break;
    }
    ++i;
  }
  return success;
}







