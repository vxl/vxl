#ifndef vbl_sparse_array_2d_base_h_
#define vbl_sparse_array_2d_base_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// .NAME    vbl_sparse_array_2d_base
// .LIBRARY vbl
// .HEADER  vxl package
// .INCLUDE vbl/vbl_sparse_array_2d_base.h
// .FILE    vbl_sparse_array_2d_base.cxx
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 29 Jun 97
//
// .SECTION Modifications
//    970629 AWF Initial version.
//
//-----------------------------------------------------------------------------

struct vbl_sparse_array_2d_base {

  // protected: should be ,  but need templated friends...
  static unsigned encode(unsigned i, unsigned j) {
    return (i << 16) | j;
  }

  static void decode(unsigned v, unsigned& i, unsigned& j) {
    j = v & 0xffff, i = (v >> 16) & 0xffff;
  }
};

#endif // vbl_sparse_array_2d_base_h_
