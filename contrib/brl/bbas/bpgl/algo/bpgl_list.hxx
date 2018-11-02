// This is bbas/bpgl/algo/bpgl_list.hxx
#ifndef bpgl_list_hxx_
#define bpgl_list_hxx_
//:
// \file
#include <iostream>
#include <fstream>
#include "bpgl_list.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//-------------------------------------------
template <class T>
bool
bpgl_write_list(
  const std::vector<T>& list,
  std::string file )
{
  std::ofstream ofp( file.c_str() );
  if ( !ofp.good() )
    return false;
  for ( unsigned int i = 0; i < list.size(); ++i )
    ofp << "Frame " << i << '\n' << list[i].get_matrix() << '\n';
  return true;
}


//-------------------------------------------
template <class T>
bool
bpgl_read_list(
  std::vector<T>& list,
  std::string file )
{
  std::ifstream ifp( file.c_str() );
  if ( !ifp.good() )
    return false;

  char line_buffer[256];
  while ( ifp.eof() == 0 ) {
    char nc = (char)ifp.peek();
    if ( nc == '-' || (nc >= '0' && nc <= '9') ) {
      T new_object;
      ifp >> new_object;
      list.push_back( new_object );
    }
    else
      ifp.getline(line_buffer,256);
  }
  return true;
}

#define BPGL_LIST_INSTANTIATE(T) \
template bool bpgl_read_list(std::vector<T >&, std::string ); \
template bool bpgl_write_list(const std::vector<T >&, std::string )

#endif // bpgl_list_hxx_
