// This is core/vsl/vsl_map_io.txx
#ifndef vsl_map_io_txx_
#define vsl_map_io_txx_
//:
// \file
// \brief Implementation binary IO functions for vcl_(multi)map<Key, T, Compare>
// \author K.Y.McGaul
//
// \verbatim
//  Modifications:
//   IMS - 22 June 2001 - Added IO for multimap
// \endverbatim

#include "vsl_map_io.h"
#include <vsl/vsl_binary_io.h>

//====================================================================================
//: Write map to binary stream
template <class Key, class T, class Compare>
void vsl_b_write(vsl_b_ostream& s, const vcl_map<Key, T, Compare>& v)
{
  const short version_no = 1;
  vsl_b_write(s, version_no);
  vsl_b_write(s, v.size());
  for (typename vcl_map<Key, T, Compare>::const_iterator iter = v.begin(); iter != v.end(); iter++)
  {
    vsl_b_write(s,(*iter).first);
    vsl_b_write(s,(*iter).second);
  }
}

//====================================================================================
//: Read map from binary stream
template <class Key, class T, class Compare>
void vsl_b_read(vsl_b_istream& is, vcl_map<Key, T, Compare>& v)
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
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vcl_map<K, T>&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

//====================================================================================
//: Output a human readable summary to the stream
template <class Key, class T, class Compare>
void vsl_print_summary(vcl_ostream& os, const vcl_map<Key, T, Compare> &v)
{
  os << "Map size: " << v.size() << vcl_endl;
  unsigned i=0;
  for (typename vcl_map<Key, T, Compare>::const_iterator iter = v.begin();
       iter != v.end() && i< 5; ++iter,++i)
  {
    os << ' ' << i << ": ";
    vsl_print_summary(os, (*iter).first);
    os << ", ";
    vsl_print_summary(os, (*iter).second);
    os << vcl_endl;
  }
  if (v.size() > 5)
    os << " ...\n";
}


#define VSL_MAP_IO_INSTANTIATE(Key, T, Compare) \
template void vsl_print_summary(vcl_ostream&, const vcl_map<Key, T, Compare >&); \
template void vsl_b_write(vsl_b_ostream& s, const vcl_map<Key, T, Compare >& v); \
template void vsl_b_read(vsl_b_istream& s, vcl_map<Key, T, Compare >& v)

//====================================================================================
//: Write multimap to binary stream
template <class Key, class T, class Compare>
void vsl_b_write(vsl_b_ostream& s, const vcl_multimap<Key, T, Compare>& v)
{
  const short version_no = 1;
  vsl_b_write(s, version_no);
  vsl_b_write(s, v.size());
  for (typename vcl_multimap<Key, T, Compare>::const_iterator iter = v.begin();
       iter != v.end(); iter++)
  {
    vsl_b_write(s,(*iter).first);
    vsl_b_write(s,(*iter).second);
  }
}

//====================================================================================
//: Read multimap from binary stream
template <class Key, class T, class Compare>
void vsl_b_read(vsl_b_istream& is, vcl_multimap<Key, T, Compare>& v)
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
      v.insert( vcl_make_pair(first_val, second_val));
    }
    break;
  default:
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vcl_multimap<K, T>&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

//====================================================================================
//: Output a human readable summary to the stream
template <class Key, class T, class Compare>
void vsl_print_summary(vcl_ostream& os, const vcl_multimap<Key, T, Compare> &v)
{
  os << "multimap size: " << v.size() << vcl_endl;
  unsigned i=0;
  for (typename vcl_multimap<Key, T, Compare>::const_iterator iter = v.begin();
       iter != v.end() && i< 5; ++iter,++i)
  {
    os << ' ' << i << ": ";
    vsl_print_summary(os, (*iter).first);
    os << ", ";
    vsl_print_summary(os, (*iter).second);
    os << vcl_endl;
  }
  if (v.size() > 5)
    os << " ...\n";
}


#define VSL_MULTIMAP_IO_INSTANTIATE(Key, T, Compare) \
template void vsl_print_summary(vcl_ostream&, const vcl_multimap<Key, T, Compare >&); \
template void vsl_b_write(vsl_b_ostream& s, const vcl_multimap<Key, T, Compare >& v); \
template void vsl_b_read(vsl_b_istream& s, vcl_multimap<Key, T, Compare >& v)

#endif // vsl_map_io_txx_
