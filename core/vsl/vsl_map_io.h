// This is core/vsl/vsl_map_io.h
#ifndef vsl_map_io_h_
#define vsl_map_io_h_
//:
// \file
// \brief binary IO functions for std::map<Key, T, Compare>
// \author K.Y.McGaul

#include <iosfwd>
#include <map>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class vsl_b_ostream;
class vsl_b_istream;

//: Write map to binary stream
template <class Key, class T, class Compare>
void vsl_b_write(vsl_b_ostream& s, const std::map<Key, T, Compare>& v);

//: Read map from binary stream
template <class Key, class T, class Compare>
void vsl_b_read(vsl_b_istream& s, std::map<Key, T, Compare>& v);

//: Print human readable summary of object to a stream
template <class Key, class T, class Compare>
void vsl_print_summary(std::ostream & os,const std::map<Key, T, Compare> &v);

//: Write map to binary stream
template <class Key, class T, class Compare>
void vsl_b_write(vsl_b_ostream& s, const std::multimap<Key, T, Compare>& v);

//: Read map from binary stream
template <class Key, class T, class Compare>
void vsl_b_read(vsl_b_istream& s, std::multimap<Key, T, Compare>& v);

//: Print human readable summary of object to a stream
template <class Key, class T, class Compare>
void vsl_print_summary(std::ostream & os,const std::multimap<Key, T, Compare> &v);

#endif // vsl_map_io_h_
