// This is mul/mbl/mbl_read_multi_props.cxx
#include <sstream>
#include <iostream>
#include <string>
#include <cctype>
#include <utility>
#include <iterator>
#include "mbl_read_multi_props.h"
//:
// \file

#include <vsl/vsl_indent.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <mbl/mbl_parse_block.h>
#include <mbl/mbl_exception.h>

void mbl_read_multi_props_print(std::ostream &afs, mbl_read_multi_props_type props)
{
  afs << vsl_indent() << "{\n";
  vsl_indent_inc(afs);
  for (auto & prop : props)
    afs << vsl_indent() << prop.first << ": " << prop.second << '\n';
  vsl_indent_dec(afs);
  afs << vsl_indent() << "}\n";
}

void mbl_read_multi_props_print(std::ostream &afs, mbl_read_multi_props_type props, unsigned max_chars)
{
  typedef mbl_read_multi_props_type::iterator ITER;
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
// Every property label ends in ":", and should not contain
// any whitespace.
// Differs from mbl_read_multi_props(afs) in that all whitespace is treated
// as a separator.
// If there is a brace after the first string following the label,
// the following text up to matching
// braces is included in the property value.
// Each property label should not contain
// any whitespace.
mbl_read_multi_props_type mbl_read_multi_props_ws(std::istream &afs)
{
  if (!afs) return mbl_read_multi_props_type();

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

  mbl_read_multi_props_type props;

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

  typedef mbl_read_multi_props_type::iterator ITER;
  std::string last_label( label );
  auto last_label_iter = props.end();

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
        afs >> std::ws >> str1;

        if ( str1.substr(0,1) == "{" )
          str1 = mbl_parse_block(afs, true);

        strip_trailing_ws(str1);
        last_label_iter = props.insert(std::make_pair(label, str1));
        last_label = label;
      }
      else if ( label.substr(0,1) == "{" )
      {
        std::string block = mbl_parse_block( afs, true );
        if ( block.substr(0,2) != "{}" )
        {
          if (last_label_iter == props.end())
          {
            props.insert(std::make_pair(last_label, str1));
            last_label_iter = props.insert(std::make_pair(last_label, std::string(" ")+block));
          }
          else
          {
            last_label_iter->second += " ";
            last_label_iter->second += block;
          }
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
          return props;
        }
      }
    }

    afs >> std::ws >> label;
  }

  while ( !afs.eof() );

  if ( need_closing_brace && label != "}" )
    mbl_exception_warning(
      mbl_exception_read_props_parse_error( std::string(
        "Unexpected end of file while "
        "looking for '}'. Last read string = \"")
        + label + '"') );

  return props;
}


//: merge two property sets.
// \param first_overrides
// properties in "a" will override identically named properties in "b"
mbl_read_multi_props_type mbl_read_multi_props_merge(const mbl_read_multi_props_type& a,
                                         const mbl_read_multi_props_type& b,
                                         bool first_overrides/*=true*/)
{
  mbl_read_multi_props_type output;

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
void mbl_read_multi_props_look_for_unused_props(
  const std::string & function_name,
  const mbl_read_multi_props_type &props,
  const mbl_read_multi_props_type &ignore)
{
  mbl_read_multi_props_type p2(props);

  // Remove ignoreable properties
  for (const auto & it : ignore)
    p2.erase(it.first);

  if (!p2.empty())
  {
    std::ostringstream ss;
    mbl_read_multi_props_print(ss, p2, 1000);
    mbl_exception_error(mbl_exception_unused_props(function_name, ss.str()));
  }
}


//: Return a single expected value of the given property \a label.
// The matching entry is removed from the property list.
// \throws mbl_exception_missing_property if \a label doesn't exist.
// \throws mbl_exception_read_props_parse_error if there are two or more values of \a label.
std::string mbl_read_multi_props_type::get_required_property(const std::string& label)
{
  std::pair<mbl_read_multi_props_type::iterator, mbl_read_multi_props_type::iterator>
    its = this->equal_range(label);
  if (its.first==its.second)
    mbl_exception_error(mbl_exception_missing_property(label));
  else if (std::distance(its.first, its.second) > 1)
    mbl_exception_error(mbl_exception_read_props_parse_error(
      std::string("Property label \"") + label + "\" occurs more than once.") );

  std::string s = its.first->second;
  this->erase(its.first);
  return s;
}


//: Return a single value of the given property \a label.
// The matching entry is removed from the property list.
// returns empty string or \a default_prop if \a label doesn't exist.
// \throws mbl_exception_read_props_parse_error if there are two or more values of \a label.
std::string mbl_read_multi_props_type::get_optional_property(
  const std::string& label, const std::string& default_prop /*=""*/)
{
  std::pair<mbl_read_multi_props_type::iterator, mbl_read_multi_props_type::iterator>
    its = this->equal_range(label);
  if (its.first==its.second) return default_prop;
  else if (std::distance(its.first, its.second) > 1)
    mbl_exception_error(mbl_exception_read_props_parse_error(
      std::string("Property label \"") + label + "\" occurs more than once.") );

  std::string s = its.first->second;
  this->erase(its.first);
  return s;
}


// Return a vector of all values for a given property label.
// Throw exception if label doesn't occur at least once.
void mbl_read_multi_props_type::get_required_properties(
  const std::string& label,
  std::vector<std::string>& values,
  const unsigned nmax/*=-1*/, //=max<unsigned>
  const unsigned nmin/*=1*/)
{
  values.clear();

  auto beg = this->lower_bound(label);
  auto fin = this->upper_bound(label);
  if (beg==fin)
    mbl_exception_error(mbl_exception_missing_property(label));
  for (auto it=beg; it!=fin; ++it)
  {
    values.push_back(it->second);
  }

  const unsigned nval = values.size();
  if (nval<nmin)
  {
    const std::string msg = "property label \"" + label + "\" occurs too few times.";
    mbl_exception_error(mbl_exception_read_props_parse_error(msg));
  }
  if (nval>nmax)
  {
    const std::string msg = "property label \"" + label + "\" occurs too many times.";
    mbl_exception_error(mbl_exception_read_props_parse_error(msg));
  }

  this->erase(beg, fin);
}


// Return a vector of all values for a given property label.
// Vector is empty if label doesn't occur at least once.
void mbl_read_multi_props_type::get_optional_properties(
  const std::string& label,
  std::vector<std::string>& values,
  const unsigned nmax/*=-1*/) //=max<unsigned>
{
  values.clear();

  auto beg = this->lower_bound(label);
  auto fin = this->upper_bound(label);

  for (auto it=beg; it!=fin; ++it)
  {
    values.push_back(it->second);
  }

  const unsigned nval = values.size();
  if (nval>nmax)
  {
    const std::string msg = "property label \"" + label + "\" occurs too many times.";
    mbl_exception_error(mbl_exception_read_props_parse_error(msg));
  }

  this->erase(beg, fin);
}
