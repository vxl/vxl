// This is core/vul/vul_string.h
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
extern char* vul_string_c_downcase(char*);
//: Capitalizes all words in a string.
// A word is defined as a sequence of characters separated by
// non-alphanumerics.
extern char* vul_string_c_capitalize(char*);
//: Removes any occurrences of rem from str, and returns the modified string.
extern char* vul_string_c_trim(char* str, const char* rem);
//: Removes any prefix occurrence of rem from str and returns modified string.
extern char* vul_string_c_left_trim(char* str, const char* rem);
//: Removes any suffix occurrence of rem from str and returns modified string.
extern char* vul_string_c_right_trim(char* str, const char* rem);
//: Reverses the order of the characters in string.
extern char* vul_string_c_reverse(char*);

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
//: Reverses the order of the characters in string
extern vcl_string& vul_string_reverse(vcl_string&);

//: Reads an integer from a string
extern int vul_string_atoi(vcl_string const&);

//: Reads an double from a string
extern double vul_string_atof(vcl_string const& s);

//: Convert a string to a boolean.
// Looks for On, true, yes, 1 to mean true. everything else is false.
// It ignores leading and trailing whitespace and capitalisation.
extern bool vul_string_to_bool(const vcl_string &str);

//: Expand any environment variables in the string.
// Expands "foo$VARfoo" to "foobarfoo" when $VAR=bar. If
// both $VAR and $VARfoo exists, an arbitrary choice will
// be made of which variable to use. This problem can
// be avoided by using the syntax "foo${VAR}foo." There
// are no inbuilt variables like in shell scripting. "$$"
// can be used to insert a literal "$" in to the output.
// \returns false if a matching variable could not be found.
extern bool vul_string_expand_var(vcl_string &str);


#endif // vul_string_h
