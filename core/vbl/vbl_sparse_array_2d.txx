// This is vxl/vbl/vbl_sparse_array_2d.txx

//-*- c++ -*-------------------------------------------------------------------
//

// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 02 Oct 96
//
//-----------------------------------------------------------------------------

#include "vbl_sparse_array_2d.h"

#include <vcl_cassert.h>
#include <vcl_iostream.h>

//: Print the Array to a stream in "(i,j): value" format.
template <class T>
vcl_ostream& vbl_sparse_array_2d<T>::print(vcl_ostream& out) const
{
  const_iterator enD = storage_.end();
  for(const_iterator p = storage_.begin(); p != enD; ++p) {
    unsigned i,j;
    vbl_sparse_array_2d_base::decode((*p).first, i, j);
    out << "(" << i << "," << j << "): " << (*p).second << vcl_endl;
  }
  return out;
}

#undef VBL_SPARSE_ARRAY_2D_INSTANTIATE_base
#define VBL_SPARSE_ARRAY_2D_INSTANTIATE_base(T)\
template class vbl_sparse_array_2d<T>

#undef VBL_SPARSE_ARRAY_2D_INSTANTIATE 
#define VBL_SPARSE_ARRAY_2D_INSTANTIATE(T) \
VBL_SPARSE_ARRAY_2D_INSTANTIATE_base(T); \
VCL_INSTANTIATE_INLINE(vcl_ostream& operator<< (vcl_ostream&, const vbl_sparse_array_2d<T> &))
 

#ifdef MAIN
#undef MAIN
#include "vbl_sparse_array.txx"

int main()
{
  vbl_sparse_array_2d<double> x;

  x(1,2) = 1.23;
  x(100,200) = 100.2003;

  vcl_cout << "2,3 was " << x.put(2,3, 7) << vcl_endl;
  vcl_cout << "2,3 is " << *x.get_addr(2,3) << vcl_endl;

  vcl_cout << "12 = " << x(1,2) << vcl_endl;
  vcl_cout << "22 = " << x(2,2) << vcl_endl;

  vcl_cout << "100, 200 = " << x(100,200) << vcl_endl;

  vcl_cout << "333 is full? " << x.fullp(3,3) << vcl_endl;
  
  vcl_cout << x;
}
#endif
