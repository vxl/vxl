// This is ./vxl/vsl/vsl_set_io.txx
#ifndef vsl_set_io_txx_
#define vsl_set_io_txx_

//:
// \file
// \brief  binary IO functions for vcl_set<T>
// \author K.Y.McGaul
//
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
  for (typename vcl_set<T>::const_iterator iter = v.begin(); iter != v.end(); iter++)
    vsl_b_write(s,*iter);
}

//====================================================================================
//: Read set from binary stream
template <class T>
void vsl_b_read(vsl_b_istream& is, vcl_set<T>& v)
{
  if (!is) return;

  v.clear();
  unsigned set_size;
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
  {
  case 1:
    vsl_b_read(is, set_size);
    for (unsigned i=0; i<set_size; i++)
    {
      T tmp;
      vsl_b_read(is,tmp);
      v.insert(tmp);
    }
    break;
  default:
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vcl_set<T>&) \n";
    vcl_cerr << "           Unknown version number "<< ver << "\n";
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

//====================================================================================
//: Output a human readable summary to the stream
template <class T>
void vsl_print_summary(vcl_ostream& os, const vcl_set<T> &v)
{
  os << "Set length: " << v.size() << vcl_endl;
  unsigned i =0;
  for (typename vcl_set<T>::const_iterator iter = v.begin();
       iter != v.end() && i<5; ++iter,++i)
  {
    os << " " << i << ": ";
    vsl_print_summary(os, *iter);
    os << vcl_endl;
  }
  if (v.size() > 5)
    os << " ..." << vcl_endl;
}


#define VSL_SET_IO_INSTANTIATE(T) \
template void vsl_print_summary(vcl_ostream& s, const vcl_set<T >& v); \
template void vsl_b_write(vsl_b_ostream& s, const vcl_set<T >& v); \
template void vsl_b_read(vsl_b_istream& s, vcl_set<T >& v)

#endif // vsl_set_io_txx_
