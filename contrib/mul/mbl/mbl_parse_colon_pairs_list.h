#ifndef mbl_parse_colon_pairs_list_h_
#define mbl_parse_colon_pairs_list_h_

//:
// \file
// \brief Parse list of string pairs separated by colons
// \author Tim Cootes

#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_iostream.h>

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
void mbl_parse_colon_pairs_list(vcl_istream& is,
                          vcl_vector<vcl_string>& item1,
                          vcl_vector<vcl_string>& item2);

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
void mbl_parse_colon_pairs_list(const vcl_string& data,
                          vcl_vector<vcl_string>& item1,
                          vcl_vector<vcl_string>& item2);

//: Writes pairs to a stream, separated by colons
// Format of output
// \verbatim
// {
//   item1[0] : item2[0]
//   item1[1] : item2[1]
// }
// \endverbatim
void mbl_write_colon_pairs_list(vcl_ostream& os,
                          const vcl_vector<vcl_string>& item1,
                          const vcl_vector<vcl_string>& item2);

#endif // mbl_parse_colon_pairs_list_h_
