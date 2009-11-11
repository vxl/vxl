#ifndef mbl_parse_string_list_h_
#define mbl_parse_string_list_h_

//:
// \file
// \brief Parse list of strings
// \author Tim Cootes

#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_iostream.h>

//: Parse list of strings
// Expects format of data:
// \verbatim
// {
//   string1 string2 string3 ...
// }
// \endverbatim
// Throws a mbl_exception_parse_error if it fails.
void mbl_parse_string_list(vcl_istream& is,
                          vcl_vector<vcl_string>& items);

//: Parse list of strings
// Expects format of data:
// \verbatim
// {
//   string1 string2 string3 ...
// }
// \endverbatim
// Throws a mbl_exception_parse_error if it fails.
void mbl_parse_string_list(const vcl_string& data,
                          vcl_vector<vcl_string>& items);


#endif // mbl_parse_string_list_h_
