//This is vxl/vsl/vsl_vector_io.txx

//:
// \file 
// \brief binary IO functions for vcl_vector<T>
// \author Tim Cootes
// Implementation

#include <vsl/vsl_vector_io.h>
#include <vsl/vsl_binary_io.h>

//====================================================================================
//: Write vector to binary stream
template <class T>
void vsl_b_write(vsl_b_ostream& s, const vcl_vector<T>& v)
{
  int n = v.size();
  vsl_b_write(s,n);
  for (int i=0;i<n;++i)
    vsl_b_write(s,v[i]);
}

//====================================================================================
//: Read vector from binary stream
template <class T>
void vsl_b_read(vsl_b_istream& s, vcl_vector<T>& v)
{
  int n;
  vsl_b_read(s,n);
  v.resize(n);
  for (int i=0;i<n;++i)
    vsl_b_read(s,v[i]);
}

//====================================================================================
//: Output a human readable summary to the stream
template <class T>
void vsl_print_summary(vcl_ostream& os, const vcl_vector<T> &v)
{
  os << "Vector length: " << v.size() << vcl_endl;
  for (int i=0; i<v.size() && i<5; i++)
  {
    os << " " << i << ": ";
    vsl_print_summary(os, v[i]);
    os << vcl_endl;
  }
  if (v.size() > 5) 
    os << " ..." << vcl_endl;
}

#define VSL_VECTOR_INSTANTIATE(T) \
template void vsl_print_summary(vcl_ostream& s, const vcl_vector<T >& v); \
template void vsl_b_write(vsl_b_ostream& s, const vcl_vector<T >& v); \
template void vsl_b_read(vsl_b_istream& s, vcl_vector<T >& v); \
;
