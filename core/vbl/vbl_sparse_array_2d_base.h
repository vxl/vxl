#ifndef vbl_sparse_array_2d_base_h_
#define vbl_sparse_array_2d_base_h_
#ifdef __GNUC__
#pragma interface
#endif
// This is vxl/vbl/vbl_sparse_array_2d_base.h

//:
// \file
// \brief base class for 2d sparse array.
// \author Andrew W. Fitzgibbon, Oxford RRG, 29 Jun 97
// \verbatim
// Modifications
//    970629 AWF Initial version
// \endverbatim
//
//----------------------------------------------------------------------------

//: contains encode and decode functions to go from 2d coordinates 
// to a single key
struct vbl_sparse_array_2d_base {

  // protected: should be ,  but need templated friends...
  //: calculate a key from (i,j)
  static unsigned encode(unsigned i, unsigned j) {
    return (i << 16) | j;
  }

  //: retrieve (i,j) from a key v
  static void decode(unsigned v, unsigned& i, unsigned& j) {
    j = v & 0xffff, i = (v >> 16) & 0xffff;
  }
};

#endif // vbl_sparse_array_2d_base_h_
