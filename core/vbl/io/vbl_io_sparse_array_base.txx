// This is core/vbl/io/vbl_io_sparse_array_base.txx
#ifndef vbl_io_sparse_array_base_txx_
#define vbl_io_sparse_array_base_txx_
//:
// \file

#include "vbl_io_sparse_array_base.h"
#include <vsl/vsl_pair_io.h>
#include <vsl/vsl_binary_io.h>

//============================================================================
//: Binary save self to stream.
template<class T, class Index>
void vsl_b_write(vsl_b_ostream &os, const vbl_sparse_array_base<T, Index> & p)
{
  const short io_version_no = 1;
  vsl_b_write(os, io_version_no);

  vsl_b_write(os, p.count_nonempty());
  for (typename vbl_sparse_array_base<T, Index>::const_iterator s = p.begin(); s != p.end(); ++s){
    // the value_type of a map<Key, T> is "pair<Key const, T>", not "pair<Key, T>".
    vcl_pair<Index, T> tt((*s).first, (*s).second);
    vsl_b_write(os, tt);
  }
}

//===========================================================================
//: Binary load self from stream.
template<class T, class Index>
void vsl_b_read(vsl_b_istream &is, vbl_sparse_array_base<T, Index> & p)
{
  if (!is) return;

  p.clear();
  short v;
  vsl_b_read(is, v);

  switch (v)
  {
   case 1: {
    unsigned int size;
    vsl_b_read(is, size);

#ifdef VCL_SUNPRO_CC_50
    // SunPro 5.0 (CC -g -c) generates wrong code (duplicate symbols).
    Index value_first;
    T     value_second;
    for (unsigned i=0; i<size; i++){
      vsl_b_read(is, value_first);
      vsl_b_read(is, value_second);
      p(value_first) = value_second;
    }
#else
    vcl_pair<Index, T> value;
    for (unsigned i=0; i<size; i++){
      vsl_b_read(is, value);
      p(value.first)=value.second;
    }
#endif
    break;
   }

   default:
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vbl_sparse_array_base<T, Index> &)\n"
             << "           Unknown version number "<< v << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}


//==========================================================================
//: Output a human readable summary to the stream
template<class T, class Index>
void vsl_print_summary(vcl_ostream& os,const vbl_sparse_array_base<T, Index> & p)
{
  os<<"nonempty elements: "<< p.count_nonempty() << '\n';
  int k=0;

  for (typename vbl_sparse_array_base<T, Index>::const_iterator s = p.begin();
      s != p.end() && k<5; ++s)
  {
    k++;
    os << ' ';
    vsl_print_summary(os, (*s).first);
    os << ": ";
    vsl_print_summary(os, (*s).second);
    os << '\n';
  }
  if (p.count_nonempty() > 5)
    os << " ...\n";
}

#define VBL_IO_SPARSE_ARRAY_BASE_INSTANTIATE(T, I) \
  template void vsl_print_summary(vcl_ostream &, const vbl_sparse_array_base<T , I > &); \
  template void vsl_b_read(vsl_b_istream &, vbl_sparse_array_base<T , I > &); \
  template void vsl_b_write(vsl_b_ostream &, const vbl_sparse_array_base<T , I > &)

#endif // vbl_io_sparse_array_base_txx_
