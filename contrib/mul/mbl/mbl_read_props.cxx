// This is mul/mbl/mbl_read_props.cxx
#include "mbl_read_props.h"
//:
// \file

#include <vsl/vsl_indent.h>
#include <vcl_string.h>

#include <mbl/mbl_parse_block.h>

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
  {
    need_closing_brace = true;
    label.erase(0,1);
  }

  mbl_read_props_type props;

  if (label.empty())
    afs >> vcl_ws >> label;

  vcl_string last_label( label );

  do 
  {
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
    else if (!label.empty())
    {
      if (label.size() > 1 && label[label.size() -1] == ':')
      {
        label.erase(label.size() -1, 1);
        afs >> vcl_ws;
        vcl_getline(afs, str1);
        props[label] = str1;
        last_label = label;
      }
      else if ( label == "{" )
      {
        vcl_string block = mbl_parse_block( afs, true );
        if ( block != "{}" )
        {
          vcl_string prop = props[ last_label ];
          prop += "\n";
          prop += block;
          props[ last_label ] = prop;
        }
      }
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
    }
    afs >> vcl_ws >> label;
  }
  while (!afs.eof());
  if (need_closing_brace && label != "}")
    vcl_cerr << "ERROR: mbl_read_props. Unexpected end of file while "
             << "looking for '}'. Last read string = \"" << label << "\"\n";
  return props;
}


//: merge two property sets.
// \param first_overrides properties in "a" will override indentically
// named properties in "b"
mbl_read_props_type mbl_read_props_merge(const mbl_read_props_type& a,
  const mbl_read_props_type& b, bool first_overrides/*=true*/)
{
  mbl_read_props_type output;

  mbl_read_props_type::const_iterator a_it = a.begin();
  mbl_read_props_type::const_iterator b_it = b.begin();


  while (a_it != a.end() || b_it != b.end())
  {
    if (a_it == a.end())
      output.insert(*(b_it++));
    else if (b_it == b.end())
      output.insert(*(a_it++));
    else if (a_it->first < b_it->first)
      output.insert(*(a_it++));
    else if (a_it->first > b_it->first)
      output.insert(*(b_it++));
    else
    {
      if (first_overrides)
        output.insert(*a_it);
      else
        output.insert(*b_it);
      ++a_it; ++b_it;
    }
  }
  return output;
}
