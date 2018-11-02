// This is mul/mbl/mbl_parse_sequence.h
#ifndef mbl_parse_sequence_h_
#define mbl_parse_sequence_h_
//:
// \file
// \author Ian Scott
// \date  7-Aug-2007
// \brief Load a sequence of PODs/objects from a config file.

#include <iostream>
#include <algorithm>
#include <istream>
#include <iterator>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <mbl/mbl_exception.h>

//: Read a sequence of PODs from a stream.
// This function will read through a stream, and store the text found to a string.
// The function terminates correctly when it finds a matched closing brace,
// Alternatively, if there was no opening brace, it will terminate at the end of a stream.
// Other conditions will cause an exception to be thrown, and the stream's fail bit to be set
//
// \throws mbl_exception_parse_error if unrecoverable parse error.
//
// \verbatim
// Example:
// std::vector<unsigned> v;
// mbl_parse_sequence(std::cin, std::back_inserter(v), unsigned());
// \endverbatim
template <class ITER, class T>
inline void mbl_parse_sequence(std::istream &afs, ITER insert_iter, T /*dummy*/)
{
  // Can't use iterator_traits<ITER>::value_type to infer T,
  // because output_iterators may not have a useful value_type
  // and are defined by the standard to have value_type void,
  // See http://www.adras.com/Why-no-std-back-insert-iterator-value-type.t2639-153.html

  if (!afs) return;
  char brace1, brace2;
  afs >> std::ws >> brace1;
  if (afs.eof()) return;

  if ( brace1 == '{')
  {
    std::copy(std::istream_iterator<T>(afs),
             std::istream_iterator<T>(), insert_iter);

    if (afs.fail())
      afs.clear();

    afs >> std::ws >> brace2;
    if (!afs || brace2 != '}')
    {
      afs.putback(brace2);
      afs.clear(std::ios::failbit); // Set a recoverable IO error on stream
      throw mbl_exception_parse_error(
        "mbl_parse_sequence failed to find closing brace.");
    }
  }
  else
  {
    afs.putback(brace1);

    std::copy(std::istream_iterator<T>(afs),
             std::istream_iterator<T>(), insert_iter);

    if (afs.fail())
      afs.clear();

    char c;
    afs >> c;
    if (!(!afs))
    {
      afs.putback(c);
      afs.clear(std::ios::failbit);

      throw mbl_exception_parse_error(
        "mbl_parse_sequence failed to find EOF.");
    }
    afs.clear(std::ios::eofbit);
  }
}

#endif // mbl_parse_sequence_h_
