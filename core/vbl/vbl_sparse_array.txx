// This is vxl/vbl/vbl_sparse_array.txx
#ifndef vbl_sparse_array_txx_
#define vbl_sparse_array_txx_

// deprecated
#include <vcl_deprecated.h>
#include "vbl_sparse_array_1d.txx"

#undef VBL_SPARSE_ARRAY_INSTANTIATE
#define VBL_SPARSE_ARRAY_INSTANTIATE(T) VBL_SPARSE_ARRAY_1D_INSTANTIATE(T)

#ifdef MAIN
int main()
{
  vbl_sparse_array<double> x;

  x[1] = 1.23;
  x[100] = 100.2003;

  vcl_cout << "203 was " << x.put(203, 7) << vcl_endl;
  vcl_cout << "203 is " << *x.get_addr(203) << vcl_endl;

  vcl_cout << "1 = " << x[1] << vcl_endl;
  vcl_cout << "2 = " << x[2] << vcl_endl;

  vcl_cout << "100 = " << x[100] << vcl_endl;

  vcl_cout << "303 is full? " << x.fullp(303) << vcl_endl;

  vcl_cout << x;
}
#endif

#endif // vbl_sparse_array_txx_
