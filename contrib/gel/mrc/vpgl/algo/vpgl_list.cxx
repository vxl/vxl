// This is gel/mrc/vpgl/algo/vpgl_list.cxx
#ifndef vpgl_list_cxx_
#define vpgl_list_cxx_
//:
// \file

#include <vcl_iostream.h>

#include "vpgl_list.h"


//-------------------------------------------
template <class T>
bool 
vpgl_write_list<T>( 
  const vcl_vector<T>& list, 
  vcl_string file )
{
  vcl_ofstream ofp( file.c_str() );
  if( !ofp.good() )
    return false;
  for( int i = 0; i < list.size(); i++ )
    ofp << "Frame " << i << '\n' << list[i].get_matrix() << '\n';
  return true;
}


//-------------------------------------------
template <class T>
bool 
vpgl_read_list<T>( 
  vcl_vector<T>& list, 
  vcl_string file )
{
  vcl_ifstream ifp( file.c_str() );
  if( !ifp.good() )
    return false;

  char line_buffer[256];
  while( ifp.eof() == 0 ){
    int S = list.size();
    char nc = ifp.peek();
    if( nc == '-' || nc == '0' || nc == '1' || nc == '2' || nc == '3' || nc == '4' || 
        nc == '5' || nc == '6' || nc == '7' || nc == '8' || nc == '9' ){
      T new_object;
      ifp >> new_object;
      list.push_back( new_object );
    }
    else
      ifp.getline(line_buffer,256);
  }
  return true;
}

#endif // vpgl_list_cxx_

