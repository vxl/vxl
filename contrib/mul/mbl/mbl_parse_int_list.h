// This is mul/mbl/mbl_parse_int_list.h
#ifndef mbl_parse_int_list_h_
#define mbl_parse_int_list_h_
//:
// \file
// \author Ian Scott
// \date  7-Aug-2007
// \brief Load an int_list of PODs/objects from a config file.

#include <istream>
#include <iostream>
#include <cctype>
#include <mbl/mbl_exception.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Read a list of integers from a stream.
// This integer list should be space-separated.
// Lists of the form "{ 1 2 5 : 10 }" a la matlab are accepted. Note that "a : b"
// will include b rather than following normal C++ convention.
// Failed parsing will cause an exception to be thrown, and the stream's fail bit to be set
//
// \throws mbl_exception_parse_error if unrecoverable parse error.
//
// \verbatim
// Example:
// std::vector<unsigned> v;
// mbl_parse_int_list(std::cin, std::back_inserter(v), unsigned());
// \endverbatim
template <class ITER, class T>
inline void mbl_parse_int_list(std::istream &afs, ITER insert_iter, T)
{
  if (!afs) return;

  char c;
  afs >> std::ws >> c;
  if (!afs) return;

  bool openbrace = false;

  if (c == '{')
    openbrace = true;
  else
    afs.putback(c);


  while (true)
  {
    char c;
    afs >> std::ws >> c;
    if (afs.eof() || (c =='}' && openbrace))
    {
      afs.clear();
      return;
    }
    else if (!afs)
    {
      afs.clear(std::ios::failbit);
      mbl_exception_warning(mbl_exception_parse_error(
        "mbl_parse_int_list: Unknown stream failure") );
      return;
    }

    else if (! (std::isdigit(c) || c== '-') )
    {
      afs.clear(std::ios::failbit);
      mbl_exception_warning(mbl_exception_parse_error(
        std::string("mbl_parse_int_list: unexpected character '") + c + "'") );
      return;
    }


    afs.putback(c);
    T current;
    afs >> current >> std::ws >> c;
    if (afs.eof() && openbrace)
    {
      afs.clear(std::ios::failbit);
      mbl_exception_warning(mbl_exception_parse_error(
        "mbl_parse_int_list: unexpected EOF" ) );
      return;
    }

    if (afs.eof() || (c =='}' && openbrace) )
    {
      *insert_iter++ = current;
      afs.clear();
      return;
    }
    else if (!afs)
    {
      mbl_exception_warning(mbl_exception_parse_error(
        "mbl_parse_int_list: non-integer detected") );
      return;
    }
    else if (std::isdigit(c) || c== '-')
    {
      afs.putback(c);
      *insert_iter++ = current;
      continue;
    }
    else if (c!=':')
    {
      afs.clear(std::ios::failbit);
      mbl_exception_warning(mbl_exception_parse_error(
        std::string("mbl_parse_int_list: unexpected character '") + c + "'") );
      return;
    }
    else
    {
      T last;
      afs >> std::ws >> last;
      if (afs.eof())
      {
        mbl_exception_warning(mbl_exception_parse_error(
          "mbl_parse_int_list: unexpected EOF") );
        return;
      }
      if (!afs)
      {
        mbl_exception_warning(mbl_exception_parse_error(
          "mbl_parse_int_list: Unknown stream error") );
        return;
      }

      if (current > last)
      {
        // Count down to last
        // Note: Do in two steps to avoid -- problems when last==0
        while (current > last)
          *insert_iter++ = current--;

        *insert_iter++ = last;
#if 0
        afs.clear(std::ios::failbit);
        mbl_exception_warning(mbl_exception_parse_error(
          std::string("mbl_parse_int_list: unbounded sequence") ));
        return;
#endif
      }
      else
      {
        // Count up to last
        while (current <= last)
          *insert_iter++ = current++;
      }
    }
  }
}

#endif // mbl_parse_int_list_h_
