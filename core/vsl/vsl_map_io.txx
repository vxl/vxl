// This is vxl/vsl/vsl_map_io.txx

//:
// \file  
// \brief  binary IO functions for vcl_map<Key, T, Compare>
// \author K.Y.McGaul
// Implementation

#include <vsl/vsl_map_io.h>
#include <vsl/vsl_binary_io.h>

//====================================================================================
//: Write map to binary stream
template <class Key, class T, class Compare>
void vsl_b_write(vsl_b_ostream& s, const vcl_map<Key, T, Compare>& v)
{
  vsl_b_write(s, v.size());
  for (vcl_map<Key, T, Compare>::const_iterator iter = v.begin(); iter != v.end(); iter++)
  {
    vsl_b_write(s,(*iter).first);
    vsl_b_write(s,(*iter).second);
  }
}

//====================================================================================
//: Read map from binary stream
template <class Key, class T, class Compare>
void vsl_b_read(vsl_b_istream& s, vcl_map<Key, T, Compare>& v)
{
  unsigned map_size;
  vsl_b_read(s, map_size);
  for (unsigned i=0; i<map_size; i++)
  {
    Key first_val;
    T second_val;
    vsl_b_read(s, first_val);
    vsl_b_read(s, second_val);
    v[first_val] = second_val;
  } 
}

//====================================================================================
//: Output a human readable summary to the stream
template <class Key, class T, class Compare>
void vsl_print_summary(vcl_ostream& os, const vcl_map<Key, T, Compare> &v)
{
  os << "Map size: " << v.size() << vcl_endl;
  unsigned i=0;
  for (vcl_map<Key, T, Compare>::const_iterator iter = v.begin(); iter != v.end() 
    && i< 5; iter++,i++)
  {
    os << " " << i << ": ";
    vsl_print_summary(os, (*iter).first);
    os << ", ";
    vsl_print_summary(os, (*iter).second);
    os << vcl_endl;
  }
  if (v.size() > 5)
    os << " ..." << vcl_endl;
}


#define VSL_MAP_IO_INSTANTIATE(Key, T, Compare) \
template void vsl_print_summary(vcl_ostream&, const vcl_map<Key, T, Compare >&); \
template void vsl_b_write(vsl_b_ostream& s, const vcl_map<Key, T, Compare >& v); \
template void vsl_b_read(vsl_b_istream& s, vcl_map<Key, T, Compare >& v); \
;
