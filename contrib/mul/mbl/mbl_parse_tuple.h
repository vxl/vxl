// This is mul/mbl/mbl_parse_sequence.h
#ifndef mbl_parse_sequence_h_
#define mbl_parse_sequence_h_
//:
// \file
// \author Ian Scott
// \date  6-Feb-2008
// \brief Convenience function a tuple of PODs from a config file.

#include <vcl_algorithm.h>
#include <vcl_istream.h>
#include <vcl_iterator.h>
#include <mbl/mbl_exception.h>

//: Read a 2-tuple of PODs from a config file.
// This function will read through a stream, and store the text found to a string.
// The function reads 2 elements. If there was an openning brace it will also consume the closing brace.
// Other conditions will cause an exception to be thrown, and the stream's fail bit to be set
//
// \throws mbl_exception_parse_error if unrecoverable parse error.
//
// \verbatim
// Example:
// vcl_istringstream ss("{ 1.0 4 }");
// float a;
// int b;
// mbl_parse_tuple(ss, a, b)
// \endverbatim
template <class T, class U>
inline void mbl_parse_tuple(vcl_istream &afs, T& a, U& b)
{

  if (!afs) return;
  char brace1, brace2;
  afs >> vcl_ws >> brace1;
  if (afs.eof())
    throw mbl_exception_parse_error("mbl_parse_tuple failed unexpected eof");
  if ( brace1 == '{')
  {
    afs >> vcl_ws >> a >> vcl_ws >> b >> vcl_ws >> brace2;
    if (!afs)
      throw mbl_exception_parse_error("mbl_parse_tuple failed with stream error");
    if (brace2 != '}')
    {
      afs.putback(brace2);
      afs.clear(vcl_ios::failbit); // Set a recoverable IO error on stream

      throw mbl_exception_parse_error("mbl_parse_tuple failed to find closing brace");
    }
  }
  else
  {
    afs.putback(brace1);
    afs >> vcl_ws >> a >> vcl_ws >> b;
    if (!afs)
      throw mbl_exception_parse_error("mbl_parse_tuple failed with stream error");
  }
}

//: Read a 3-tuple of PODs from a config file.
// This function will read through a stream, and store the text found to a string.
// The function reads 3 elements. If there was an openning brace it will also consume the closing brace.
// Other conditions will cause an exception to be thrown, and the stream's fail bit to be set
//
// \throws mbl_exception_parse_error if unrecoverable parse error.
//
// \verbatim
// Example:
// vcl_istringstream ss("{ 1.0 -5 4 }");
// float a;
// int b;
// unsigned c;
// mbl_parse_tuple(ss, a, b, c)
// \endverbatim
template <class T, class U, class V>
inline void mbl_parse_tuple(vcl_istream &afs, T& a, U& b, V& c)
{

  if (!afs) return;
  char brace1, brace2;
  afs >> vcl_ws >> brace1;
  if (afs.eof())
    throw mbl_exception_parse_error("mbl_parse_tuple failed unexpected eof");
  if ( brace1 == '{')
  {
    afs >> vcl_ws >> a >> vcl_ws >> b >> vcl_ws >> c >> vcl_ws >> brace2;
    if (!afs)
      throw mbl_exception_parse_error("mbl_parse_tuple failed with stream error");
    if (brace2 != '}')
    {
      afs.putback(brace2);
      afs.clear(vcl_ios::failbit); // Set a recoverable IO error on stream

      throw mbl_exception_parse_error("mbl_parse_tuple failed to find closing brace");
    }
  }
  else
  {
    afs.putback(brace1);
    afs >> vcl_ws >> a >> vcl_ws >> b >> vcl_ws >> c;
    if (!afs)
      throw mbl_exception_parse_error("mbl_parse_tuple failed with stream error");
  }
}



//: Read a 4-tuple of PODs from a config file.
// This function will read through a stream, and store the text found to a string.
// The function reads 4 elements. If there was an openning brace it will also consume the closing brace.
// Other conditions will cause an exception to be thrown, and the stream's fail bit to be set
//
// \throws mbl_exception_parse_error if unrecoverable parse error.
//
// \verbatim
// Example:
// vcl_istringstream ss("{ 1.0 -5 4 a }");
// float a;
// int b;
// unsigned c;
// char d;
// mbl_parse_tuple(ss, a, b, c)
// \endverbatim
template <class T, class U, class V, class W>
inline void mbl_parse_tuple(vcl_istream &afs, T& a, U& b, V& c, W& d)
{

  if (!afs) return;
  char brace1, brace2;
  afs >> vcl_ws >> brace1;
  if (afs.eof())
    throw mbl_exception_parse_error("mbl_parse_tuple failed unexpected eof");
  if ( brace1 == '{')
  {
    afs >> vcl_ws >> a >> vcl_ws >> b >> vcl_ws >> c >> vcl_ws >> d >> vcl_ws >> brace2;
    if (!afs)
      throw mbl_exception_parse_error("mbl_parse_tuple failed with stream error");
    if (brace2 != '}')
    {
      afs.putback(brace2);
      afs.clear(vcl_ios::failbit); // Set a recoverable IO error on stream

      throw mbl_exception_parse_error("mbl_parse_tuple failed to find closing brace");
    }
  }
  else
  {
    afs.putback(brace1);
    afs >> vcl_ws >> a >> vcl_ws >> b >> vcl_ws >> c >> vcl_ws >> d;
    if (!afs)
      throw mbl_exception_parse_error("mbl_parse_tuple failed with stream error");
  }
}
#endif // mbl_parse_sequence_h_
