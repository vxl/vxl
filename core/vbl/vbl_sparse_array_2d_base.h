//-*- c++ -*-------------------------------------------------------------------
#ifndef vbl_sparse_array_2d_base_h_
#define vbl_sparse_array_2d_base_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// .LIBRARY     vbl
// .HEADER	vxl package
// .INCLUDE     vbl/vbl_sparse_array_2d_base.h
// .FILE        vbl/vbl_sparse_array_2d_base.cxx
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 29 Jun 97
//
// .SECTION Modifications
//    970629 AWF Initial version.
//
//-----------------------------------------------------------------------------

//: a class that awf hasn't documented properly. FIXME
struct vbl_sparse_array_2d_base {

  // protected: should be ,  but need templated friends...
  static unsigned encode(unsigned i, unsigned j) {
    return (i << 16) | j;
  }

  static void decode(unsigned v, unsigned& i, unsigned& j) {
    j = v & 0xffff, i = (v >> 16) & 0xffff;
  }
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vbl_sparse_array_2d_base.
