// This is vxl/vsl/vsl_list_io.txx

//:
// \file   
// \brief  binary IO functions for vcl_list<T>
// \author K.Y.McGaul
// Implementation

#include <vsl/vsl_list_io.h>
#include <vsl/vsl_binary_io.h>

//====================================================================================
//: Write list to binary stream
template <class T>
void vsl_b_write(vsl_b_ostream& s, const vcl_list<T>& v)
{
  vsl_b_write(s, v.size());
  for (vcl_list<T>::const_iterator iter = v.begin(); iter != v.end(); iter++)
    vsl_b_write(s,*iter);
}

//====================================================================================
//: Read list from binary stream
template <class T>
void vsl_b_read(vsl_b_istream& s, vcl_list<T>& v)
{
  unsigned list_size;
  vsl_b_read(s, list_size);
  for (unsigned i=0; i<list_size; i++)
  {
    T tmp;
    vsl_b_read(s,tmp);
    v.push_back(tmp);
  } 
}

//====================================================================================
//: Output a human readable summary to the stream
template <class T>
void vsl_print_summary(vcl_ostream& os, const vcl_list<T> &v)
{
  unsigned i=0;
  os << "List length: " << v.size() << vcl_endl;
  for (vcl_list<T>::const_iterator iter = v.begin(); iter != v.end()
    && i<5; iter++, i++)
  {
    os << " " << i << ": ";
    vsl_print_summary(os, *iter);
    os << vcl_endl;
  }
  if (v.size() > 5)
    os << " ..." << vcl_endl;
}

#define VSL_LIST_INSTANTIATE(T) \
template void vsl_print_summary(vcl_ostream&, const vcl_list<T >&); \
template void vsl_b_write(vsl_b_ostream& s, const vcl_list<T >& v); \
template void vsl_b_read(vsl_b_istream& s, vcl_list<T >& v); \
;
