// This is mul/mbl/mbl_parse_tuple.h
#ifndef mbl_parse_tuple_h_
#define mbl_parse_tuple_h_
//:
// \file
// \author Ian Scott
// \date  6-Feb-2008
// \brief Convenience function a tuple of PODs from a config file.

#include <iostream>
#include <istream>
#include <sstream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <mbl/mbl_exception.h>

//: Read a 2-tuple of PODs from a config file.
// This function will read through a stream, and store the text found to a string.
// The function reads 2 elements. If there was an opening brace it will also consume the closing brace.
// Other conditions will cause an exception to be thrown, and the stream's fail bit to be set
//
// \throws mbl_exception_parse_error if unrecoverable parse error.
//
// Example:
// \code
//   std::istringstream ss("{ 1.0 4 }");
//   float a;
//   int b;
//   mbl_parse_tuple(ss, a, b)
// \endcode
//
template <class T, class U>
inline void mbl_parse_tuple(std::istream &afs, T& a, U& b)
{
  if (!afs) return;
  char brace1, brace2;
  afs >> std::ws >> brace1;
  if (afs.eof())
    throw mbl_exception_parse_error("mbl_parse_tuple failed unexpected eof");
  if ( brace1 == '{')
  {
    afs >> std::ws >> a >> std::ws >> b >> std::ws >> brace2;
    if (!afs)
      throw mbl_exception_parse_error("mbl_parse_tuple failed with stream error");
    if (brace2 != '}')
    {
      afs.putback(brace2);
      afs.clear(std::ios::failbit); // Set a recoverable IO error on stream

      throw mbl_exception_parse_error("mbl_parse_tuple failed to find closing brace");
    }
  }
  else
  {
    afs.putback(brace1);
    afs >> std::ws >> a >> std::ws >> b;
    if (!afs)
      throw mbl_exception_parse_error("mbl_parse_tuple failed with stream error");
  }
}

//: Read a 3-tuple of PODs from a config file.
// This function will read through a stream, and store the text found to a string.
// The function reads 3 elements. If there was an opening brace it will also consume the closing brace.
// Other conditions will cause an exception to be thrown, and the stream's fail bit to be set
//
// \throws mbl_exception_parse_error if unrecoverable parse error.
//
// \verbatim
// Example:
// std::istringstream ss("{ 1.0 -5 4 }");
// float a;
// int b;
// unsigned c;
// mbl_parse_tuple(ss, a, b, c)
// \endverbatim
template <class T, class U, class V>
inline void mbl_parse_tuple(std::istream &afs, T& a, U& b, V& c)
{
  if (!afs) return;
  char brace1, brace2;
  afs >> std::ws >> brace1;
  if (afs.eof())
    throw mbl_exception_parse_error("mbl_parse_tuple failed unexpected eof");
  if ( brace1 == '{')
  {
    afs >> std::ws >> a >> std::ws >> b >> std::ws >> c >> std::ws >> brace2;
    if (!afs)
      throw mbl_exception_parse_error("mbl_parse_tuple failed with stream error");
    if (brace2 != '}')
    {
      afs.putback(brace2);
      afs.clear(std::ios::failbit); // Set a recoverable IO error on stream

      throw mbl_exception_parse_error("mbl_parse_tuple failed to find closing brace");
    }
  }
  else
  {
    afs.putback(brace1);
    afs >> std::ws >> a >> std::ws >> b >> std::ws >> c;
    if (!afs)
      throw mbl_exception_parse_error("mbl_parse_tuple failed with stream error");
  }
}


//: Read a 4-tuple of PODs from a config file.
// This function will read through a stream, and store the text found to a string.
// The function reads 4 elements. If there was an opening brace it will also consume the closing brace.
// Other conditions will cause an exception to be thrown, and the stream's fail bit to be set
//
// \throws mbl_exception_parse_error if unrecoverable parse error.
//
// \verbatim
// Example:
// std::istringstream ss("{ 1.0 -5 4 a }");
// float a;
// int b;
// unsigned c;
// char d;
// mbl_parse_tuple(ss, a, b, c, d)
// \endverbatim
template <class T, class U, class V, class W>
inline void mbl_parse_tuple(std::istream &afs, T& a, U& b, V& c, W& d)
{
  if (!afs) return;
  char brace1, brace2;
  afs >> std::ws >> brace1;
  if (afs.eof())
    throw mbl_exception_parse_error("mbl_parse_tuple failed unexpected eof");
  if ( brace1 == '{')
  {
    afs >> std::ws >> a >> std::ws >> b >> std::ws >> c >> std::ws >> d >> std::ws >> brace2;
    if (!afs)
      throw mbl_exception_parse_error("mbl_parse_tuple failed with stream error");
    if (brace2 != '}')
    {
      afs.putback(brace2);
      afs.clear(std::ios::failbit); // Set a recoverable IO error on stream

      throw mbl_exception_parse_error("mbl_parse_tuple failed to find closing brace");
    }
  }
  else
  {
    afs.putback(brace1);
    afs >> std::ws >> a >> std::ws >> b >> std::ws >> c >> std::ws >> d;
    if (!afs)
      throw mbl_exception_parse_error("mbl_parse_tuple failed with stream error");
  }
}


