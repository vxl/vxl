// This is vsl_set_io.txx

//:
// \file   
// \brief  binary IO functions for vcl_set<T>
// \author K.Y.McGaul
// Implementation

#include <vsl/vsl_set_io.h>
#include <vsl/vsl_binary_io.h>

//====================================================================================
//: Write set to binary stream
template <class T>
void vsl_b_write(vsl_b_ostream& s, const vcl_set<T>& v)
{
  const short version_no = 1;
  vsl_b_write(s, version_no);
  vsl_b_write(s, v.size());
  for (vcl_set<T>::const_iterator iter = v.begin(); iter != v.end(); iter++)
    vsl_b_write(s,*iter);
}

//====================================================================================
//: Read set from binary stream
template <class T>
void vsl_b_read(vsl_b_istream& s, vcl_set<T>& v)
{
  unsigned set_size;
  short ver;
  vsl_b_read(s, ver);
  switch (ver)
  {
  case 1:
    vsl_b_read(s, set_size);
    for (unsigned i=0; i<set_size; i++)
    {
      T tmp;
      vsl_b_read(s,tmp);
      v.insert(tmp);
    } 
    break;
  default:
    vcl_cerr << "vsl_b_read(s, vcl_set<T>&) Unknown version number "<< ver << vcl_endl;
    vcl_abort();
  }
}

//====================================================================================
//: Output a human readable summary to the stream
template <class T>
void vsl_print_summary(vcl_ostream& os, const vcl_set<T> &v)
{
  os << "Set length: " << v.size() << vcl_endl;
  unsigned i =0;
  for (vcl_set<T>::const_iterator iter = v.begin(); iter != v.end() 
    && i<5; iter++, i++)
  {
    os << " " << i << ": ";
    vsl_print_summary(os, *iter);
    os << vcl_endl;
  }
  if (v.size() > 5)
    os << " ..." << vcl_endl;
}

#define VSL_SET_INSTANTIATE(T) \
template void vsl_print_summary(vcl_ostream& s, const vcl_set<T >& v); \
template void vsl_b_write(vsl_b_ostream& s, const vcl_set<T >& v); \
template void vsl_b_read(vsl_b_istream& s, vcl_set<T >& v); \
;
