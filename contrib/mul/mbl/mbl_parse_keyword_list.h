#ifndef mbl_parse_keyword_list_h_
#define mbl_parse_keyword_list_h_

//:
// \file
// \brief Parse list of strings
// \author Tim Cootes

#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_iostream.h>

//: Read in data from a stream, assumed to be a list of items
// Assumes list of objects separated by a keyword.
//  keyword is always the same word, defined in the input variable.
// Expects format of data:
// \verbatim
// {
//   keyword: string1
//   keyword: { data: fruit }
//   keyword: string3
// }
// To parse this, require keyword="keyword:". 
// For this example, on exit, items[1]=="{ data: fruit }", which could
// be passed to mbl_read_props.
// \endverbatim
// Throws a mbl_exception_parse_error if it fails.
void mbl_parse_keyword_list(vcl_istream& is, const vcl_string& keyword,
                vcl_vector<vcl_string>& items);

#endif // mbl_parse_keyword_list_h_