//: Read a 5-tuple of PODs from a config file.
// This function will read through a stream, and store the text found to a string.
// The function reads 5 elements. If there was an opening brace it will also consume the closing brace.
// Other conditions will cause an exception to be thrown, and the stream's fail bit to be set
//
// \throws mbl_exception_parse_error if unrecoverable parse error.
//
// \verbatim
// Example:
// std::istringstream ss("{ 1.0 -5 4 k l }");
// float a;
// int b;
// unsigned c;
// char d, e;
// mbl_parse_tuple(ss, a, b, c, d, e)
// \endverbatim
template <class T, class U, class V, class W, class X>
inline void mbl_parse_tuple(std::istream &afs, T& a, U& b, V& c, W& d, X& e)
{
  if (!afs) return;
  char brace1, brace2;
  afs >> std::ws >> brace1;
  if (afs.eof())
    throw mbl_exception_parse_error("mbl_parse_tuple failed unexpected eof");
  if ( brace1 == '{')
  {
    afs >> std::ws >> a >> std::ws >> b >> std::ws >> c >> std::ws >> d >> std::ws >> e >> std::ws >> brace2;
    if (!afs)
      throw mbl_exception_parse_error("mbl_parse_tuple failed with stream error");
    if (brace2 != '}')
    {
      afs.putback(brace2);
      afs.clear(std::ios::failbit); // Set a recoverable IO error on stream

      throw mbl_exception_parse_error("mbl_parse_tuple failed to find closing brace");
    }
  }
  else
  {
    afs.putback(brace1);
    afs >> std::ws >> a >> std::ws >> b >> std::ws >> c >> std::ws >> d >> std::ws >> e;
    if (!afs)
      throw mbl_exception_parse_error("mbl_parse_tuple failed with stream error");
  }
}


//: Read a 6-tuple of PODs from a config file.
// This function will read through a stream, and store the text found to a string.
// The function reads 6 elements. If there was an opening brace it will also consume the closing brace.
// Other conditions will cause an exception to be thrown, and the stream's fail bit to be set
//
// \throws mbl_exception_parse_error if unrecoverable parse error.
//
// \verbatim
// Example:
// std::istringstream ss("{ 1.0 -3.4 -5 4 k l }");
// float a;
// double b;
// int c;
// unsigned d;
// char e, f;
// mbl_parse_tuple(ss, a, b, c, d, e, f)
// \endverbatim
template <class T, class U, class V, class W, class X, class Y>
inline void mbl_parse_tuple(std::istream &afs, T& a, U& b, V& c, W& d, X& e, Y& f)
{
  if (!afs) return;
  char brace1, brace2;
  afs >> std::ws >> brace1;
  if (afs.eof())
    throw mbl_exception_parse_error("mbl_parse_tuple failed unexpected eof");
  if ( brace1 == '{')
  {
    afs >> std::ws >> a >> std::ws >> b >> std::ws >> c >> std::ws >> d >> std::ws >>
      e >> std::ws >> f >> std::ws >> brace2;
    if (!afs)
      throw mbl_exception_parse_error("mbl_parse_tuple failed with stream error");
    if (brace2 != '}')
    {
      afs.putback(brace2);
      afs.clear(std::ios::failbit); // Set a recoverable IO error on stream

      throw mbl_exception_parse_error("mbl_parse_tuple failed to find closing brace");
    }
  }
  else
  {
    afs.putback(brace1);
    afs >> std::ws >> a >> std::ws >> b >> std::ws >> c >> std::ws >> d >>
      std::ws >> e >> std::ws >> f;
    if (!afs)
      throw mbl_exception_parse_error("mbl_parse_tuple failed with stream error");
  }
}
#endif // mbl_parse_tuple_h_
