#ifndef vbl_binary_h_
#define vbl_binary_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME vbl_binary - save and load vcl_vector<T>s in binary format
// .LIBRARY vbl
// .HEADER vxl package
// .INCLUDE vbl/vbl_binary.h
// .FILE vbl_binary.txx
// .SECTION Author
//  fsm@robots.ox.ac.uk

#include <vcl/vcl_iosfwd.h>
#include <vcl/vcl_vector.h>

template </*typename*/class T> void vbl_binary_save(ostream &, vcl_vector<T> const &);
template </*typename*/class T> void vbl_binary_load(istream &, vcl_vector<T> &);

#endif // vbl_binary_h_
