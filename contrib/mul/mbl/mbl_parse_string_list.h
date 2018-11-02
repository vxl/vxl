#ifndef mbl_parse_string_list_h_
#define mbl_parse_string_list_h_

//:
// \file
// \brief Parse list of strings
// \author Tim Cootes

#include <string>
#include <vector>
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Parse list of strings
// Expects format of data:
// \verbatim
// {
//   string1 string2 string3 // Comment ignored
//   string4 string5
// }
// \param comment_str defines string indicating comments
// \endverbatim
// Throws a mbl_exception_parse_error if it fails.
void mbl_parse_string_list(std::istream& is,
                          std::vector<std::string>& items,
                          const std::string& comment_str="//");

//: Parse list of strings
// Expects format of data:
// \verbatim
// {
//   string1 string2 string3 ...
// }
// \endverbatim
// \param comment_str defines string indicating comments
// Throws a mbl_exception_parse_error if it fails.
void mbl_parse_string_list(const std::string& data,
                          std::vector<std::string>& items,
                          const std::string& comment_str="//");


#endif // mbl_parse_string_list_h_
