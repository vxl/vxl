// This is mul/mbl/mbl_read_props.cxx
//:
// \file

#include <sstream>
#include <iostream>
#include <string>
#include <cctype>
#include <utility>
#include "mbl_read_props.h"
#include <vsl/vsl_indent.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <mbl/mbl_parse_block.h>
#include <mbl/mbl_exception.h>


// Return the contents for a given (required) property prop.
// prop is removed from the property list.
// \throws mbl_exception_missing_property if prop doesn't exist
std::string mbl_read_props_type::get_required_property(const std::string &prop)
{
  auto it = this->find(prop);
  if (it==this->end())
    mbl_exception_error(mbl_exception_missing_property(prop));
  std::string result = it->second;
  this->erase(it);
  return result;
}


// Return the contents for a given (optional) property prop.
// prop is removed from the property list.
// Returns empty string if prop doesn't exist.
std::string mbl_read_props_type::get_optional_property(const std::string &prop,
                                                      const std::string &def_value /*=""*/)
{
  std::string result(def_value);
  auto it = this->find(prop);
  if (it!=this->end())
  {
    result = it->second;
    this->erase(it);
  }
  return result;
}


void mbl_read_props_print(std::ostream &afs, mbl_read_props_type props)
{
  afs << vsl_indent() << "{\n";
  vsl_indent_inc(afs);
  for (auto & prop : props)
    afs << vsl_indent() << prop.first << ": " << prop.second << '\n';
  vsl_indent_dec(afs);
  afs << vsl_indent() << "}\n";
}


void mbl_read_props_print(std::ostream &afs, mbl_read_props_type props, unsigned max_chars)
{
  typedef std::map<std::string, std::string>::iterator ITER;
  afs << vsl_indent() << "{\n";
  vsl_indent_inc(afs);
  for (auto & prop : props)
  {
    afs << vsl_indent() << prop.first << ": " << prop.second.substr(0, max_chars) << '\n';
    if (max_chars < prop.second.size()) afs << vsl_indent() << "...\n";
  }
  vsl_indent_dec(afs);
  afs << vsl_indent() << "}\n";
}


static void strip_trailing_ws(std::string &s)
{
  int p=s.length()-1;
  while (p>0 && std::isspace(s[p])) --p;
  s.erase(p+1);
}

