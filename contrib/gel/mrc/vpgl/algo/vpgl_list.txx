// This is gel/mrc/vpgl/algo/vpgl_list.txx
#ifndef vpgl_list_txx_
#define vpgl_list_txx_
//:
// \file
#include "vpgl_list.h"

#include <vcl_iostream.h>

//-------------------------------------------
template <class T>
bool 
vpgl_write_list( 
  const vcl_vector<T>& list, 
  vcl_string file )
{
  vcl_ofstream ofp( file.c_str() );
  if ( !ofp.good() )
    return false;
  for ( unsigned int i = 0; i < list.size(); ++i )
    ofp << "Frame " << i << '\n' << list[i].get_matrix() << '\n';
  return true;
}


//-------------------------------------------
template <class T>
bool 
vpgl_read_list( 
  vcl_vector<T>& list, 
  vcl_string file )
{
  vcl_ifstream ifp( file.c_str() );
  if ( !ifp.good() )
    return false;

  char line_buffer[256];
  while ( ifp.eof() == 0 ){
    unsigned int S = list.size();
    char nc = ifp.peek();
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

#define VPGL_LIST_INSTANTIATE(T) \
template bool vpgl_read_list(vcl_vector<T >&, vcl_string ); \
template bool vpgl_write_list(const vcl_vector<T >&, vcl_string )

#endif // vpgl_list_txx_
