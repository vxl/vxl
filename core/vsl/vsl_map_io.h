// This is vxl/vsl/vsl_map_io.h
#ifndef vsl_map_io_h_
#define vsl_map_io_h_
//:
// \file 
// \brief binary IO functions for vcl_map<Key, T, Compare>
// \author K.Y.McGaul

#include <vcl_iosfwd.h>
#include <vcl_map.h>

class vsl_b_ostream;
class vsl_b_istream;

//: Write map to binary stream
template <class Key, class T, class Compare>
void vsl_b_write(vsl_b_ostream& s, const vcl_map<Key, T, Compare>& v);

//: Read map from binary stream
template <class Key, class T, class Compare>
void vsl_b_read(vsl_b_istream& s, vcl_map<Key, T, Compare>& v);

//: Print human readable summary of object to a stream
template <class Key, class T, class Compare>
void vsl_print_summary(vcl_ostream & os,const vcl_map<Key, T, Compare> &v);

//: Write map to binary stream
template <class Key, class T, class Compare>
void vsl_b_write(vsl_b_ostream& s, const vcl_multimap<Key, T, Compare>& v);

//: Read map from binary stream
template <class Key, class T, class Compare>
void vsl_b_read(vsl_b_istream& s, vcl_multimap<Key, T, Compare>& v);

//: Print human readable summary of object to a stream
template <class Key, class T, class Compare>
void vsl_print_summary(vcl_ostream & os,const vcl_multimap<Key, T, Compare> &v);

#endif // vsl_map_io_h_
