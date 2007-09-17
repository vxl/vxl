// This is mul/mbl/mbl_parse_sequence.h
#ifndef mbl_parse_sequence_h_
#define mbl_parse_sequence_h_
//:
// \file
// \author Ian Scott
// \date  7-Aug-2007
// \brief Load a sequence of PODs/objects from a config file.

#include <vcl_algorithm.h>
#include <vcl_istream.h>
#include <vcl_iterator.h>
#include <mbl/mbl_exception.h>

//: Read a sequence of PODs from a stream.
// This function will read through a stream, and store the text found to a string.
// The function terminates correctly when it finds a matched closing brace,
// Alternatively, if there was no openning brace, it will terminate at the end of a stream.
// Other conditions will cause an exception to be thrown, and the stream's fail bit to be set
//
// \throws mbl_exception_parse_error if unrecoverable parse error.
//
// \verbatim
// Example:
// vcl_vector<unsigned> v;
// mbl_parse_sequence(vcl_cin, vcl_back_inserter(v), unsigned());
// \endverbatim
template <class ITER, class T>
void mbl_parse_sequence(vcl_istream &afs, ITER insert_iter, T dummy)
{
  // Can't use iterator_traits<ITER>::value_type to infer T,
  // because output_iterators may not have a useful value_type
  // and are defined by the standard to have value_type void,
  // See http://www.adras.com/Why-no-std-back-insert-iterator-value-type.t2639-153.html

  if (!afs) return;
  char brace1, brace2;
  afs >> vcl_ws >> brace1;
  if (afs.eof()) return;

  if ( brace1 == '{')
  {
    vcl_copy(vcl_istream_iterator<T>(afs),
      vcl_istream_iterator<T>(), insert_iter);

    if (afs.fail())
      afs.clear();

    afs >> vcl_ws >> brace2;
    if (!afs || brace2 != '}')
    {
      afs.putback(brace2);
      afs.clear(vcl_ios::failbit); // Set a recoverable IO error on stream
      throw mbl_exception_parse_error(
        "mbl_parse_sequence failed to find closing brace.");
    }
  }
  else
  {
    afs.putback(brace1);

    vcl_copy(vcl_istream_iterator<T>(afs),
      vcl_istream_iterator<T>(), insert_iter);

    if (afs.fail())
      afs.clear();

    char dummy;
    afs >> dummy;
    if (!(!afs))
    {
      afs.putback(dummy);
      afs.clear(vcl_ios::failbit);

      throw mbl_exception_parse_error(
        "mbl_parse_sequence failed to find EOF.");
    }
    afs.clear(vcl_ios::eofbit);
  }
}

#endif // mbl_parse_sequence_h_
