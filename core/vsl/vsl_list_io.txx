// This is ./vxl/vsl/vsl_list_io.txx
#ifndef vsl_list_io_txx_
#define vsl_list_io_txx_

//:
// \file
// \brief  binary IO functions for vcl_list<T>
// \author K.Y.McGaul
//
// Implementation

#include <vsl/vsl_list_io.h>
#include <vsl/vsl_binary_io.h>
#include <vcl_iostream.h>

//====================================================================================
//: Write list to binary stream
template <class T>
void vsl_b_write(vsl_b_ostream& s, const vcl_list<T>& v)
{
  const short version_no = 1;
  vsl_b_write(s, version_no);
  vsl_b_write(s, v.size());
  for (typename vcl_list<T>::const_iterator iter = v.begin(); iter != v.end(); iter++)
    vsl_b_write(s,*iter);
}

//====================================================================================
//: Read list from binary stream
template <class T>
void vsl_b_read(vsl_b_istream& is, vcl_list<T>& v)
{
  if (!is) return;

  v.clear();
  unsigned list_size;
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
  {
  case 1:
    vsl_b_read(is, list_size);
    for (unsigned i=0; i<list_size; i++)
    {
      T tmp;
      vsl_b_read(is,tmp);
      v.push_back(tmp);
    }
    break;
  default:
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vcl_list<T>&) \n";
    vcl_cerr << "           Unknown version number "<< ver << "\n";
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

//====================================================================================
//: Output a human readable summary to the stream
template <class T>
void vsl_print_summary(vcl_ostream& os, const vcl_list<T> &v)
{
  unsigned i=0;
  os << "List length: " << v.size() << vcl_endl;
  for (typename vcl_list<T>::const_iterator iter = v.begin();
       iter != v.end() && i<5; ++iter,++i)
  {
    os << " " << i << ": ";
    vsl_print_summary(os, *iter);
    os << vcl_endl;
  }
  if (v.size() > 5)
    os << " ..." << vcl_endl;
}

#define VSL_LIST_IO_INSTANTIATE(T) \
template void vsl_print_summary(vcl_ostream&, const vcl_list<T >&); \
template void vsl_b_write(vsl_b_ostream& s, const vcl_list<T >& v); \
template void vsl_b_read(vsl_b_istream& s, vcl_list<T >& v)

#endif // vsl_list_io_txx_
