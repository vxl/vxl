#ifndef vbl_index_3d_h_
#define vbl_index_3d_h_
#ifdef __GNUC__
#pragma interface
#endif
// This is vxl/vbl/vbl_index_3d.h

//:
// \file
// \brief An index for 3d arrays
// \author Ian M. Scott, Manchester ISBE, 10 April 2001
// \verbatim
// Modifications:
// 10 April 2001 Peter Vanroose - corrected operator<
// 11 April 2001 Peter Vanroose - moved out of vbl_sparse_array_3d.h
// \endverbatim
//---------------------------------------------------------------------------


//: An index for 3d arrays
// Used specifically with vbl_sparse_array_3d, but can be used
// for other things.
struct vbl_index_3d
{
  vbl_index_3d() { }

  //: Construct 3d index.
  vbl_index_3d(unsigned index_i, unsigned index_j, unsigned index_k):
    i(index_i), j(index_j), k(index_k) {}

  unsigned i;
  unsigned j;
  unsigned k;
};

//: Equality operator
inline bool operator==(const vbl_index_3d &a, const vbl_index_3d &b)
{
  return (a.i == b.i) && (a.j == b.j) && (a.k == b.k);
}

//: Operator !=
inline bool operator!=(const vbl_index_3d &a, const vbl_index_3d &b)
{
  return !(a == b);
}

//: Defines an ordering on vbl_index_3d
inline bool operator< (const vbl_index_3d &a, const vbl_index_3d &other)
{
  if (a.i == other.i && a.j == other.j) return (a.k < other.k);
  else if (a.i == other.i) return (a.j < other.j);
  else return (a.i < other.i);
}

#endif // vbl_index_3d_h_
