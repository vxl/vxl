#include "mbl_parse_colon_pairs_list.h"
//:
// \file
// \brief Parse list of string pairs separated by colons
// \author Tim Cootes

#include <mbl/mbl_exception.h>
#include <vsl/vsl_indent.h>
#include <vcl_sstream.h>
#include <vcl_cassert.h>

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
                                vcl_vector<vcl_string>& item2)
{
  vcl_istringstream data_stream(data);
  mbl_parse_colon_pairs_list(data_stream,item1,item2);
}

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
                                vcl_vector<vcl_string>& item2)
{
  char c;
  is >> vcl_ws>>c;
  if (c!='{')
  {
    is.clear(vcl_ios::failbit);  // Set error flag
    vcl_string error_msg("Expecting '{' got: '");
    error_msg+=c;
    error_msg+="'";
    throw (mbl_exception_parse_error(error_msg));
  }

  // Empty lists
  item1.resize(0);
  item2.resize(0);

  vcl_string string1,string2;
  while (!is.eof())
  {
    // Read first item
    is >> string1;
    if (string1=="}") continue;
    item1.push_back(string1);

    // Check there is a colon
    is >> vcl_ws>>c;
    if (c!=':')
    {
      is.clear(vcl_ios::failbit);  // Set error flag
      vcl_string error_msg("Expecting ':' after ");
      error_msg+=string1;
      error_msg+=" Got `";
      error_msg+=c;
      error_msg+="'";
      throw (mbl_exception_parse_error(error_msg));
    }

    // Read second item
    is >> string2;
    if (string2=="}")
    {
      is.clear(vcl_ios::failbit);  // Set error flag
      vcl_string error_msg("Expecting a string after ");
      error_msg+=string1;
      error_msg+=" : Got `}`";
      throw (mbl_exception_parse_error(error_msg));
    }
    item2.push_back(string2);
  }

  if (string1!="}")
  {
    vcl_string error_msg("Expected list to end with a }, not ");
    error_msg+=string2;
    is.clear(vcl_ios::failbit);  // Set error flag
    throw (mbl_exception_parse_error(error_msg));
  }
}

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
                                const vcl_vector<vcl_string>& item2)
{
  assert(item1.size()==item2.size());
  os<<vsl_indent()<<'{'<<vcl_endl;
  vsl_indent_inc(os);
  for (unsigned i=0;i<item1.size();++i)
  {
    os<<vsl_indent()<<item1[i]<<" : "<<item2[i]<<vcl_endl;
  }
  vsl_indent_dec(os);
  os<<'}'<<vcl_endl;
}

