// This is vxl/vul/vul_string.cxx

#include "vul_string.h"

#include <vcl_cstdlib.h>
#include <vcl_cstring.h>
#include <vcl_cctype.h>
#include <vcl_algorithm.h>
#include <vcl_iterator.h>

#define TO_LOWER vcl_tolower                    // use ANSI functions
#define TO_UPPER vcl_toupper

#ifndef END_OF_STRING                           // If END_OF_STRING not defined
#define END_OF_STRING (0)
#endif

// Converts all alphabetical characters to uppercase.
char* vul_string_c_upcase (char* s)      // Convert entire string to upper case
{
  char* p = s;                  // Point to beginning of string
  while (*p) {                  // While there are still valid characters
    if (vcl_islower (*p))       // if this is lower case
      *p = TO_UPPER (*p);       // convert to uppercase
    p++;                        // Advance pointer
  }
  return s;                     // Return reference to modified string
}

// Converts all alphabetical characters to lowercase.
char* vul_string_c_downcase (char* s)    // Convert entire string to lower case
{
  char* p = s;                  // Point to beginning of string
  while (*p) {                  // While there are still valid characters
    if (vcl_isupper (*p))       // if this is upper case
      *p = TO_LOWER (*p);       // convert to lowercase
    p++;                        // Advance pointer
  }
  return s;                     // Return reference to modified string
}

// Capitalizes all words in a string. A word is defined as
// a sequence of characters separated by non-alphanumerics.
char* vul_string_c_capitalize (char* s)       // Capitalize each word in string
{
  char* p = s;                          // Point to beginning of string
  while (true) {                        // Infinite loop
    for (; *p && !vcl_isalnum(*p); p++);// Skip to first alphanumeric
    if (*p == END_OF_STRING)            // If end of string
      return s;                         // Return string
    *p = TO_UPPER(*p);                  // Convert character
    while (*++p && vcl_isalnum (*p));   // Search for next word
  }
}

// Removes any occurrence of the string rem from string str,
// and returns the modified string str.
char* vul_string_c_trim (char* str, const char* rem) // Trim characters from string
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
char* vul_string_c_left_trim (char* str, const char* rem) // Trim prefix from string
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
char* vul_string_c_right_trim (char* str, const char* rem) // Trim suffix from string
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
void vul_string_c_reverse (char* c)          // Reverse the order of characters
{
  int length = vcl_strlen (c);          // Number of characters in string
  char temp;
 
  for (int i = 0, j = length-1;         // Counting from front and rear
       2*i < length; ++i, --j)          // until we reach the middle
  {
    temp = c[i];                        // Save front character
    c[i] = c[j];                        // Switch with rear character
    c[j] = temp;                        // Copy new rear character
  }
}

// Converts all alphabetical characters in string s to uppercase.
vcl_string& vul_string_upcase(vcl_string& s)
{
  vcl_transform( s.begin(), s.end(), s.begin(), vcl_toupper );
  return s;
}

// Converts all alphabetical characters in string s to lowercase.
vcl_string& vul_string_downcase(vcl_string& s)
{
  vcl_transform( s.begin(), s.end(), s.begin(), vcl_tolower );
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
  const char syes[] = "YES";
  const char strue[] = "TRUE";
  const char s1[] = "1";
  const char son[] = "ON";
  if (myequals(begin, end, syes, syes+3)) return true;
  if (myequals(begin, end, strue, strue+4)) return true;
  if (myequals(begin, end, s1, s1+1)) return true;
  if (myequals(begin, end, son, son+2)) return true;
  return false;
}

bool vul_string_c_to_bool(const char *str)
{
  const char *begin = vcl_find_if(str, str + vcl_strlen(str), NotSpace);
#ifdef VCL_VC60
  const vcl_reverse_iterator<const char *, const char>
#else
  const vcl_reverse_iterator<const char *>
#endif
    rend(begin), rbegin(str+vcl_strlen(str)); 
  const char * end = vcl_find_if(rbegin, rend, NotSpace).base();
  const char syes[] = "YES";
  const char strue[] = "TRUE";
  const char s1[] = "1";
  const char son[] = "ON";
  if (myequals(begin, end, syes, syes+3)) return true;
  if (myequals(begin, end, strue, strue+4)) return true;
  if (myequals(begin, end, s1, s1+1)) return true;
  if (myequals(begin, end, son, son+2)) return true;
  return false;
}
