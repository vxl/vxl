// This is core/vsl/vsl_map_io.hxx
#ifndef vsl_map_io_hxx_
#define vsl_map_io_hxx_
//:
// \file
// \brief Implementation binary IO functions for vcl_(multi)map<Key, T, Compare>
// \author K.Y.McGaul
//
// \verbatim
//  Modifications
//   IMS - 22 June 2001 - Added IO for multimap
// \endverbatim

#include <iostream>
#include "vsl_map_io.h"
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_indent.h>
#include <vsl/vsl_pair_io.h>

//====================================================================================
//: Write map to binary stream
template <class Key, class T, class Compare>
void vsl_b_write(vsl_b_ostream& s, const std::map<Key, T, Compare>& v)
{
  constexpr short version_no = 1;
  vsl_b_write(s, version_no);
  vsl_b_write(s, v.size());
  for (typename std::map<Key, T, Compare>::const_iterator iter = v.begin(); iter != v.end(); iter++)
  {
    vsl_b_write(s,(*iter).first);
    vsl_b_write(s,(*iter).second);
  }
}

//====================================================================================
//: Read map from binary stream
template <class Key, class T, class Compare>
void vsl_b_read(vsl_b_istream& is, std::map<Key, T, Compare>& v)
{
  if (!is) return;

  v.clear();
  unsigned map_size;
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
  {
  case 1:
    vsl_b_read(is, map_size);
    for (unsigned i=0; i<map_size; i++)
    {
      Key first_val;
      T second_val;
      vsl_b_read(is, first_val);
      vsl_b_read(is, second_val);
      v[first_val] = second_val;
    }
    break;
  default:
    std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, std::map<K, T>&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

//====================================================================================
//: Output a human readable summary to the stream
template <class Key, class T, class Compare>
void vsl_print_summary(std::ostream& os, const std::map<Key, T, Compare> &v)
{
  os << "Map size: " << v.size() << '\n';
  unsigned i=0;
  for (typename std::map<Key, T, Compare>::const_iterator iter = v.begin();
       iter != v.end() && i< 5; ++iter,++i)
  {
    os << vsl_indent() << ' ' << i << ": ";
    vsl_print_summary(os, (*iter).first);
    os << ", ";
    vsl_indent_inc(os);
    vsl_print_summary(os, (*iter).second);
    os << '\n';
    vsl_indent_dec(os);
 }
  if (v.size() > 5)
    os << " ...\n";
}


#define VSL_MAP_IO_INSTANTIATE(Key, T, Compare) \
template void vsl_print_summary(std::ostream&, const std::map<Key, T, Compare >&); \
template void vsl_b_write(vsl_b_ostream& s, const std::map<Key, T, Compare >& v); \
template void vsl_b_read(vsl_b_istream& s, std::map<Key, T, Compare >& v)

//====================================================================================
//: Write multimap to binary stream
template <class Key, class T, class Compare>
void vsl_b_write(vsl_b_ostream& s, const std::multimap<Key, T, Compare>& v)
{
  constexpr short version_no = 1;
  vsl_b_write(s, version_no);
  vsl_b_write(s, v.size());
  for (typename std::multimap<Key, T, Compare>::const_iterator iter = v.begin();
       iter != v.end(); iter++)
  {
    vsl_b_write(s,(*iter).first);
    vsl_b_write(s,(*iter).second);
  }
}

//====================================================================================
//: Read multimap from binary stream
template <class Key, class T, class Compare>
void vsl_b_read(vsl_b_istream& is, std::multimap<Key, T, Compare>& v)
{
  if (!is) return;

  v.clear();
  unsigned multimap_size;
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
  {
  case 1:
    vsl_b_read(is, multimap_size);
    for (unsigned i=0; i<multimap_size; i++)
    {
      Key first_val;
      T second_val;
      vsl_b_read(is, first_val);
      vsl_b_read(is, second_val);
      v.insert( std::make_pair(first_val, second_val));
    }
    break;
  default:
    std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, std::multimap<K, T>&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

//====================================================================================
//: Output a human readable summary to the stream
template <class Key, class T, class Compare>
void vsl_print_summary(std::ostream& os, const std::multimap<Key, T, Compare> &v)
{
  os << "multimap size: " << v.size() << '\n';
  unsigned i=0;
  for (typename std::multimap<Key, T, Compare>::const_iterator iter = v.begin();
       iter != v.end() && i< 5; ++iter,++i)
  {
    os << ' ' << i << ": ";
    vsl_print_summary(os, (*iter).first);
    os << ", ";
    vsl_print_summary(os, (*iter).second);
    os << '\n';
  }
  if (v.size() > 5)
    os << " ...\n";
}


#define VSL_MULTIMAP_IO_INSTANTIATE(Key, T, Compare) \
template void vsl_print_summary(std::ostream&, const std::multimap<Key, T, Compare >&); \
template void vsl_b_write(vsl_b_ostream& s, const std::multimap<Key, T, Compare >& v); \
template void vsl_b_read(vsl_b_istream& s, std::multimap<Key, T, Compare >& v)

#endif // vsl_map_io_hxx_
