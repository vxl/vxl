#ifndef vbl_binary_h_
#define vbl_binary_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  fsm@robots.ox.ac.uk
*/

// Purpose: save and load vcl_vector<T>s in binary format.

#include <vcl/vcl_iosfwd.h>
#include <vcl/vcl_vector.h>

template </*typename*/class T> void vbl_binary_save(ostream &, vcl_vector<T> const &);
template </*typename*/class T> void vbl_binary_load(istream &, vcl_vector<T> &);

#endif // vbl_binary_h_
