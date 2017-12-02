// This is core/vbl/io/vbl_io_sparse_array_base.hxx
#ifndef vbl_io_sparse_array_base_hxx_
#define vbl_io_sparse_array_base_hxx_
//:
// \file

#include <iostream>
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
    std::pair<Index, T> tt((*s).first, (*s).second);
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

    std::pair<Index, T> value;
    for (unsigned i=0; i<size; i++){
      vsl_b_read(is, value);
      p(value.first)=value.second;
    }
    break;
   }

   default:
    std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vbl_sparse_array_base<T, Index> &)\n"
             << "           Unknown version number "<< v << '\n';
    is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}


//==========================================================================
//: Output a human readable summary to the stream
template<class T, class Index>
void vsl_print_summary(std::ostream& os,const vbl_sparse_array_base<T, Index> & p)
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
  template void vsl_print_summary(std::ostream &, const vbl_sparse_array_base<T , I > &); \
  template void vsl_b_read(vsl_b_istream &, vbl_sparse_array_base<T , I > &); \
  template void vsl_b_write(vsl_b_ostream &, const vbl_sparse_array_base<T , I > &)

#endif // vbl_io_sparse_array_base_hxx_
