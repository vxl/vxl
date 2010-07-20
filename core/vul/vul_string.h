// This is core/vul/vul_string.h
#ifndef vul_string_h
#define vul_string_h
//:
// \file
// \brief Utility functions for C strings and vcl_strings

#include <vcl_string.h>
#include <vcl_vector.h>

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

//: Reads a double from a string
extern double vul_string_atof(vcl_string const& s);

//: Reads a double from a string, with k, kb, M, etc suffix.
// No space is allowed between the number and the suffix.
// k=10^3, ki=2^10, M=10^6, Mi=2^20, G=10^9, Gi=2^30, T=10^12, Ti=2^40
// The i suffix is from the IEC 60027 standard.
extern double vul_string_atof_withsuffix(vcl_string const& s);

//: Convert a string to a boolean.
// Looks for On, true, yes, 1 to mean true. everything else is false.
// It ignores leading and trailing whitespace and capitalisation.
extern bool vul_string_to_bool(const vcl_string &str);

//: Convert a string to a list of ints.
extern vcl_vector<int> vul_string_to_int_list(vcl_string str);

//: Expand any environment variables in the string.
// \verbatim
// Expands "foo$VARfoo" to "foobarfoo" when $VAR=bar. If
// both $VAR and $VARfoo exists, an arbitrary choice will
// be made of which variable to use. This problem can
// be avoided by using the syntax "foo${VAR}foo." "$(VAR)" and
// "$[VAR]" can also be used. There are no inbuilt variables
// like in shell scripting. "$$" can be used to insert a
// literal "$" in to the output.
// \endverbatim
// \returns false if a matching variable could not be found.
extern bool vul_string_expand_var(vcl_string &str);

//: replaces instances "find_str" in "full_str" with "replace_str" a given "num_times" (default 1000).
//  \returns true iff at least one replacement took place.
extern bool vul_string_replace( vcl_string& full_str,
                                const vcl_string& find_str,
                                const vcl_string& replace_str,
                                int num_times=1000);

//: Replace control chars with escaped representations.
// Space and \n are preserved, but tabs, CR, etc are escaped.
// This is not aimed and is not suitable for any particular input-validation
// security problem, such as sql-injection.
vcl_string vul_string_escape_ctrl_chars(const vcl_string &in);


#endif // vul_string_h
