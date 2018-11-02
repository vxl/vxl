// This is core/vul/vul_string.h
#ifndef vul_string_h
#define vul_string_h
//:
// \file
// \brief Utility functions for C strings and std::strings

#include <string>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

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

// std::string functions:

//: Converts all alphabetical characters to uppercase.
extern std::string& vul_string_upcase(std::string&);
//: Converts all alphabetical characters to lowercase.
extern std::string& vul_string_downcase(std::string&);
//: Capitalizes all words in string.
extern std::string& vul_string_capitalize(std::string&);
//: Removes any occurrences of rem from str and returns modified string
extern std::string& vul_string_trim(std::string&, const char*);
//: Removes any prefix occurrence of rem from str and returns modified string
extern std::string& vul_string_left_trim(std::string&, const char*);
//: Removes any suffix occurrence of rem from str and returns modified string
extern std::string& vul_string_right_trim(std::string&, const char*);
//: Reverses the order of the characters in string
extern std::string& vul_string_reverse(std::string&);

//: Reads an integer from a string
extern int vul_string_atoi(std::string const&);

//: Reads a double from a string
extern double vul_string_atof(std::string const& s);

//: Reads a double from a string, with k, kb, M, etc suffix.
// No space is allowed between the number and the suffix.
// k=10^3, ki=2^10, M=10^6, Mi=2^20, G=10^9, Gi=2^30, T=10^12, Ti=2^40
// The i suffix is from the IEC 60027 standard.
extern double vul_string_atof_withsuffix(std::string const& s);

//: Convert a string to a boolean.
// Looks for On, true, yes, 1 to mean true. everything else is false.
// It ignores leading and trailing whitespace and capitalisation.
extern bool vul_string_to_bool(const std::string &str);

//: Convert a string to a list of ints.
extern std::vector<int> vul_string_to_int_list(std::string str);

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
extern bool vul_string_expand_var(std::string &str);

//: replaces instances "find_str" in "full_str" with "replace_str" a given "num_times" (default 1000).
//  \returns true iff at least one replacement took place.
extern bool vul_string_replace( std::string& full_str,
                                const std::string& find_str,
                                const std::string& replace_str,
                                int num_times=1000);

//: Replace control chars with escaped representations.
// Space and \n are preserved, but tabs, CR, etc are escaped.
// This is not aimed and is not suitable for any particular input-validation
// security problem, such as sql-injection.
std::string vul_string_escape_ctrl_chars(const std::string &in);


#endif // vul_string_h
