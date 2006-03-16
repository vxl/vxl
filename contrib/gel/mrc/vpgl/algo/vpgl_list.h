// This is gel/mrc/vpgl/algo/vpgl_list.h
#ifndef vpgl_list_h_
#define vpgl_list_h_
//:
// \file
// \brief Write lists of cameras, homographies, fundamental matrices in a standard format
//
// Template classes for writing lists of cameras, homographies, fundamental
// matrices, etc in a standard format.  This will eventually be replaced by XML
// read/writers.
// \author Thomas Pollard
// \date 03/05/05
//
// \verbatim
//
// \endverbatim

#include <vcl_iosfwd.h>

template <class T>
bool vpgl_read_list(
  vcl_vector<T>& list,
  vcl_string file );

template <class T>
bool vpgl_write_list(
  const vcl_vector<T>& list,
  vcl_string file );

#endif // vpgl_list_h_