//: Read properties from a text stream.
// The function will terminate on an eof. If one of
// the opening lines contains an opening brace '{', then the function
// will also stop reading the stream after finding a line containing
// a closing brace '}'
//
// There should be one property per line, with a colon ':' after
// each property label. The remainder of that line is the property value.
// If the next line begins with a brace, the following text up to matching
// braces is included in the property value.
// Each property label should not contain
// any whitespace.
mbl_read_props_type mbl_read_props(std::istream &afs)
{
  if (!afs) return mbl_read_props_type();

  std::string label, str1;

  while ( afs>>std::ws, !afs.eof() )
  {
    afs >> label;
    if (label.substr(0,2) =="//")
    {
      // Comment line, so read to end
      std::getline( afs, str1 );
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

  if ( label.empty() )
  {
    afs >> std::ws;

    // Several tests with Borland 5.5.1 fail because this next
    // statement 'afs >> label;' moves past the '\n' char when the
    // next section of the stream looks like "//comment\n a: a".  With
    // Borland 5.5.1, after this statement, afs.peek() returns 32
    // (space), while other compilers it returns 10 ('\n').  Seems
    // like a Borland standard library problem.  -Fred Wheeler

    afs >> label;

    // std::cout << "debug label " << label << std::endl
    //          << "debug peek() " << afs.peek() << std::endl;
  }

  std::string last_label( label );

  do
  {
    if ( label.substr(0,2) =="//" )
    {
      // Comment line, so read to end
      std::getline(afs, str1);
    }
    else if ( need_closing_brace && label[0] == '}' )
    {
      // Strip rest of line
      return props;
    }
    else if ( !label.empty() )
    {
      if ( label.size() > 1 &&
           label[label.size() -1] == ':' )
      {
        label.erase( label.size() -1, 1 );
        afs >> std::ws;
        std::getline(afs, str1);

        if ( str1.substr(0,1) == "{" )
        {
          str1 = mbl_parse_block(afs, true);
        }

        strip_trailing_ws(str1);

        auto it = props.lower_bound(label);

        if (it != props.end() && it->first == label)
        {
          mbl_exception_warning(
            mbl_exception_read_props_parse_error( std::string(
              "Found second entry with label \"") + label + '"' ) );
          afs.clear(std::ios::badbit); // Set an unrecoverable IO error on stream
          return props;
        }


        props.insert(it, std::make_pair(label, str1));
        last_label = label;
      }
      else if ( label.substr(0,1) == "{" )
      {
        std::string block = mbl_parse_block( afs, true );
        if ( block.substr(0,2) != "{}" )
        {
          std::string prop = props[ last_label ];
          prop += "\n";
          prop += block;
          props[ last_label ] = prop;
        }
      }
      else
      {
        char c;
        afs >> std::ws;
        afs >> c;

        if (c != ':')
        {
          std::getline(afs, str1);
          // The next loop replaces any characters outside the ASCII range
          // 32-126 with their XML equivalent, e.g. a TAB with &#9;
          // This is necessary for the tests dashboard since otherwise the
          // the Dart server gives up on interpreting the XML file sent. - PVr
          for (int i=-1; i<256; ++i)
          {
            char c= i<0 ? '&' : char(i); std::string s(1,c); // first do '&'
            if (i>=32 && i<127 && c!='<')
              continue; // keep "normal" chars

            std::ostringstream os; os << "&#" << (i<0?int(c):i) << ';';
            std::string::size_type pos;

            while ((pos=str1.find(s)) != std::string::npos)
              str1.replace(pos,1,os.str());

            while ((pos=label.find(s)) != std::string::npos)
              label.replace(pos,1,os.str());
          }
          mbl_exception_warning(
            mbl_exception_read_props_parse_error( std::string(
              "Could not find colon ':' separator while reading line ")
              + label + " " + str1) );
          afs.clear(std::ios::badbit); // Set an unrecoverable IO error on stream
          return props;
        }
      }
    }

    afs >> std::ws >> label;
  }

  while ( !afs.eof() );

  if ( need_closing_brace && label != "}" )
  {
    mbl_exception_warning(
      mbl_exception_read_props_parse_error( std::string(
        "Unexpected end of file while "
        "looking for '}'. Last read string = \"")
        + label +'"') );
    afs.clear(std::ios::badbit); // Set an unrecoverable IO error on stream
  }

  return props;
}



//: Read properties from a text stream.
// The function will terminate on an eof. If one of
// the opening lines contains an opening brace '{', then the function
// will also stop reading the stream after finding a line containing
// a closing brace '}'
//
// Every property label ends in ":", and should not contain
// any whitespace.
// Differs from mbl_read_props(afs) in that all whitespace is treated
// as a separator.
// If there is a brace after the first string following the label,
// the following text up to matching
// braces is included in the property value.
// Each property label should not contain
// any whitespace.
mbl_read_props_type mbl_read_props_ws(std::istream &afs)
{
  if (!afs) return mbl_read_props_type();

  std::string label, str1;

  while ( afs>>std::ws, !afs.eof() )
  {
    afs >> label;
    if (label.substr(0,2) =="//")
    {
      // Comment line, so read to end
      std::getline( afs, str1 );
    }
    else break;
  }

  if (afs.eof()) return mbl_read_props_type();

  bool need_closing_brace = false;

  if (label[0] == '{')
  {
    need_closing_brace = true;
    label.erase(0,1);
  }

  mbl_read_props_type props;

  if ( label.empty() )
  {
    afs >> std::ws;

    // Several tests with Borland 5.5.1 fail because this next
    // statement 'afs >> label;' moves past the '\n' char when the
    // next section of the stream looks like "//comment\n a: a".  With
    // Borland 5.5.1, after this statement, afs.peek() returns 32
    // (space), while other compilers it returns 10 ('\n').  Seems
    // like a Borland standard library problem.  -Fred Wheeler

    afs >> label;

    // std::cout << "debug label " << label << std::endl
    //          << "debug peek() " << afs.peek() << std::endl;
  }

  std::string last_label( label );

  do
  {
    if ( label.substr(0,2) =="//" )
    {
      // Comment line, so read to end
      std::getline(afs, str1);
    }
    else if ( need_closing_brace && label[0] == '}' )
    {
      // Strip rest of line
      return props;
    }
    else if ( !label.empty() )
    {
      if ( label.size() > 1 &&
           label[label.size() -1] == ':' )
      {
        label.erase( label.size() -1, 1 );

        char brace;
        afs >> std::ws >> brace;

        if (brace == '{')
          str1 = mbl_parse_block(afs, true);
        else
        {
          afs.putback(brace);
          afs >> str1;
        }


        strip_trailing_ws(str1);

        auto it = props.lower_bound(label);

        if (it != props.end() && it->first == label)
        {
          mbl_exception_warning(
            mbl_exception_read_props_parse_error( std::string(
              "Found second entry with label \"") + label + '"' ) );
          afs.clear(std::ios::badbit); // Set an unrecoverable IO error on stream
          return props;
        }

        props.insert(it, std::make_pair(label, str1));

        last_label = label;
      }
      else if ( label.substr(0,1) == "{" )
      {
        std::string block = mbl_parse_block( afs, true );
        if ( block.substr(0,2) != "{}" )
        {
          std::string prop = props[ last_label ];
          prop += " ";
          prop += block;
          props[ last_label ] = prop;
        }
      }
      else
      {
        char c;
        afs >> std::ws;
        afs >> c;

        if (c != ':')
        {
          std::getline(afs, str1);
          // The next loop replaces any characters outside the ASCII range
          // 32-126 with their XML equivalent, e.g. a TAB with &#9;
          // This is necessary for the tests dashboard since otherwise the
          // the Dart server gives up on interpreting the XML file sent. - PVr
          for (int i=-1; i<256; ++i)
          {
            char c= i<0 ? '&' : char(i); std::string s(1,c); // first do '&'
            if (i>=32 && i<127 && c!='<')
              continue; // keep "normal" chars

            std::ostringstream os; os << "&#" << (i<0?int(c):i) << ';';
            std::string::size_type pos;

            while ((pos=str1.find(s)) != std::string::npos)
              str1.replace(pos,1,os.str());

            while ((pos=label.find(s)) != std::string::npos)
              label.replace(pos,1,os.str());
          }
          mbl_exception_warning(
            mbl_exception_read_props_parse_error( std::string(
              "Could not find colon ':' separator while reading line ")
              + label + " " + str1) );
          afs.clear(std::ios::badbit); // Set an unrecoverable IO error on stream
          return props;
        }
      }
    }

    afs >> std::ws >> label;
  }

  while ( !afs.eof() );

  if ( need_closing_brace && label != "}" )
  {
    mbl_exception_warning(
      mbl_exception_read_props_parse_error( std::string(
        "Unexpected end of file while "
        "looking for '}'. Last read string = \"")
        + label + '"') );
    afs.clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    return props;
  }

  return props;
}


//: merge two property sets.
// \param first_overrides
// properties in "a" will override identically named properties in "b"
mbl_read_props_type mbl_read_props_merge(const mbl_read_props_type& a,
                                         const mbl_read_props_type& b,
                                         bool first_overrides/*=true*/)
{
  mbl_read_props_type output;

  auto a_it = a.begin();
  auto b_it = b.begin();


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

//: Throw error if there are any keys in props that aren't in ignore.
// \throw mbl_exception_unused_props
void mbl_read_props_look_for_unused_props(
  const std::string & function_name,
  const mbl_read_props_type &props,
  const mbl_read_props_type &ignore)
{
  mbl_read_props_type p2(props);

  // Remove ignoreable properties
  for (const auto & it : ignore)
    p2.erase(it.first);

  if (!p2.empty())
  {

    std::ostringstream ss;
    mbl_read_props_print(ss, p2, 1000);
    mbl_exception_error(mbl_exception_unused_props(function_name, ss.str()));
  }
}
