#ifndef vbl_index_3d_h_
#define vbl_index_3d_h_
#ifdef __GNUC__
#pragma interface
#endif
//: \author IMS

//: An index for 3d arrays
// Used specifically with vbl_sparse_array_3d, but can be used
// for other things.
struct vbl_index_3d
{
  vbl_index_3d() { }
  
  //: Construct 3d index.
  vbl_index_3d(unsigned index_i, unsigned index_j,
               unsigned index_k):
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
inline bool operator< (const vbl_index_3d &a, const vbl_index_3d &b)
{
  if (a.i != b.i) return a.i < b.i;
  if (a.j != b.j) return a.j < b.j;
  if (a.k != b.k) return a.k < b.k;
  return false;
  //if (a.i == other.i) return (a.j < other.j);
  //else return (a.i < other.i);
}

#endif
