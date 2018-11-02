#ifndef mbl_parse_keyword_list_h_
#define mbl_parse_keyword_list_h_

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

//: Read in keyword separated blocks of text from stream.
// Assumes list of blocks separated by a keyword.
//  keyword is always the same word, defined in the input variable.
// Expects format of data:
// \verbatim
// {
//   object: { data: fruit0 }
//   object: { data: fruit1 }
//   object: { data: fruit2 }
// }
// To parse this, require keyword="object:".
// For this example, on exit, items[1]=="{ data: fruit1 }", which could
// be passed to mbl_read_props.
// \endverbatim
// Throws a mbl_exception_parse_error if it fails.
void mbl_parse_keyword_list(std::istream& is, const std::string& keyword,
                std::vector<std::string>& items,
                bool discard_comments = false);


//: Read in keyword separated blocks of text from stream.
// Assumes list of blocks separated by a keyword.
//  keyword is always the same word, defined in the input variable.
// Expects format of data:
// \verbatim
// {
//   object: thing0 { data: fruit0 }
//   object: thing1 { data: fruit1 }
//   object: thing2 { data: fruit2 }
// }
// To parse this, require keyword="object:".
// For this example, on exit, items[1]=="thing1 { data: fruit1 }".
// \endverbatim
// Throws a mbl_exception_parse_error if it fails.
void mbl_parse_keyword_list2(std::istream& is, const std::string& keyword,
                std::vector<std::string>& items,
                bool discard_comments = false);

#endif // mbl_parse_keyword_list_h_
