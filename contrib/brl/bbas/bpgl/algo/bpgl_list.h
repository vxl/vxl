// This is bbas/bpgl/algo/bpgl_list.h
#ifndef bpgl_list_h_
#define bpgl_list_h_
//:
// \file
// \brief Write lists of cameras, homographies, fundamental matrices in a standard format
//
// Template classes for writing lists of cameras, homographies, fundamental
// matrices, etc in a standard format.  This will eventually be replaced by XML
// read/writers.
// \author Thomas Pollard
// \date March 05, 2005

#include <iostream>
#include <string>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class T>
bool bpgl_read_list(
  std::vector<T>& list,
  std::string file );

template <class T>
bool bpgl_write_list(
  const std::vector<T>& list,
  std::string file );

#endif // bpgl_list_h_
