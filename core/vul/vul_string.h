// This is vxl/vul/vul_string.h
#ifndef vul_string_h
#define vul_string_h
//:
// \file
// \brief Utility functions for C strings and vcl_strings

#include <vcl_string.h>

// C string functions:

//: Converts all alphabetical characters to uppercase.
extern char* vul_string_c_upcase(char*);
//: Converts all alphabetical characters to lowercase.
extern char* vul_string_c_downcase (char*);
//: Capitalizes all words in a string.
// A word is defined as a sequence of characters separated by
// non-alphanumerics.
extern char* vul_string_c_capitalize (char*);
//: Removes any occurrences of rem from str, and returns the modified string.
extern char* vul_string_c_trim (char* str, const char* rem);
//: Removes any prefix occurrence of rem from str and returns modified string.
extern char* vul_string_c_left_trim (char* str, const char* rem);
//: Removes any suffix occurrence of rem from str and returns modified string.
extern char* vul_string_c_right_trim (char* str, const char* rem);
//: Reverses the order of the characters in char*.
extern void  vul_string_c_reverse(char*);

// vcl_string functions:

//: Converts all alphabetical characters to uppercase.
extern vcl_string& vul_string_upcase(vcl_string&);
//: Converts all alphabetical characters to lowercase.
extern vcl_string& vul_string_downcase(vcl_string&);
//: Capitalizes all words in string.
extern vcl_string& vul_string_capitalize(vcl_string&);
//: Removes any occurrences of rem from str and returns modified string
extern vcl_string& vul_string_trim(vcl_string&, const char*);
//: Removes any prefix occurrence of rem from str and returns modified string
extern vcl_string& vul_string_left_trim(vcl_string&, const char*);
//: Removes any suffix occurrence of rem from str and returns modified string
extern vcl_string& vul_string_right_trim(vcl_string&, const char*);
//: Converts asci to integer
extern int vul_string_atoi(vcl_string const&);

//: Convert a string to a boolean.
// Looks for On, true, yes, 1 to mean true. everything else is false.
// It ignores leading and trailing whitespace and capitalisation.
extern bool vul_string_to_bool(const vcl_string &str);

#endif // vul_string_h
