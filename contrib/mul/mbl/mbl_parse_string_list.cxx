#include "mbl_parse_string_list.h"
//:
// \file
// \brief Parse list of strings
// \author Tim Cootes

#include <mbl/mbl_exception.h>
#include <mbl/mbl_parse_block.h>
#include <vcl_sstream.h>

//: Parse list of strings
// Expects format of data:
// \verbatim
// {
//   string1 string2 string3 ...
// }
// \endverbatim
// Throws a mbl_exception_parse_error if it fails.
void mbl_parse_string_list(vcl_istream& is,
                           vcl_vector<vcl_string>& items,
                           const vcl_string& comment_str)
{
  vcl_string s = mbl_parse_block(is);
  vcl_istringstream ss(s);
  char c;
  ss>>c;  // Remove opening brace
  if (c!='{')
  {
    throw mbl_exception_parse_error("Expected '{' in mbl_parse_string_list");
  }

  unsigned comment_len = comment_str.size();

  items.resize(0);
  vcl_string label;
  while (!ss.eof())
  {
    ss >> label;
    if (comment_len>0 &&
        label.length()>=comment_len &&
        label.substr(0,comment_len)==comment_str)
    {
      // label begins with comment_str
      // - treat as comment and discard rest of line
      vcl_string dummy;
      vcl_getline(ss,dummy);
      continue;
    }
    if (label == "}") continue;
    items.push_back(label);
  }

  if (label!="}")
  {
    throw mbl_exception_parse_error("Expected closing '}' in mbl_parse_string_list");
  }
}

//: Parse list of strings
// Expects format of data:
// \verbatim
// {
//   string1 string2 string3 ...
// }
// \endverbatim
// Throws a mbl_exception_parse_error if it fails.
void mbl_parse_string_list(const vcl_string& data,
                           vcl_vector<vcl_string>& items,
                           const vcl_string& comment_str)
{
  vcl_istringstream data_stream(data);
  mbl_parse_string_list(data_stream,items,comment_str);
}


