// This is mul/mbl/mbl_read_props.cxx
#include "mbl/mbl_read_props.h"
//:
//  \file

#include <vsl/vsl_indent.h>
#include <vcl_string.h>

void mbl_read_props_print(vcl_ostream &afs, mbl_read_props_type props)
{
  typedef vcl_map<vcl_string, vcl_string>::iterator ITER;
  afs << vsl_indent() << "{\n";
  vsl_indent_inc(afs);
  for (ITER i = props.begin(); i != props.end(); ++i)
    afs << vsl_indent() << (*i).first << ": " << (*i).second << '\n';
  vsl_indent_dec(afs);
  afs << vsl_indent() << "}\n";
}


//: Read properties from a text stream.
// The function will terminate on an eof. If one of
// the opening lines contains an opening brace '{', then the function
// will also stop reading the stream after finding a line containing
// a closing brace '}'
//
// There should be one property per line, with a colon ':' after
// each property label. Each property label should not contain
// any whitespace.
mbl_read_props_type mbl_read_props(vcl_istream &afs)
{
  if (!afs) return mbl_read_props_type();

  vcl_string label, str1;

  while (afs>>vcl_ws, !afs.eof())
  {
    afs>>label;
    if (label.substr(0,2) =="//")
    {
      // Comment line, so read to end
      vcl_getline(afs, str1);
    }
    else break;
  }
  bool need_closing_brace = false;

  if (label[0] == '{')
    need_closing_brace = true;

  mbl_read_props_type props;

  while (afs>>vcl_ws, !afs.eof())
  {
    afs >> label;
    if (label.substr(0,2) =="//")
    {
      // Comment line, so read to end
      vcl_getline(afs, str1);
    }
    else if (need_closing_brace && label[0] == '}')
    {
      // Strip rest of line
      return props;
    }
    else
    {
      if (label.size() > 1 && label[label.size() -1] == ':')
        label.erase(label.size() -1, 1);
      else
      {
        char c;
        afs >> vcl_ws;
        afs >> c;
        if (c != ':')
        {
          vcl_getline(afs, str1);
          vcl_cerr << "ERROR: mbl_read_props. Could not find colon ':'"
                   <<"separator while reading line"
                   << label << " " << str1 << '\n';
          return props;
        }
      }
      afs >> vcl_ws;
      vcl_getline(afs, str1);
      props[label] = str1;
    }
  }
  if (need_closing_brace)
    vcl_cerr << "ERROR: mbl_read_props. Unexpected end of file while "
             << "looking for '}'\n";
  return props;
}
