#ifndef vbl_sparse_array_1d_h_
#define vbl_sparse_array_1d_h_
#ifdef __GNUC__
#pragma interface
#endif
// This is vxl/vbl/vbl_sparse_array_1d.h

//:
// \file
// \brief Sparse array allowing space efficient access of the form s[3000]=2.
// \author  Andrew W. Fitzgibbon, Oxford RRG
// \date    02 Oct 96
//
// \verbatim
// Modifications:
//     Ian Scott (Manchester, ISBE) rewritten to use vbl_sparse_array_base
// \endverbatim
//---------------------------------------------------------------------------

#include <vbl/vbl_sparse_array_base.h>
#include <vcl_iosfwd.h>

//: Sparse array allowing space efficient access of the form s[3000] = 2;
template <class T>
class vbl_sparse_array_1d: public vbl_sparse_array_base<T, unsigned>
{
    //: Print the Array to a stream in "(i,j): value" format.
  vcl_ostream& print(vcl_ostream&) const;
};

#define VBL_SPARSE_ARRAY_1D_INSTANTIATE(T) \
extern "please include vbl/vbl_sparse_array_1d.txx instead"

#endif // vbl_sparse_array_1d_h_
