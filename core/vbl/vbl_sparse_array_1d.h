// This is core/vbl/vbl_sparse_array_1d.h
#ifndef vbl_sparse_array_1d_h_
#define vbl_sparse_array_1d_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Sparse array allowing space efficient access of the form s[3000]=2.
// \author  Andrew W. Fitzgibbon, Oxford RRG
// \date    02 Oct 96
//
// \verbatim
//  Modifications
//   Ian Scott (Manchester, ISBE) rewritten to use vbl_sparse_array_base
// \endverbatim
//---------------------------------------------------------------------------

#include <vbl/vbl_sparse_array_base.h>
#include <vcl_iostream.h>

//: Sparse array allowing space efficient access of the form s[3000] = 2;
template <class T>
class vbl_sparse_array_1d: public vbl_sparse_array_base<T, unsigned>
{
 public:
  typedef typename vbl_sparse_array_base<T,unsigned>::const_iterator const_iterator;

  //: Print the Array to a stream in "(i,j): value" format.
  vcl_ostream& print(vcl_ostream& out) const
  {
    for (const_iterator p = this->begin(); p != this->end(); ++p)
      out << '(' << (*p).first << "): " << (*p).second << vcl_endl;
    return out;
  }
};

//: Stream operator - print the Array to a stream in "(i,j): value" format.
template <class T>
inline vcl_ostream& operator<< (vcl_ostream& s, const vbl_sparse_array_1d<T>& a)
{
  return a.print(s);
}

#define VBL_SPARSE_ARRAY_1D_INSTANTIATE(T) \
extern "please include vbl/vbl_sparse_array_1d.txx instead"

#endif // vbl_sparse_array_1d_h_
