#ifndef vbl_index_2d_h_
#define vbl_index_2d_h_
#ifdef __GNUC__
#pragma interface
#endif
// This is vxl/vbl/vbl_index_2d.h

//:
// \file
// \brief An index for 2d arrays
// \author Ian M. Scott, Manchester ISBE, 10 April 2001
// \verbatim
// Modifications:
// 11 April 2001 Peter Vanroose - moved out of vbl_sparse_array_2d.h
// \endverbatim
//---------------------------------------------------------------------------


//: An index for 2d arrays
// Used specifically with vbl_sparse_array_2d, but can be used
// for other things.
struct vbl_index_2d
{
  vbl_index_2d() { }

  vbl_index_2d(unsigned index_i, unsigned index_j): i(index_i), j(index_j) {} 
  unsigned i;
  unsigned j;
};

//: operator==
inline bool operator==(const vbl_index_2d &a, const vbl_index_2d &b)
{
  return (a.i == b.i) && (a.j == b.j);
}

//: operator!=
inline bool operator!=(const vbl_index_2d &a, const vbl_index_2d &b)
{
  return !(a == b);
}

//: Defines an ordering on vbl_index_2d
inline bool operator< (const vbl_index_2d &a, const vbl_index_2d &other)
{
  if (a.i == other.i) return (a.j < other.j);
  else return (a.i < other.i);
}

#endif // vbl_index_2d_h_
