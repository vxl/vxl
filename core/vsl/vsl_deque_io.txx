// This is vsl_deque_io.txx

//:
// \file   
// \brief  binary IO functions for vcl_deque<T>
// \author K.Y.McGaul
// Implementation

#include <vsl/vsl_deque_io.h>
#include <vsl/vsl_binary_io.h>

//====================================================================================
//: Write deque to binary stream
template <class T>
void vsl_b_write(vsl_b_ostream& s, const vcl_deque<T>& v)
{
  vsl_b_write(s, v.size());
  for (unsigned i=0; i<v.size(); i++)
    vsl_b_write(s,v[i]);
}

//====================================================================================
//: Read deque from binary stream
template <class T>
void vsl_b_read(vsl_b_istream& s, vcl_deque<T>& v)
{
  unsigned deque_size;
  vsl_b_read(s, deque_size);
  v.resize(deque_size);
  for (unsigned i=0; i<deque_size; i++)
    vsl_b_read(s,v[i]);
}

//====================================================================================
//: Output a human readable summary to the stream
template <class T>
void vsl_print_summary(vcl_ostream& os, const vcl_deque<T> &v)
{
  os << "Deque length: " << v.size() << vcl_endl;
  for (int i=0; i<v.size() && i<5; i++)
  {
    os << " " << i << ": ";
    vsl_print_summary(os,v[i]);
    os << vcl_endl;
  }
  if (v.size() > 5)
    os << " ..." << vcl_endl;
}


#define VSL_DEQUE_INSTANTIATE(T) \
template void vsl_print_summary(vcl_ostream&, const vcl_deque<T >&); \
template void vsl_b_write(vsl_b_ostream& s, const vcl_deque<T >& v); \
template void vsl_b_read(vsl_b_istream& s, vcl_deque<T >& v); \
;
