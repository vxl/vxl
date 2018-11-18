#include <iostream>
#include <sstream>
#include "mbl_parse_colon_pairs_list.h"
//:
// \file
// \brief Parse list of string pairs separated by colons
// \author Tim Cootes

#include <mbl/mbl_exception.h>
#include <vsl/vsl_indent.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

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
                                std::vector<std::string>& item2)
{
  std::istringstream data_stream(data);
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
void mbl_parse_colon_pairs_list(std::istream& is,
                                std::vector<std::string>& item1,
                                std::vector<std::string>& item2)
{
  char c;
  is >> std::ws>>c;
  if (c!='{')
  {
    is.clear(std::ios::failbit);  // Set error flag
    std::string error_msg("Expecting '{' got: '");
    error_msg+=c;
    error_msg+="'";
    throw (mbl_exception_parse_error(error_msg));
  }

  // Empty lists
  item1.resize(0);
  item2.resize(0);

  std::string string1,string2;
  while (!is.eof())
  {
    // Read first item
    is >> string1;
    if (string1=="}") continue;
    item1.push_back(string1);

    // Check there is a colon
    is >> std::ws>>c;
    if (c!=':')
    {
      is.clear(std::ios::failbit);  // Set error flag
      std::string error_msg("Expecting ':' after ");
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
      is.clear(std::ios::failbit);  // Set error flag
      std::string error_msg("Expecting a string after ");
      error_msg+=string1;
      error_msg+=" : Got `}`";
      throw (mbl_exception_parse_error(error_msg));
    }
    item2.push_back(string2);
  }

  if (string1!="}")
  {
    std::string error_msg("Expected list to end with a }, not ");
    error_msg+=string2;
    is.clear(std::ios::failbit);  // Set error flag
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
void mbl_write_colon_pairs_list(std::ostream& os,
                                const std::vector<std::string>& item1,
                                const std::vector<std::string>& item2)
{
  assert(item1.size()==item2.size());
  os<<vsl_indent()<<'{'<<std::endl;
  vsl_indent_inc(os);
  for (unsigned i=0;i<item1.size();++i)
  {
    os<<vsl_indent()<<item1[i]<<" : "<<item2[i]<<std::endl;
  }
  vsl_indent_dec(os);
  os<<'}'<<std::endl;
}
