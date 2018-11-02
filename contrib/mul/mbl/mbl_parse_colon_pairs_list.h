#ifndef mbl_parse_colon_pairs_list_h_
#define mbl_parse_colon_pairs_list_h_

//:
// \file
// \brief Parse list of string pairs separated by colons
// \author Tim Cootes

#include <string>
#include <vector>
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Parse list of string pairs separated by colons
// Expects format of data string to contain pairs of
// strings, with colons between items, eg
// \verbatim
// {
//   item1_0 : item2_0
//   item1_1 : item2_1
//   item1_2 : item2_2
// }
// \endverbatim
// Throws a mbl_exception_parse_error if it fails.
void mbl_parse_colon_pairs_list(std::istream& is,
                          std::vector<std::string>& item1,
                          std::vector<std::string>& item2);

//: Parse list of string pairs separated by colons
// Expects format of data string to contain pairs of
// strings, with colons between items, eg
// \verbatim
// {
//   item1_0 : item2_0
//   item1_1 : item2_1
//   item1_2 : item2_2
// }
// \endverbatim
// Throws a mbl_exception_parse_error if it fails.
void mbl_parse_colon_pairs_list(const std::string& data,
                          std::vector<std::string>& item1,
                          std::vector<std::string>& item2);

//: Writes pairs to a stream, separated by colons
// Format of output
// \verbatim
// {
//   item1[0] : item2[0]
//   item1[1] : item2[1]
// }
// \endverbatim
void mbl_write_colon_pairs_list(std::ostream& os,
                          const std::vector<std::string>& item1,
                          const std::vector<std::string>& item2);

#endif // mbl_parse_colon_pairs_list_h_
